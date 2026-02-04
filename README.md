# The Legend of Zelda: Twilight Princess Randomizer (Decomp Base)

A work-in-progress experimental mod that aims to randomize Twilight Princess.

**Note: This is not meant to compete with the REL Randomizer and is a seperate project, although work may be shared between them. No feature suggestions will be accepted for the time being.**

The code for the GameCube releases is completely matching. However, not every Translation Unit (TU) has been linked yet. Work is continuing by aligning the Debug version and getting the Wii versions to match. All versions are built from the same codebase using conditional compilation.

> [!IMPORTANT]
> This repository does **not** contain any game assets or assembly whatsoever. An existing copy of the game is required.
>
> This project itself **is not**, and will not, produce a port, to PC or any other platform.

The project can target the following supported versions:

- **`GZ2E01`**: GameCube - North America
- **`GZ2P01`**: GameCube - Europe/Australia
- **`GZ2J01`**: GameCube - Japan
- `RZDE01_00`: Wii - North America (Rev 0)
- `RZDE01_02`: Wii - North America (Rev 2)
- `RZDP01`: Wii - Europe/Australia
- `RZDJ01`: Wii - Japan
- `Shield`: Nvidia Shield - China
- `ShieldD`: Nvidia Shield - China (Debug Version)

<!--ts-->
- [Dependencies](#dependencies)
- [Building](#building)
- [Diffing](#diffing)
- [Contributing](#contributing)
- [FAQ](https://zsrtp.link/about)

## Dependencies

You will need the following dependencies:

- git
- ninja
- python3
- clang-format (optional)

### Windows

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

### macOS

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

### Linux

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

## Building

- Clone the repository:

  ```sh
  git clone https://github.com/zeldaret/tp.git
  ```

- Copy your game's disc image to `orig/GZ2E01`.
  - Supported formats: ISO (GCM), RVZ, WIA, WBFS, CISO, NFS, GCZ, TGC.
  - After the initial build, the disc image can be deleted to save space.

- Configure:

  ```sh
  python configure.py
  ```

  To use a version other than `GZ2E01` (GCN USA), specify it with `--version`.
- Build:

  ```sh
  ninja
  ```
