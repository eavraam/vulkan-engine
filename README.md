# Vulkan Engine

A learning project following the [Khronos Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/index.html), using **SDL3** for windowing and input instead of GLFW.

## Status

Window creation with SDL3 is working. Vulkan setup (instance, device, swapchain, ...) follows the tutorial chapter by chapter.

## Requirements

- Windows 10/11, Visual Studio 2022 with the "Desktop development with C++" workload (kept up to date)
- [Vulkan SDK](https://vulkan.lunarg.com/) 1.4.335 or newer (provides headers, validation layers and `slangc`)
- [vcpkg](https://github.com/microsoft/vcpkg)
- CMake 3.29 or newer (included with Visual Studio)

## Dependencies

Installed through vcpkg (see `scripts/install_dependencies_windows.bat`):

`sdl3[vulkan]`, `glm`, `tinyobjloader`, `stb`, `tinygltf`, `nlohmann-json`, `ktx[vulkan]`, `openal-soft`

Note that SDL3 must be built with the `vulkan` feature, otherwise window creation with `SDL_WINDOW_VULKAN` fails.

## Build

```powershell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

Or open `build/VulkanEngine.sln` in Visual Studio, set `VulkanEngine` as the startup project and press F5.

The executable, compiled shaders and assets end up in `build/VulkanEngine/`.

## Controls

- `Esc`: quit

## Project layout

```
CMake/        Find modules (FindVulkan.cmake)
assets/       Models and textures used by the tutorial
scripts/      Dependency install scripts
main.cpp      Application entry point
shader.slang  Slang shader, compiled to SPIR-V at build time
```

## Credits

Based on the [Khronos Vulkan Tutorial](https://github.com/KhronosGroup/Vulkan-Tutorial). Template files, assets and CMake modules originate from that repository and remain under its license terms.
