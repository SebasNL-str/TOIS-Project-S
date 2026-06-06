#version 330 core
out vec4 FragColor;

in vec2 vPos; 

uniform vec3 barColor;
uniform float globalTime; 
uniform bool isSpinner;

#define PI 3.14159265359

void main() {
    if (isSpinner) {
        // Corregimos la posición restando el desfase del centro (0.0, -0.6) 
        // para que la GPU calcule el ángulo perfectamente desde el eje real
        vec2 localPos = vec2(vPos.x - 0.0, vPos.y - (-0.6));
        
        // 1. Calcular el ángulo del círculo actual (-PI a PI)
        float angle = atan(localPos.y, localPos.x);
        
        // 2. Normalizar el ángulo para que vaya de 0.0 a 1.0 en sentido horario
        float normalizedAngle = (angle + PI) / (2.0 * PI);
        
        // 3. Crear una onda de tiempo continua (¡Corregido a 'fract'!)
        float speed = 1.5; 
        float wave = fract(globalTime * speed); 
        
        // 4. Calcular la distancia entre la ola de luz y el ángulo del círculo
        float diff = abs(wave - normalizedAngle);
        if (diff > 0.5) diff = 1.0 - diff; 
        
        // 5. Definir la intensidad de la ola de luz
        float intensity = smoothstep(0.2, 0.0, diff);
        float brightness = mix(0.1, 1.0, intensity);
        
        FragColor = vec4(barColor * brightness, 1.0);
    } else {
        // La barra de progreso recuperará su color verde/gris normal
        FragColor = vec4(barColor, 1.0);
    }
}
