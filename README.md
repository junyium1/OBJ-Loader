# OBJ Loader

Single-header OBJ loader. Copy `objloader.h` and include it.

Please don't bother using main.cpp really, it's just to try things out :)

## Usage

```cpp
OBJLoader loader;
if (!loader.load("model.obj")) { /* failed to open */ }

// loader.out_vertices  std::vector<Vertex>
// loader.out_indices   std::vector<unsigned int>
```

## Vertex

```cpp
struct Vertex {
    float pos[3];
    float uv[2];
    float normal[3];
};
```

Supports all face formats (`v`, `v/vt`, `v//vn`, `v/vt/vn`). Polygons are triangulated. Duplicate vertices are deduplicated via hash map.
