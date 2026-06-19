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
    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;
    vec3 bloomColor = texture(blurTex, TexCoords).rgb;

    if (bloomEnabled)
    {
        hdrColor += bloomColor;
    }

    float exposure = 1.0;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
