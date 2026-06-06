#version 330 core
out vec4 FragColor;
uniform vec3 barColor;
void main() {
    FragColor = vec4(barColor, 1.0);
}
