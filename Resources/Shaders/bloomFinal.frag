#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTex;
uniform sampler2D blurTex;
uniform bool bloomEnabled; // <-- NUEVO UNIFORM
uniform float exposure = 1.0;

void main() {             
    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;      
    vec3 bloomColor = texture(blurTex, TexCoords).rgb;
    
    // Si está habilitado, sumamos el blur. Si no, dejamos solo la escena base.
    vec3 result = hdrColor;
    if (bloomEnabled) {
        result += bloomColor; // Mezcla aditiva
    }
    
    // Tone mapping (Reinhard) y corrección Gamma para que la pantalla no dé negro
    vec3 mapped = vec3(1.0) - exp(-result * exposure);
    mapped = pow(mapped, vec3(1.0 / 2.2));
  
    FragColor = vec4(mapped, 1.0);
}