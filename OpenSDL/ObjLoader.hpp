#pragma once
#include "ObjMesh.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ObjLoader {
public:
    static bool load(const std::string& filename, ObjMesh& mesh) {
        mesh.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            printf("Failed to open OBJ file: %s\n", filename.c_str());
            return false;
        }

        std::vector<vec3> tempVertices;
        std::vector<vec3> tempNormals;
        std::vector<vec2> tempUVs;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                tempVertices.push_back(vec3(x, y, z));
            }
            else if (prefix == "vt") {
                float u, v;
                iss >> u >> v;
                tempUVs.push_back(vec2(u, v));
                mesh.hasUVs = true;
            }
            else if (prefix == "vn") {
                float x, y, z;
                iss >> x >> y >> z;
                tempNormals.push_back(vec3(x, y, z));
                mesh.hasNormals = true;
            }
            else if (prefix == "f") {
                std::string v1, v2, v3;
                iss >> v1 >> v2 >> v3;

                auto parseVertex = [](const std::string& str) -> std::tuple<int, int, int> {
                    int v = 0, vt = 0, vn = 0;
                    char slash;
                    std::istringstream sstream(str);
                    sstream >> v;
                    if (sstream.peek() == '/') {
                        sstream >> slash;
                        if (sstream.peek() != '/') {
                            sstream >> vt;
                        }
                        if (sstream.peek() == '/') {
                            sstream >> slash;
                            sstream >> vn;
                        }
                    }
                    return { v - 1, vt - 1, vn - 1 };
                    };

                auto [v1_idx, vt1_idx, vn1_idx] = parseVertex(v1);
                auto [v2_idx, vt2_idx, vn2_idx] = parseVertex(v2);
                auto [v3_idx, vt3_idx, vn3_idx] = parseVertex(v3);

                mesh.vertices.push_back(tempVertices[v1_idx]);
                mesh.vertices.push_back(tempVertices[v2_idx]);
                mesh.vertices.push_back(tempVertices[v3_idx]);

                if (mesh.hasUVs) {
                    mesh.uvs.push_back(tempUVs[vt1_idx]);
                    mesh.uvs.push_back(tempUVs[vt2_idx]);
                    mesh.uvs.push_back(tempUVs[vt3_idx]);
                }

                if (mesh.hasNormals) {
                    mesh.normals.push_back(tempNormals[vn1_idx]);
                    mesh.normals.push_back(tempNormals[vn2_idx]);
                    mesh.normals.push_back(tempNormals[vn3_idx]);
                }

                mesh.indices.push_back(mesh.vertices.size() - 3);
                mesh.indices.push_back(mesh.vertices.size() - 2);
                mesh.indices.push_back(mesh.vertices.size() - 1);
            }
        }

        file.close();
        printf("Loaded OBJ: %zu vertices, %zu triangles\n",
            mesh.vertices.size(), mesh.indices.size() / 3);
        return true;
    }
};