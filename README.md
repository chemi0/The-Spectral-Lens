# The Spectral Lens (about)

2D OpenGL 3.3+ project. 
Graphical (kind-of, not really pixel-art) interactive set containing a "fake" landscape, and a hidden one, which you can explore using "Spectral Lens" and discover different moving entities inside of it.
Framerate is locked to 75FPS.
Landscapes are hand-crafted (by me) in photoshop using free assets from different websites:
https://assetstore.unity.com
https://opengameart.org
https://craftpix.net

## Dependencies
Place GLFW and GLEW locally (outside version control):
- `packages/glfw.3.4.0/...`
- `packages/glew-2.2.0.2.2.0.1/...`

Ensure the project Include/Lib paths point to these directories, or install via vcpkg:
- `vcpkg install glfw3 glew`

Large vendor binaries in `packages/` are intentionally excluded to keep the repo small.

