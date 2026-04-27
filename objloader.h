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

    bool operator==(const Vertex& other) const {
        return pos[0] == other.pos[0] &&
            pos[1] == other.pos[1] &&
            pos[2] == other.pos[2] &&
            uv[0] == other.uv[0] &&
            uv[1] == other.uv[1] &&
            normal[0] == other.normal[0] &&
            normal[1] == other.normal[1] &&
            normal[2] == other.normal[2];
    }
};

struct VertexHasher
{
    size_t operator()(const Vertex& vertex) const {
        size_t hash = 0;
        const float* f = reinterpret_cast<const float*>(&vertex);
        for (int i = 0; i < 8; i++) {
            hash ^= std::hash<float>{}(f[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

class OBJLoader
{
public:
    std::vector<Vertex> out_vertices;
    std::vector<unsigned int> out_indices;

    bool load(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::vector<float> temp_pos;
        std::vector<float> temp_uv;
        std::vector<float> temp_normals;
        std::unordered_map<Vertex, unsigned int, VertexHasher> uniqueVertices;

        std::string line;

        while (std::getline(file, line))
        {
            if (line.rfind("v ", 0) == 0)
            {
                float x, y, z;
                sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);
                temp_pos.insert(temp_pos.end(), { x, y, z });
            }
            else if (line.rfind("vt ", 0) == 0)
            {
                float u, v;
                sscanf_s(line.c_str(), "vt %f %f", &u, &v);
                temp_uv.insert(temp_uv.end(), { u, v });
            }
            else if (line.rfind("vn ", 0) == 0)
            {
                float x, y, z;
                sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);
                temp_normals.insert(temp_normals.end(), { x, y, z });
            }
            else if (line.rfind("f ", 0) == 0)
            {
                process(line, temp_pos, temp_uv, temp_normals, uniqueVertices);
            }
        }

        return true;
    }

private:

    struct ObjIndex { int v = 0, vt = 0, vn = 0; };

    void process(
        const std::string& line,
        const std::vector<float>& pos,
        const std::vector<float>& uv,
        const std::vector<float>& norm,
        std::unordered_map<Vertex, unsigned int, VertexHasher>& uniqueVertices)
    {
        std::vector<ObjIndex> face;

        const char* ptr = line.c_str() + 2;
        int v, vt, vn, offset;

        while (*ptr)
        {
            vt = 0;
            vn = 0;

            if (sscanf_s(ptr, "%d/%d/%d%n", &v, &vt, &vn, &offset) == 3) {}
            else if (sscanf_s(ptr, "%d//%d%n", &v, &vn, &offset) == 2) {}
            else if (sscanf_s(ptr, "%d/%d%n", &v, &vt, &offset) == 2) {}
            else if (sscanf_s(ptr, "%d%n", &v, &offset) == 1) {}
            else break;

            face.push_back({ v, vt, vn });
            ptr += offset;
        }

        if (face.size() < 3) return;

        for (size_t i = 1; i + 1 < face.size(); i++)
        {
            int idxs[3] = { 0, (int)i, (int)i + 1 };

            for (int j = 0; j < 3; j++)
            {
                ObjIndex& id = face[idxs[j]];
                Vertex vertex{};

                if (id.v > 0)
                {
                    int p = (id.v - 1) * 3;
                    vertex.pos[0] = pos[p];
                    vertex.pos[1] = pos[p + 1];
                    vertex.pos[2] = pos[p + 2];
                }

                if (id.vt > 0)
                {
                    int t = (id.vt - 1) * 2;
                    if (t + 1 < (int)uv.size())
                    {
                        vertex.uv[0] = uv[t];
                        vertex.uv[1] = 1.0f - uv[t + 1]; // FIX OpenGL
                    }
                }

                if (id.vn > 0)
                {
                    int n = (id.vn - 1) * 3;
                    if (n + 2 < (int)norm.size())
                    {
                        vertex.normal[0] = norm[n];
                        vertex.normal[1] = norm[n + 1];
                        vertex.normal[2] = norm[n + 2];
                    }
                }

                if (uniqueVertices.find(vertex) == uniqueVertices.end())
                {
                    uniqueVertices[vertex] = (unsigned int)out_vertices.size();
                    out_vertices.push_back(vertex);
                }

                out_indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
};