# OBJ Loader

Single-header OBJ loader. Copy `objloader.h` and include it.

Please don't bother using main.cpp really, it's just to try things out :)

## Usage
### First step

Load your .obj file (for now) like in the code above to initiate the load.

```cpp
OBJLoader loader;
if (!loader.load("model.obj")) { /* failed to open */ }
```

### Second step

Write your buffers like you would normally do on your favourite graphics library, for exemple in OpenGL : 

```cpp
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, loader.out_vertices.size() * sizeof(Vertex), loader.out_vertices.data(), GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, loader.out_indices.size() * sizeof(unsigned int), loader.out_indices.data(), GL_STATIC_DRAW);
```

## Small guide

`loader.out_vertices` : A vector of `Vertex`, each vertex contains its position, its UVs and its normals.
`loader.out_indices` : A vector of integers (`unsigned int`). They're the indices that point to the top vertices.
`.size()` returns the number of elements stored in the vector.
`.data()` returns a pointer to the first to the very first element of the array in the RAM.


## Vertex

```cpp
struct Vertex {
    float pos[3];
    float uv[2];
    float normal[3];
};
```

Supports all face formats (`v`, `v/vt`, `v//vn`, `v/vt/vn`). Polygons are triangulated. Duplicate vertices are deduplicated via hash map.
