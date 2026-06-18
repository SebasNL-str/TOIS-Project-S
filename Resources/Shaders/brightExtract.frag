#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTex;

void main()
{            
    vec3 color = texture(sceneTex, TexCoords).rgb;
    
    // Calcular luminancia según la percepción del ojo humano
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Umbral de Bloom (Ajustable)
    float threshold = 1.0; 
    
    if(brightness > threshold)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
