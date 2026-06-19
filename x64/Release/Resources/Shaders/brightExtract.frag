#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTex;

void main()
{           
    // Muestrear el color de la escena HDR original
    vec3 color = texture(sceneTex, TexCoords).rgb;
    
    // 1. Calcular luminancia según la percepción del ojo humano (CIE 1931)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // 2. Parámetros calibrados del Umbral Suave (Soft Knee)
    float threshold = 0.6; // Bajamos el umbral para rescatar la luz de los emisores
    float knee = 0.4;      // Mantenemos una curva de transición sedosa y orgánica

    // 3. Ecuación matemática de Soft Knee para evitar bordes duros
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = (soft * soft) / (4.0 * knee + 0.0001);
    
    // Combinar la curva suave con la extracción estándar
    float contribution = max(soft, brightness - threshold);
    
    // 4. BOOST DE ENERGÍA (Ganancia)
    // Multiplicamos por 2.5 para que los objetos emisores vuelvan a brillar con fuerza.
    // Al dividir por el brillo, normalizamos la aportación de color original.
    contribution = (contribution * 2.5) / max(brightness, 0.0001);

    // Guardar el color filtrado y potenciado listo para el shader de desenfoque (Blur)
    FragColor = vec4(color * contribution, 1.0);
}
