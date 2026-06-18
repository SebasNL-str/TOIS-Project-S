#version 330 core

// Color de salida final hacia la pantalla || Final output color to screen
out vec4 FragColor;

// Coordenadas de textura desde el Vertex Shader || Texture coordinates from Vertex Shader
in vec2 TexCoords;

// Samplers para la escena HDR y el brillo difuminado || Samplers for HDR scene and blurred bloom
uniform sampler2D sceneTex;
uniform sampler2D blurTex;

// Bandera de control para activar/desactivar el efecto || Control flag to toggle the effect
uniform bool bloomEnabled;

void main()
{             
    // Muestrear la escena original (nitida) y el mapa de desenfoque (Bloom)
    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;      
    vec3 bloomColor = texture(blurTex, TexCoords).rgb;
    
    // Si el Bloom esta habilitado mediante la tecla B, realizamos la mezcla aditiva
    if(bloomEnabled)
    {
        hdrColor += bloomColor; 
    }
    
    // =========================================================================
    // MAPEO DE TONOS (TONEMAPPING) POR EXPOSICIÓN
    // =========================================================================
    // Comprime los valores flotantes masivos (> 1.0) al rango visible [0.0, 1.0].
    // Puedes ajustar el valor de 'exposure' (ej. 0.8 o 1.2) para alterar el brillo general.
    float exposure = 1.0; 
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    // =========================================================================
    // CORRECCIÓN GAMMA (Estándar sRGB 2.2)
    // =========================================================================
    // Evita que los colores se vean lavados, oscuros o sobresaturados en el monitor.
    result = pow(result, vec3(1.0 / 2.2));
  
    // Salida final con opacidad completa
    FragColor = vec4(result, 1.0);
}