#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculamos la posición del fragmento en el espacio del mundo real
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Matriz normal inversa-traspuesta para corregir la iluminación si escalas las criptas o tumbas
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    TexCoords = aTexCoords;
    
    // Proyección final del vértice a coordenadas homogéneas de clip
    gl_Position = projection * view * vec4(FragPos, 1.0);
}