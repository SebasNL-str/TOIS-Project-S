#version 330 core

// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Variable uniforme para el color plano || Uniform variable for flat color
uniform vec3 color;

void main()
{
    // Asignar el color de salida con opacidad total || Assign output color with full opacity
    FragColor = vec4(color, 1.0);
}
