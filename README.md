# C++ Game development template

A C++ game dev project template. Containing libraries for ECS, graphics, audio and physics.

## Stack

- **C++23**
- **Flecs 4.1.4** - Entity Component System
- **Raylib 5.5** - Graphics and audio
- **Jolt Physics 5.5.0** - Physics engine

## Prerequisites

- CMake 3.28.3+
- Ninja
- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- [mise](https://mise.jdx.dev/) (optional, for task automation, and some dependency handling)

### System Dependencies

Run `mise run setup` to auto-detect and install or manually:

**Linux (Debian/Ubuntu):**

```bash
sudo apt-get install libgl1-mesa-dev libx11-dev libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev
```

**Linux (Fedora/RHEL):**

```bash
sudo dnf install mesa-libGL-devel libX11-devel libXcursor-devel libXinerama-devel libXrandr-devel libXi-devel
```

**Linux (Arch):**

```bash
sudo pacman -S mesa libx11 libxcursor libxinerama libxrandr libxi
```

**macOS/Windows:** No additional dependencies needed.

## Setup

1. Clone with submodules:

   ```bash
   git clone --recursive https://github.com/wezik/cpp-game-dev-template.git
   cd cpp-game-dev-template
   ```

2. If you forgot `--recursive`:

   ```bash
   git submodule update --init --recursive
   ```

## Building

### With mise (recommended)

```bash
mise run setup      # Install system deps (once per machine)
mise run build      # Configure and build (debug)
mise run dev        # Build and run
```

### Manual

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/cpp-game
```

### Release build

```bash
mise run build-release
mise run release
```

Or manually:

```bash
cmake --preset release
cmake --build --preset release
./build/release/cpp-game
```

## IDE Setup

For clangd-based LSP (Neovim, VS Code, etc.), the `.clangd` file points to the debug build's `compile_commands.json`. Run a debug build first to generate it.
