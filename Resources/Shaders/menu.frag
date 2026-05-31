#version 330 core
out vec4 FragColor;

uniform vec4 uiColor;
uniform sampler2D uiTexture;
uniform bool useTexture;

in vec2 TexCoord;

void main()
{
    if (useTexture)
    {
        FragColor = texture(uiTexture, TexCoord);
        return;
    }

    FragColor = uiColor;
}
