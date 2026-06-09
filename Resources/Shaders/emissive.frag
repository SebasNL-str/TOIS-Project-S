#version 330 core

// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Variables de entrada desde el Vertex Shader || Input variables from Vertex Shader
in vec2 TexCoord;

// Propiedades materiales de iluminacion || Material lighting properties
uniform sampler2D texture1;
uniform vec3 emissiveColor;
uniform float intensity;

void main()
{
    // Muestrear los canales RGB de la textura || Sample the RGB channels of the texture
    vec3 tex = texture(texture1, TexCoord).rgb;

    // Calcular el color final con el factor de emision || Calculate final color with emission factor
    vec3 color = tex * emissiveColor * intensity;

    // Asignar color de salida con opacidad total || Assign output color with full opacity
    FragColor = vec4(color, 1.0);
}
