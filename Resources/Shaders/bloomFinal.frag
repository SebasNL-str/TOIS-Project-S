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
    // Muestrear el mapa de color base y el mapa de brillo difuminado || Sample the base color map and the blurred bloom map
    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;
    vec3 bloomColor = texture(blurTex, TexCoords).rgb;

    // Sumar aritmeticamente el brillo si el efecto esta activo || Arithmetically add bloom if the effect is enabled
    if (bloomEnabled)
    {
        hdrColor += bloomColor;
    }

    // Aplicar mapeo de tonos por exposicion exponencial || Apply exposure tone mapping exponentially
    float exposure = 1.0;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    // Realizar la correccion de gamma final || Perform final gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    // Asignar el color mapeado definitivo a la pantalla || Assign the definitive mapped color to the screen
    FragColor = vec4(result, 1.0);
}

