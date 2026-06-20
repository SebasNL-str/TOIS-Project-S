#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas de textura interpoladas de entrada || Input interpolated texture coordinates
in vec2 TexCoords;

// Sampler de la textura de entrada || Input texture sampler
uniform sampler2D image;

// Bandera para alternar la direccion del filtrado || Flag to toggle filtering direction
uniform bool horizontal;

// Pesos del filtro Gaussiano para el desenfoque || Gaussian filter weights for blurring
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    // Calcular el tamaño inverso de la textura || Calculate inverse texture size
    vec2 tex_offset = 1.0 / textureSize(image, 0); 
    
    // Inicializar el acumulador con el pixel central || Initialize accumulator with the central pixel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; 
    
    // Aplicar convolucion segun la orientacion lineal || Apply convolution based on linear orientation
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            // Acumular muestras en el eje horizontal derecho e izquierdo || Accumulate samples on the right and left horizontal axis
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            // Acumular muestras en el eje vertical superior e inferior || Accumulate samples on the upper and lower vertical axis
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    
    // Asignar el color difuminado final || Assign the final blurred color
    FragColor = vec4(result, 1.0);
}
