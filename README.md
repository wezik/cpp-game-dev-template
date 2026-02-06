# C++ Game development template

A C++ game dev project template. Containing libraries for ECS, graphics, audio and physics.

## Stack

- **C++23**
- **Flecs 4.1.1** - Entity Component System
- **Raylib 5.5** - Graphics and audio
- **Jolt Physics 5.2.0** - Physics engine

## Prerequisites

- [mise](https://mise.jdx.dev/) - Task automation and tool management
- C++23 compatible compiler (Clang 16+ preferred, GCC 13+)
- `python3-venv` - Required by mise for conan and meson

### System Dependencies

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

```bash
git clone https://github.com/wezik/cpp-game-dev-template.git
cd cpp-game-dev-template
mise run setup
```

Setup auto-detects your compiler and generates a repo-local conan profile (`conan/profile`).
Clang is preferred when available for best clangd LSP compatibility.

## Building

```bash
mise run build      # Configure and build (debug)
mise run dev        # Build and run
```

## IDE Setup

For clangd-based LSP (Neovim, VS Code, etc.), `compile_commands.json` is auto-symlinked
from the meson build directory after each build. Run a build first to generate it.

## Potential Issues

- **`python3-venv` not installed** - mise uses python3-venv for conan and meson.
  Install it with `sudo apt install python3-venv` (Debian/Ubuntu).
- **clangd not working properly with gcc** - The setup prefers clang because clangd
  works best with clang-generated compile commands. If only gcc is available, clangd
  may report false errors. Install clang for the best experience: `sudo apt install clang`.
- **SSE/AVX errors with Jolt Physics** - Jolt Physics uses SIMD intrinsics.
  The build uses `-march=native` to enable CPU-specific instruction sets.
  This means binaries are not portable across different CPU architectures.
- **New source files not compiling** - Meson requires explicit source file listing.
  When adding new `.cpp` files, add them to the `srcs` list in `meson.build`.
