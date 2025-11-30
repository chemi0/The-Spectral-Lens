# The Spectral Lens (Kostur)

2D OpenGL 3.3+ project. Large vendor binaries in `packages/` are intentionally excluded to keep the repo small.

## Structure
- `Kostur.sln`, `Kostur.vcxproj`, `Kostur.vcxproj.filters`
- `Source/`, `Header/`, `Shaders/`, `Resources/`
- `x64/` (ignored)
- `packages/` (ignored)

## Dependencies
Place GLFW and GLEW locally (outside version control):
- `packages/glfw.3.4.0/...`
- `packages/glew-2.2.0.2.2.0.1/...`

Ensure the project Include/Lib paths point to these directories, or install via vcpkg:
- `vcpkg install glfw3 glew`
