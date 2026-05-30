#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 emissiveColor;
uniform float intensity;

void main()
{
    vec3 tex = texture(texture1, TexCoord).rgb;

    vec3 color = tex * emissiveColor * intensity;

    FragColor = vec4(color, 1.0);
}