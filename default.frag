#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Cambiamos lightPos por lightDir
uniform vec3 lightDir;    // Ejemplo: vec3(-0.2f, -1.0f, -0.3f) -> Apunta hacia abajo y un poco de lado
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

uniform samplerCube skybox;       
uniform float reflectionFactor;   

void main() {
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1) discard;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // =========================================================
    // CÁLCULO DE LUZ DIRECCIONAL (Sin Atenuación)
    // =========================================================
    
    // 1. Ambiental (Le da claridad general al modelo para que nunca sea negro)
    float ambientStrength = 0.35; 
    vec3 ambient = ambientStrength * lightColor;

    // 2. Difusa (El vector de luz simplemente es la dirección invertida)
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. Especular (Brillo reflejado de la luna)
    float specularStrength = 0.20;
    vec3 reflectDir = reflect(-lightDirNorm, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0); 
    vec3 specular = specularStrength * spec * lightColor;  

    // Resultado de la iluminación global de la luna
    vec3 lightingColor = (ambient + diffuse + specular) * texColor.rgb;

    // --- LÓGICA DE REFLEXIÓN DEL CUBEMAP ---
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, norm);
    vec3 skyboxReflection = texture(skybox, R).rgb;

    vec3 finalColor = mix(lightingColor, skyboxReflection, reflectionFactor);

    FragColor = vec4(finalColor, 1.0);
}