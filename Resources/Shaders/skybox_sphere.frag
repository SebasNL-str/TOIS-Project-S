#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas de textura esferica de entrada || Input spherical texture coordinates
in vec2 TexCoords;

// Sampler de la textura 2D esferica || Spherical 2D texture sampler
uniform sampler2D skyboxSphere;

void main()
{
    // Muestrear el mapa esferico usando las coordenadas UV || Sample the spherical map using UV coordinates
    FragColor = texture(skyboxSphere, TexCoords);
}
