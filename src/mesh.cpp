#include "leap_internal.h"
#include <algorithm>

leap::Mesh::Mesh(leap::shapes::MeshData mesh) {
    vertices = mesh.vertices;
    indices = mesh.indices;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void leap::Mesh::Draw() {
    Material *material = owner->GetComponent<Material>();
    glm::vec4 color = material ? material->color : glm::vec4(1.0f);
    glUniform4fv(mainShader->GetUniformLocation("color"), 1, glm::value_ptr(color));

    bool hasTexture = material && material->texture;
    glUniform1i(mainShader->GetUniformLocation("useTexture"), hasTexture);

    bool isUnlit = material && material->unlit;
    glUniform1i(mainShader->GetUniformLocation("unlit"), isUnlit);

    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material->texture->GetID());
        glUniform1i(mainShader->GetUniformLocation("texture0"), 0);
    }

    Transform *transform = owner->GetComponent<Transform>();
    glm::mat4 model = transform ? transform->GetModelMatrix() : glm::mat4(1.0f);
    glm::mat4 view  = mainCam->GetViewMatrix();
    glm::mat4 proj  = mainCam->GetProjectionMatrix();
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(mainShader->GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(mainShader->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(mainShader->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix3fv(mainShader->GetUniformLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform1f(mainShader->GetUniformLocation("ambientStrength"), 0.15f);

    constexpr int MAX_LIGHTS = 8;
    int count = std::min(static_cast<int>(allLights.size()), MAX_LIGHTS);
    glUniform1i(mainShader->GetUniformLocation("numLights"), count);

    for (int i = 0; i < count; ++i) {
        Light *light = allLights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        int typeInt = (light->type == LightType::Directional) ? 0 : 1;
        glUniform1i(mainShader->GetUniformLocation(prefix + "type"), typeInt);
        glUniform3fv(mainShader->GetUniformLocation(prefix + "color"), 1, glm::value_ptr(light->color));
        glUniform1f(mainShader->GetUniformLocation(prefix + "intensity"), light->intensity);

        if (light->type == LightType::Directional) {
            glm::vec3 dir = glm::normalize(light->direction);
            glUniform3fv(mainShader->GetUniformLocation(prefix + "direction"), 1, glm::value_ptr(dir));
        } else {
            Transform *lightTransform = light->owner->GetComponent<Transform>();
            glm::vec3 pos = lightTransform ? lightTransform->position : glm::vec3(0.0f);
            glUniform3fv(mainShader->GetUniformLocation(prefix + "position"), 1, glm::value_ptr(pos));
            glUniform1f(mainShader->GetUniformLocation(prefix + "range"), light->range);
        }
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void leap::Mesh::Cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}