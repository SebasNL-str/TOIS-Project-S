#version 330 core
// Atributos de entrada del vertice || Vertex input attributes
layout (location = 0) in vec2 aPos;

// Variable de salida hacia el fragment shader || Output variable to fragment shader
out vec2 vPos; 

void main() {
    // Pasar coordenadas de posicion local || Pass local position coordinates
    vPos = aPos; 
    
    // Calcular la posicion final proyectada en pantalla || Calculate final projected screen position
    gl_Position = vec4(aPos, 0.0, 1.0); 
}


