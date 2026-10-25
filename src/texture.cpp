#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "leap_internal.h"
#include <iostream>

leap::Texture::Texture(const char *path) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (data) {
        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: \"" << path << "\"\n";
    }
    stbi_image_free(data);
}

leap::Texture::~Texture() {
    glDeleteTextures(1, &id);
}

unsigned int leap::Texture::GetID() const {
    return static_cast<unsigned int> (id);
}