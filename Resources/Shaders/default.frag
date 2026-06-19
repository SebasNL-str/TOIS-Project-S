#version 330 core

#define MAX_LIGHTS 16

// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Variables de entrada desde el Vertex Shader || Input variables from Vertex Shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Sampler para la textura || Sampler for the texture
uniform sampler2D texture1;

// Posicion de la camara || Camera position
uniform vec3 viewPos;

// Estructura de datos para las luces || Light data structure
struct Light
{
    int type;          // 0 = Point, 1 = Directional, 2 = Spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

// Arreglo y cantidad de luces activas || Array and count of active lights
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

// Limites del cono para la luz focal || Cone limits for spot light
uniform float spotCutOff;      
uniform float spotOuterCutOff; 

// Capa de niebla reutilizable || Reusable fog layer
struct FogLayer
{
    vec3 color;
    float nearDistance;
    float farDistance;
    float density;
    float baseHeight;
    float heightFalloff;
    float maxOpacity;
};

// Niebla de dos capas || Two-layer fog
struct Fog
{
    bool enabled;
    FogLayer localLayer;
    FogLayer boundaryLayer;
};

uniform Fog fog;

// Calcula una capa de niebla usando distancia y desvanecido vertical opcional || Calculate one fog layer using distance and optional height falloff
float CalculateFogAmount(FogLayer layer)
{
    float cameraDistance = length(viewPos - FragPos);
    float rangeSize = max(layer.farDistance - layer.nearDistance, 0.001);
    float distanceRange = clamp((cameraDistance - layer.nearDistance) / rangeSize, 0.0, 1.0);
    float distanceFog = smoothstep(0.0, 1.0, distanceRange);
    float densityFog = 1.0 - exp(-layer.density * cameraDistance);

    float verticalOffset = max(FragPos.y - layer.baseHeight, 0.0);
    float heightFog = exp(-verticalOffset * layer.heightFalloff);

    return clamp(distanceFog * densityFog * heightFog, 0.0, layer.maxOpacity);
}

// Mezcla la iluminacion con niebla local primero y luego con niebla lejana de encierro || Mix scene lighting with local fog first, then far enclosing fog
vec3 ApplyFog(vec3 color)
{
    if (!fog.enabled)
        return color;

    float localFogAmount = CalculateFogAmount(fog.localLayer);
    vec3 foggedColor = mix(color, fog.localLayer.color, localFogAmount);

    float boundaryFogAmount = CalculateFogAmount(fog.boundaryLayer);
    return mix(foggedColor, fog.boundaryLayer.color, boundaryFogAmount);
}

void main()
{
    // Muestrear el color de la textura || Sample the texture color
    vec4 texColor = texture(texture1, TexCoord);

    // Descartar pixeles transparentes || Discard transparent pixels
    if (texColor.a < 0.5)
        discard;

    // Normalizar y corregir orientacion de caras || Normalize and correct face orientation
    vec3 N = normalize(Normal);
    if (!gl_FrontFacing)
        N = -N;

    // Calcular vector de vision || Calculate view vector
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // Calcular iluminacion ambiental base || Calculate base ambient lighting
    vec3 ambient = 0.08 * texColor.rgb;
    result += ambient;

    // Ciclo para procesar cada luz || Loop to process each light
    for (int i = 0; i < numLights; i++)
    {
        vec3 L;
        float attenuation = 1.0;

        // Calcular atenuacion de luz puntual || Calculate point light attenuation
        if (lights[i].type == 0)
        {
            L = normalize(lights[i].position - FragPos);
            float dist = length(lights[i].position - FragPos);
            attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        }
        // Calcular direccion de luz direccional || Calculate directional light direction
        else if (lights[i].type == 1)
        {
            L = normalize(-lights[i].direction);
            attenuation = 1.0;
        }
        // Calcular atenuacion e intensidad del cono focal || Calculate spot light attenuation and intensity
        else if (lights[i].type == 2)
        {
            vec3 lightDir = normalize(lights[i].position - FragPos);
            L = lightDir;

            float theta = dot(lightDir, normalize(-lights[i].direction));
            float epsilon = max(spotCutOff - spotOuterCutOff, 0.0001);
            float intensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

            float dist = length(lights[i].position - FragPos);
            float distanceAttenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
            
            attenuation = intensity * distanceAttenuation;
        }

        // Calcular componente difusa || Calculate diffuse component
        float diff = max(dot(N, L), 0.0);

        // Calcular componente especular Blinn-Phong || Calculate Blinn-Phong specular component
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), 32.0);

        // Calcular color final de la luz combinando su color e intensidad HDR
        vec3 lightColor = lights[i].color * lights[i].intensity;

        // Factor de albedo físico (absorción de energía para materiales opacos normales)
        float materialAlbedo = 0.20; 

        // Aplicamos el albedo para balancear la reflectividad de la superficie
        vec3 diffuse = diff * lightColor * texColor.rgb * materialAlbedo;
        vec3 specular = spec * lightColor * 0.25 * materialAlbedo;

        // Acumular iluminacion atenuada
        result += (diffuse + specular) * attenuation;
    }
    
    // Aplicar las capas de niebla sobre el rango dinámico HDR calibrado
    result = ApplyFog(result);

    // Asignar color final reteniendo la información HDR para el pipeline de post-procesado
    FragColor = vec4(result, texColor.a);
}