#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    GLuint diffuseTex,
    GLuint roughnessTex,
    GLuint metallicTex)
{
    // Asignar los identificadores de las texturas de los materiales || Assign the material texture identifiers
    this->diffuseTex = diffuseTex;
    this->roughnessTex = roughnessTex;
    this->metallicTex = metallicTex;

    // Almacenar los indices de construccion geometrica || Store the geometric construction indices
    this->indices = indices;
    indexCount = static_cast<GLsizei>(indices.size());

    // Generar y enlazar los objetos de buffer de OpenGL || Generate and bind OpenGL buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Cargar el buffer de datos dinamicos de los vertices || Load vertices dynamic data buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    // Cargar el buffer de datos indexados de la geometria || Load geometry indexed data buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // Atributo 0: Configurar puntero para las posiciones X, Y, Z || Attribute 0: Configure pointer for X, Y, Z positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, x));

    // Atributo 1: Configurar puntero para los vectores normales || Attribute 1: Configure pointer for normal vectors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, nx));

    // Atributo 2: Configurar puntero para las coordenadas UV || Attribute 2: Configure pointer for UV coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, u));

    glBindVertexArray(0);
}

// Activar buffers y renderizar la malla || Activate buffers and render the mesh
void Mesh::Draw()
{
    // Realizar el enlace secuencial de los mapas de texturas || Perform sequential binding of texture maps
    if (diffuseTex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
    }
    if (roughnessTex) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roughnessTex);
    }
    if (metallicTex) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallicTex);
    }

    // Enlazar el arreglo de vertices y dibujar triangulos || Bind vertex array and draw triangles
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
