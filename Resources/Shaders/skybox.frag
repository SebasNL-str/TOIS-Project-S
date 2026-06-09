#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas de textura cubica de entrada || Input cube map texture coordinates
in vec3 TexCoords;

// Sampler del mapa de cubos || Cube map sampler
uniform samplerCube skybox;

void main()
{
    // Muestrear el mapa de cubos usando las coordenadas 3D || Sample the cube map using 3D coordinates
    FragColor = texture(skybox, TexCoords);
}

