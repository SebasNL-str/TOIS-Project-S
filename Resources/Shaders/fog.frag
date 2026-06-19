#version 330 core

out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 fogColor;
uniform vec3 cameraPos;
uniform float fogRadius;
uniform float fogThickness;
uniform float fogOpacity;

void main()
{
    float d = length(WorldPos - cameraPos);
    float fog = smoothstep(fogRadius - fogThickness, fogRadius, d);
    float alpha = clamp(fog * fogOpacity, 0.0, 1.0);
    FragColor = vec4(fogColor, alpha);
}
