#pragma once
#include <string>
#include <vector>

struct Vertex3D;

unsigned int LoadOBJ(const std::string& filePath, std::vector<Vertex3D>& vertexBuffer);