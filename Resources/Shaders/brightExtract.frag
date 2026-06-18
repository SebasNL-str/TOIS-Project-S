#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTex;

void main() {
    vec3 color = texture(sceneTex, TexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Parámetros para el umbral suave
    float threshold = 1.0;
    float knee = 0.1; // Qué tan suave es la transición (ajusta entre 0.05 y 0.2)
    
    // Fórmula matemática para Soft-Knee
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = (soft * soft) / (4.0 * knee + 0.00001);
    
    float contribution = max(soft, brightness - threshold);
    contribution /= max(brightness, 0.00001);
    
    FragColor = vec4(color * contribution, 1.0);
}
