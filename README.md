# OBJ Loader & Viewer

A lightweight OBJ file viewer built with OpenGL 3.3, featuring a single-header parser, real-time camera controls, and multiple rendering modes switchable at runtime via ImGui.

## Features

- **OBJ parsing** — positions, UVs, normals, all face formats (`v`, `v/vt`, `v//vn`, `v/vt/vn`), polygon triangulation, vertex deduplication via hash map
- **MTL support** — reads diffuse color from `.mtl` files
- **3 rendering modes** (switchable live):
  - **Normals** — visualizes surface normals as colors
  - **Texture** — applies a `color.png` texture via UV mapping
  - **MTL** — Lambert diffuse lighting using the MTL diffuse color, with a controllable light direction
- **ImGui panel** — camera position, FOV, light direction slider (MTL mode)
- **Free camera** — WASD movement, right-click drag to rotate, scroll to zoom

## Dependencies

- [GLFW](https://www.glfw.org/)
- [GLAD](https://glad.dav1d.de/)
- [GLM](https://github.com/g-truc/glm)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [stb_image](https://github.com/nothings/stb)

## Getting started

1. Clone the repo and open `OBJ Loader.slnx` in Visual Studio
2. Place your `.obj` (and optionally its `.mtl`) in the project root
3. In `main.cpp`, set the filename on this line:
```cpp
if (!loader.load("yourmodel.obj")) return -1;
```
4. Build and run

## Using the OBJLoader header

`objloader.h` is a single-header parser you can drop into any project.

```cpp
#include "objloader.h"

OBJLoader loader;
if (!loader.load("model.obj")) { /* failed to open */ }
```

Then feed the output directly into your GPU buffers:

```cpp
glBufferData(GL_ARRAY_BUFFER, loader.out_vertices.size() * sizeof(Vertex), loader.out_vertices.data(), GL_STATIC_DRAW);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, loader.out_indices.size() * sizeof(unsigned int), loader.out_indices.data(), GL_STATIC_DRAW);
```

### Output data

| Field | Type | Description |
|---|---|---|
| `loader.out_vertices` | `vector<Vertex>` | Deduplicated vertices (pos, UV, normal) |
| `loader.out_indices` | `vector<unsigned int>` | Indices for indexed drawing |
| `loader.diffuseColor` | `glm::vec3` | Diffuse color parsed from `.mtl` |

### Vertex layout

```cpp
struct Vertex {
    float pos[3];    // location 0
    float uv[2];     // location 1
    float normal[3]; // location 2
};
```
