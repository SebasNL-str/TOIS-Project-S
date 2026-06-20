#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas de textura interpoladas de entrada || Input interpolated texture coordinates
in vec2 TexCoords;

// Sampler de la textura de la escena || Scene texture sampler
uniform sampler2D sceneTex;

void main()
{           
    // Muestrear el mapa de bits de la escena original || Sample the original scene bitmap
    vec3 color = texture(sceneTex, TexCoords).rgb;
    
    // Calcular factor de luminancia perceptual || Calculate perceptual luminance factor
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Definir parametros de umbral y transicion suave || Define threshold and smooth transition parameters
    float threshold = 0.6; 
    float knee = 0.4;      

    // Aplicar ecuacion matemática de curva suave || Apply soft knee curve mathematical equation
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = (soft * soft) / (4.0 * knee + 0.0001);
    
    // Evaluar la contribución final de brillo || Evaluate final brightness contribution
    float contribution = max(soft, brightness - threshold);
    
    // Aplicar factor de multiplicacion de ganancia energetica || Apply energy gain multiplication factor
    contribution = (contribution * 2.5) / max(brightness, 0.0001);

    // Asignar el color filtrado y escalado resultante || Assign the resulting filtered and scaled color
    FragColor = vec4(color * contribution, 1.0);
}
