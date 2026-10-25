#include "leap_internal.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <array>
#include <map>

static void AppendFanDisk(leap::shapes::MeshData &data, float y, float radius, unsigned int sides, bool faceUp) {
    glm::vec3 normal = faceUp ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);

    GLuint centerIdx = static_cast<GLuint> (data.vertices.size() / 8);
    data.vertices.insert(data.vertices.end(), { 0.0f, y, 0.0f,  normal.x, normal.y, normal.z,  0.5f, 0.5f });
    for (unsigned int i = 0; i <= sides; ++i) {
        float theta = static_cast<float> (i) / sides * 2.0f * glm::pi<float>();
        float x = cos(theta) * radius;
        float z = sin(theta) * radius;
        data.vertices.insert(data.vertices.end(), { x, y, z,  normal.x, normal.y, normal.z,  static_cast<float> (0.5f + cos(theta) * 0.5f), static_cast<float> (0.5f + sin(theta) * 0.5f) });
    }
    
    for (unsigned int i = 0; i < sides; ++i) {
        GLuint a = centerIdx + 1 + i;
        GLuint b = centerIdx + 1 + i + 1;
        if (faceUp) data.indices.insert(data.indices.end(), { centerIdx, a, b });
        else        data.indices.insert(data.indices.end(), { centerIdx, b, a });
    }
}

leap::shapes::MeshData leap::shapes::LoadCube() {
    MeshData data;
    data.vertices = {
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    data.indices = {
        0,  1,  2,  2,  3,  0,
        4,  5,  6,  6,  7,  4,
        8,  9,  10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
    return data;
}

leap::shapes::MeshData leap::shapes::LoadPlane() {
    MeshData data;
    data.vertices = {
        -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };
    data.indices = { 0, 1, 2,  2, 3, 0 };
    return data;
}

leap::shapes::MeshData leap::shapes::LoadSphere(unsigned int stacks, unsigned int slices) {
    MeshData data;
    for (unsigned int i = 0; i <= stacks; ++i) {
        float v = static_cast<float> (i) / stacks;
        float phi = v * glm::pi<float>();
        for (unsigned int j = 0; j <= slices; ++j) {
            float u = static_cast<float> (j) / slices;
            float theta = u * 2.0f * glm::pi<float>();

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            data.vertices.insert(data.vertices.end(), { x * 0.5f, y * 0.5f, z * 0.5f,  x, y, z,  u, v });
        }
    }

    unsigned int stride = slices + 1;
    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            GLuint a = i * stride + j;
            GLuint b = a + stride;
            data.indices.insert(data.indices.end(), { a, b, a + 1,  a + 1, b, b + 1 });
        }
    }
    return data;
}

leap::shapes::MeshData leap::shapes::LoadPyramid(unsigned int sides) {
    if (sides < 3) sides = 3;
    MeshData data;

    glm::vec3 apex(0.0f, 0.5f, 0.0f);
    float baseY = -0.5f;

    for (unsigned int i = 0; i < sides; ++i) {
        float theta0 = (float)i / sides * 2.0f * glm::pi<float>();
        float theta1 = (float)(i + 1) / sides * 2.0f * glm::pi<float>();

        glm::vec3 p0(cos(theta0) * 0.5f, baseY, sin(theta0) * 0.5f);
        glm::vec3 p1(cos(theta1) * 0.5f, baseY, sin(theta1) * 0.5f);

        glm::vec3 normal = glm::normalize(glm::cross(p0 - apex, p1 - apex));

        GLuint base = static_cast<GLuint> (data.vertices.size() / 8);
        data.vertices.insert(data.vertices.end(), { apex.x, apex.y, apex.z,  normal.x, normal.y, normal.z,  0.5f, 1.0f });
        data.vertices.insert(data.vertices.end(), { p0.x, p0.y, p0.z,        normal.x, normal.y, normal.z,  0.0f, 0.0f });
        data.vertices.insert(data.vertices.end(), { p1.x, p1.y, p1.z,        normal.x, normal.y, normal.z,  1.0f, 0.0f });
        data.indices.insert(data.indices.end(), { base, base + 1, base + 2 });
    }

    AppendFanDisk(data, baseY, 0.5f, sides, false);
    return data;
}

leap::shapes::MeshData leap::shapes::LoadCylinder(unsigned int stacks, unsigned int slices) {
    if (slices < 3) { slices = 3; }
    if (stacks < 1) { stacks = 1; }
    MeshData data;
    float halfHeight = 0.5f;

    for (unsigned int i = 0; i <= stacks; ++i) {
        float v = static_cast<float> (i) / stacks;
        float y = -halfHeight + v * (halfHeight * 2.0f);
        for (unsigned int j = 0; j <= slices; ++j) {
            float u = static_cast<float> (j) / slices;
            float theta = u * 2.0f * glm::pi<float>();
            float nx = cos(theta);
            float nz = sin(theta);
            data.vertices.insert(data.vertices.end(), { nx * 0.5f, y, nz * 0.5f,  nx, 0.0f, nz,  u, v });
        }
    }

    unsigned int stride = slices + 1;
    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            GLuint a = i * stride + j;
            GLuint b = a + stride;
            data.indices.insert(data.indices.end(), { a, b, a + 1,  a + 1, b, b + 1 });
        }
    }

    AppendFanDisk(data, halfHeight, 0.5f, slices, true);
    AppendFanDisk(data, -halfHeight, 0.5f, slices, false);
    return data;
}

leap::shapes::MeshData leap::shapes::LoadQuad() {
    MeshData data;
    data.vertices = {
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
    };
    data.indices = { 0, 1, 2,  2, 3, 0 };
    return data;
}

leap::shapes::MeshData leap::shapes::LoadPolygon(unsigned int sides) {
    if (sides < 3) { sides = 3; }
    MeshData data;
    AppendFanDisk(data, 0.0f, 0.5f, sides, true);
    return data;
}

struct ObjVertex {
    int pos = -1;
    int uv = -1;
    int norm = -1;
};

static int ParseObjIndex(const std::string &s, size_t count) {
    if (s.empty()) { return -1; }

    int idx = 0;
    try { idx = std::stoi(s); } catch (...) { return -1; }

    if      (idx > 0) { idx -= 1; }
    else if (idx < 0) { idx = static_cast<int>(count) + idx; }
    else              { return -1; }

    return (idx >= 0 && idx < static_cast<int> (count)) ? idx : -1;
}

static ObjVertex ParseFaceToken(const std::string &token, size_t posCount, size_t uvCount, size_t normCount) {
    std::string parts[3];
    size_t start = 0;
    for (int i = 0; i < 3; ++i) {
        size_t slash = token.find('/', start);
        if (slash == std::string::npos) { parts[i] = token.substr(start); break; }
        parts[i] = token.substr(start, slash - start);
        start = slash + 1;
    }
    return { ParseObjIndex(parts[0], posCount), ParseObjIndex(parts[1], uvCount), ParseObjIndex(parts[2], normCount) };
}

leap::shapes::MeshData leap::shapes::LoadOBJ(const std::string &path) {
    MeshData data;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "ERROR: Failed to Open \"" << path << "\"\n";
        return data;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::map<std::array<int, 3>, GLuint> vertexCache;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        } else if (prefix == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (prefix == "f") {
            std::vector<ObjVertex> face;
            std::string token;
            while (ss >> token) {
                ObjVertex v = ParseFaceToken(token, positions.size(), uvs.size(), normals.size());
                if (v.pos < 0) { face.clear(); break; }
                face.push_back(v);
            }

            for (size_t i = 1; i + 1 < face.size(); ++i) {
                ObjVertex tri[3] = { face[0], face[i], face[i + 1] };

                glm::vec3 faceNormal(0.0f, 1.0f, 0.0f);
                if (tri[0].norm < 0 || tri[1].norm < 0 || tri[2].norm < 0) {
                    glm::vec3 c = glm::cross(positions[tri[1].pos] - positions[tri[0].pos],
                                             positions[tri[2].pos] - positions[tri[0].pos]);
                    if (glm::length(c) > 1e-8f) { faceNormal = glm::normalize(c); }
                }

                for (const ObjVertex &v : tri) {
                    bool shareable = v.norm >= 0;
                    std::array<int, 3> key = { v.pos, v.uv, v.norm };

                    if (shareable) {
                        auto it = vertexCache.find(key);
                        if (it != vertexCache.end()) { data.indices.push_back(it->second); continue; }
                    }

                    glm::vec3 p = positions[v.pos];
                    glm::vec3 n = shareable ? normals[v.norm] : faceNormal;
                    glm::vec2 t = (v.uv >= 0) ? uvs[v.uv] : glm::vec2(0.0f);

                    GLuint index = static_cast<GLuint>(data.vertices.size() / 8);
                    data.vertices.insert(data.vertices.end(), { p.x, p.y, p.z,  n.x, n.y, n.z,  t.x, t.y });
                    data.indices.push_back(index);
                    if (shareable) { vertexCache[key] = index; }
                }
            }
        }
    }

    return data;
}