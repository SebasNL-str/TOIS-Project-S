#version 330 core

// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas espaciales interpoladas de entrada || Input interpolated spatial coordinates
in vec3 WorldPos;

// Variables uniformes para el control de la atmosfera || Uniform variables for atmospheric control
uniform vec3 fogColor;
uniform vec3 cameraPos;
uniform float fogRadius;
uniform float fogThickness;
uniform float fogOpacity;

void main()
{
    // Calcular la distancia euclidiana respecto a la camara || Calculate euclidean distance relative to the camera
    float d = length(WorldPos - cameraPos);
    
    // Calcular el factor de interpolacion suave de la niebla || Calculate smooth interpolation factor of the fog
    float fog = smoothstep(fogRadius - fogThickness, fogRadius, d);
    
    // Limitar la opacidad resultante entre cero y uno || Clamp the resulting opacity between zero and one
    float alpha = clamp(fog * fogOpacity, 0.0, 1.0);
    
    // Asignar el color final de la atmosfera con su transparencia || Assign the final atmospheric color with its transparency
    FragColor = vec4(fogColor, alpha);
}

