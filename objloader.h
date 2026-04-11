#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstring> 

struct Vertex 
{
    float pos[3];
    float uv[2];
    float normal[3];

    bool operator == (const Vertex& other) const {
        return memcmp(this, &other, sizeof(Vertex)) == 0;
    }
};

struct VertexHasher 
{
    size_t operator()(const Vertex& vertex) const {
        size_t h = 0;
        const float* f = reinterpret_cast<const float*>(&vertex);
        for (int i = 0; i < 8; i++) {
            h ^= std::hash<float>{}(f[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

class OBJLoader 
{
public:
    std::vector<Vertex> out_vertices;
    std::vector<unsigned int> out_indices;

    inline bool load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::vector<float> temp_pos;
        std::vector<float> temp_uv;
        std::vector<float> temp_normals;
        std::unordered_map<Vertex, unsigned int, VertexHasher> uniqueVertices;

        std::string line;
        while (std::getline(file, line)) {
            if (line.compare(0, 2, "v ") == 0) {
                float x, y, z;
                sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);
                temp_pos.insert(temp_pos.end(), {x, y, z});
            }
            else if (line.compare(0, 3, "vt ") == 0) {
                float u, v;
                sscanf_s(line.c_str(), "vt %f %f", &u, &v);
                temp_uv.insert(temp_uv.end(), {u, v});
            }
            else if (line.compare(0, 3, "vn ") == 0) {
                float x, y, z;
                
                sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);
                temp_normals.insert(temp_normals.end(), {x, y, z});
            }
            else if (line.compare(0, 2, "f ") == 0) {
                
                process(line, temp_pos, temp_uv, temp_normals, uniqueVertices);
            }
        }
        return true;
    }

private: 
    inline void process(const std::string& line,
        const std::vector<float>& pos,
        const std::vector<float>& uv,
        const std::vector<float>& norm,
        std::unordered_map<Vertex, unsigned int, VertexHasher>& uniqueVertices)
    {
        struct ObjIndex { int v, vt, vn; };
        std::vector<ObjIndex> faceIndices;
        const char* ptr = line.c_str() + 2;
        int v, vt, vn, offset;

        while (true) {
            if (sscanf_s(ptr, "%d/%d/%d%n", &v, &vt, &vn, &offset) == 3) {}
            else if (sscanf_s(ptr, "%d//%d%n", &v, &vn, &offset) == 2) { vt = 0; }
            else if (sscanf_s(ptr, "%d/%d%n", &v, &vt, &offset) == 2) { vn = 0; }
            else if (sscanf_s(ptr, "%d%n", &v, &offset) == 1) { vt = 0; vn = 0; }
            else { break; }

            faceIndices.push_back({ v, vt, vn });
            ptr += offset;
        }

        if (faceIndices.size() < 3) return;

        for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
            int corners[3] = { 0, (int)i, (int)i + 1 };
            for (int j = 0; j < 3; j++) {
                int idx = corners[j];
                Vertex vertex{};

                int vIdx = faceIndices[idx].v;
                if (vIdx > 0 && (size_t)(vIdx - 1) * 3 + 2 < pos.size()) {
                    vertex.pos[0] = pos[(vIdx - 1) * 3];
                    vertex.pos[1] = pos[(vIdx - 1) * 3 + 1];
                    vertex.pos[2] = pos[(vIdx - 1) * 3 + 2];
                }

                int vtIdx = faceIndices[idx].vt;
                if (vtIdx > 0 && (size_t)(vtIdx - 1) * 2 + 1 < uv.size()) {
                    vertex.uv[0] = uv[(vtIdx - 1) * 2];
                    vertex.uv[1] = uv[(vtIdx - 1) * 2 + 1];
                }

                int vnIdx = faceIndices[idx].vn;
                if (vnIdx > 0 && (size_t)(vnIdx - 1) * 3 + 2 < norm.size()) {
                    vertex.normal[0] = norm[(vnIdx - 1) * 3];
                    vertex.normal[1] = norm[(vnIdx - 1) * 3 + 1];
                    vertex.normal[2] = norm[(vnIdx - 1) * 3 + 2];
                }

                if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
                    uniqueVertices[vertex] = static_cast<unsigned int>(out_vertices.size());
                    out_vertices.push_back(vertex);
                }
                out_indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
};