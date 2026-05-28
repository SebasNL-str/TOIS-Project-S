#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>

// Debes descargar stb_image.h y ponerlo en tu carpeta de includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

class Texture {
public:
    unsigned int ID;
    std::string type;
    std::string path;

    Texture(const char* imagePath, const std::string& directory) {
        std::string filename = std::string(imagePath);
        filename = directory + '/' + filename;

        glGenTextures(1, &ID);

        int width, height, nrComponents;
        // Invertir texturas en el eje Y (Assimp y OpenGL tienen ejes opuestos)
      //  stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            GLenum format = GL_RGB; // Valor por defecto para evitar basura
            bool validFormat = true;

            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else {
                // Caso para 2 canales u otros formatos no estándar
                std::cout << "Advertencia: Formato de " << nrComponents << " canales no manejado. Saltando textura." << std::endl;
                validFormat = false;
            }

            if (validFormat) {
                glBindTexture(GL_TEXTURE_2D, ID);
                // Usamos el formato detectado tanto para el formato interno como para el de los datos
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }

            stbi_image_free(data);
        }
        else {
            std::cout << "Error al cargar textura en la ruta: " << filename << std::endl;
            stbi_image_free(data);
        }
    }

    void Bind(unsigned int unit = 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};