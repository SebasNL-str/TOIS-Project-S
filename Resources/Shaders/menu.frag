#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Variables uniformes de color e interfaz || Uniform color and interface variables
uniform vec4 uiColor;
uniform sampler2D uiTexture;
uniform bool useTexture;

// Coordenadas de entrada desde el Vertex Shader || Input coordinates from Vertex Shader
in vec2 TexCoord;

void main()
{
    // Renderizar textura si la bandera esta activa || Render texture if the flag is enabled
    if (useTexture)
    {
        FragColor = texture(uiTexture, TexCoord);
        return;
    }

    // Usar color plano en caso contrario || Use flat color otherwise
    FragColor = uiColor;
}
