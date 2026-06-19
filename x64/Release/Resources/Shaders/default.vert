#version 330 core

// Atributos de entrada del vertice || Vertex input attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// Variables de salida hacia el Fragment Shader || Output variables to Fragment Shader
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

// Matrices de transformacion || Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Bandera para ignorar la traslacion de la vista || Flag to ignore view translation
uniform bool useViewNoTranslation; 

void main()
{
    // Calcular posicion en el espacio de mundo || Calculate position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transformar normales al espacio de mundo || Transform normals to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pasar coordenadas de textura || Pass texture coordinates
    TexCoord = aTexCoord;

    // Calcular la posicion final del vertice || Calculate final vertex position
    if (useViewNoTranslation) {
        // Eliminar traslacion para renderizado tipo skybox || Remove translation for skybox rendering
        mat4 viewNoTranslation = mat4(mat3(view));
        gl_Position = projection * viewNoTranslation * model * vec4(aPos, 1.0);
    } else {
        // Proyeccion de escena estandar || Standard scene projection
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}


