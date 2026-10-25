#include "leap_internal.h"
#include <iostream>

static const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aUV;\n"
    "\n"
    "out vec3 fragNormal;\n"
    "out vec3 fragPos;\n"
    "out vec2 fragUV;\n"
    "\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform mat3 normalMatrix;\n"
    "\n"
    "void main() {\n"
    "    fragPos = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));\n"
    "    fragNormal = normalize(normalMatrix * aNormal);\n"
    "    fragUV = aUV;\n"
    "    gl_Position = projection * view * vec4(fragPos.x, fragPos.y, fragPos.z, 1.0);\n"
    "}\0";
static const GLchar *fragmentShaderSource =
    "#version 330 core\n"
    "#define MAX_LIGHTS 8\n"
    "\n"
    "in vec3 fragNormal;\n"
    "in vec3 fragPos;\n"
    "in vec2 fragUV;\n"
    "\n"
    "out vec4 outColor;\n"
    "\n"
    "uniform vec4 color;\n"
    "uniform bool useTexture;\n"
    "uniform bool unlit;\n"
    "uniform sampler2D texture0;\n"
    "uniform float ambientStrength;\n"
    "\n"
    "struct Light {\n"
    "    int type;\n"
    "    vec3 direction;\n"
    "    vec3 position;\n"
    "    vec3 color;\n"
    "    float intensity;\n"
    "    float range;\n"
    "};\n"
    "\n"
    "uniform Light lights[MAX_LIGHTS];\n"
    "uniform int numLights;\n"
    "\n"
    "void main() {\n"
    "    vec4 baseColor = useTexture ? texture(texture0, fragUV) * color : color;\n"
    "\n"
    "    if (unlit) {\n"
    "        outColor = baseColor;\n"
    "        return;\n"
    "    }\n"
    "\n"
    "    vec3 normal = normalize(fragNormal);\n"
    "    vec3 result = ambientStrength * baseColor.rgb;\n"
    "\n"
    "    for (int i = 0; i < numLights; ++i) {\n"
    "        vec3 lightDir;\n"
    "        float attenuation = 1.0;\n"
    "\n"
    "        if (lights[i].type == 0) {\n"
    "            lightDir = -normalize(lights[i].direction);\n"
    "        } else {\n"
    "            vec3 toLight = lights[i].position - fragPos;\n"
    "            float dist = length(toLight);\n"
    "            lightDir = toLight / dist;\n"
    "            attenuation = clamp(1.0 - dist / lights[i].range, 0.0, 1.0);\n"
    "        }\n"
    "\n"
    "        float diff = max(dot(normal, lightDir), 0.0);\n"
    "        result += diff * lights[i].color * lights[i].intensity * attenuation * baseColor.rgb;\n"
    "    }\n"
    "\n"
    "    outColor = vec4(result, baseColor.a);\n"
    "}\0";

leap::Shader::Shader() {
    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        throw Error(std::string("Vertex Shader Failed to Compile\n") + infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw Error(std::string("Fragment Shader Failed to Compile\n") + infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        throw Error(std::string("Shader Program Failed to Link\n") + infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

leap::Shader::~Shader() {
    glDeleteProgram(shaderProgram);
}

void leap::Shader::Render() {
    glUseProgram(shaderProgram);
}

unsigned int leap::Shader::GetShaderProgram() const {
    return static_cast<unsigned int> (shaderProgram);
}

int leap::Shader::GetUniformLocation(const std::string &name) {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        return static_cast<int> (it->second);
    }

    GLint location = glGetUniformLocation(shaderProgram, name.c_str());
    if (location == -1) {
        std::cout << "WARNING: uniform\"" << name << "\" not found (typo or unused/optimized out by the shader compiler)\n";
    }

    uniformCache[name] = location;
    return static_cast<int> (location);
}