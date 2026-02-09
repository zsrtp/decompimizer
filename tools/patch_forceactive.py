#!/usr/bin/env python3

"""
Patches the DOL ldscript.lcf FORCEACTIVE section with all defined symbols
from custom object files. This ensures REL modules can resolve references
to functions/data defined in custom DOL code (e.g. rando.cpp).

Usage: python3 tools/patch_forceactive.py <ldscript.lcf> <nm_binary> <obj_file> [<obj_file> ...]
"""

import subprocess
import sys


def get_defined_symbols(nm_path: str, obj_path: str) -> list[str]:
    """Extract all defined global symbols from an object file using nm."""
    result = subprocess.run(
        [nm_path, "-g", obj_path],
        capture_output=True,
        text=True,
    )
    symbols = []
    for line in result.stdout.splitlines():
        parts = line.split()
        # Defined symbols have 3 columns: address, type, name
        # Undefined (U) and debug (N, V) symbols should be skipped
        if len(parts) == 3:
            sym_type = parts[1]
            sym_name = parts[2]
            if sym_type in ("T", "D", "B", "S"):
                symbols.append(sym_name)
    return symbols


def patch_ldscript(ldscript_path: str, symbols: list[str]) -> None:
    """Add symbols to the FORCEACTIVE section of the ldscript."""
    with open(ldscript_path, "r") as f:
        content = f.read()

    new_symbols = [s for s in symbols if s not in content]
    if not new_symbols:
        return

    insert = "\n".join(f"    {s}" for s in new_symbols)
    # Find the closing brace of FORCEACTIVE (last } in the file)
    content = content.rstrip()
    if content.endswith("}"):
        content = content[:-1] + insert + "\n}\n"

    with open(ldscript_path, "w") as f:
        f.write(content)


def main() -> None:
    if len(sys.argv) < 4:
        print(f"Usage: {sys.argv[0]} <ldscript.lcf> <nm_binary> <obj_file> [<obj_file> ...]")
        sys.exit(1)

    ldscript_path = sys.argv[1]
    nm_path = sys.argv[2]
    obj_files = sys.argv[3:]

    all_symbols: list[str] = []
    for obj_file in obj_files:
        all_symbols.extend(get_defined_symbols(nm_path, obj_file))

    if all_symbols:
        patch_ldscript(ldscript_path, all_symbols)


if __name__ == "__main__":
    main()
