# gfx
A game development framework that aims to be cross-platform. The objective isn't necessarily to be very efficient but rather to be easy to use. All dependencies are included and by default are statically linked:

- [Assimp](https://github.com/assimp/assimp)
- [FreeType](https://github.com/freetype/freetype)
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [miniaudioex](https://github.com/japajoe/miniaudioex)
- [Dear ImGui](https://github.com/ocornut/imgui)

# Building
`git clone https://github.com/japajoe/gfx`

`cd gfx`

`git submodule update --init --recursive`

`mkdir build`

`cd build`

`cmake ..`

`cmake --build .`