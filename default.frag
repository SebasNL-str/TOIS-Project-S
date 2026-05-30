#version 330 core

#define MAX_LIGHTS 16

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// =========================
// TEXTURA
// =========================
uniform sampler2D texture1;

// =========================
// CÁMARA
// =========================
uniform vec3 viewPos;

// =========================
// LIGHT STRUCT
// =========================
struct Light
{
    int type;          // 0 = Point, 1 = Directional, 2 = Spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

// =========================
// SPOT CONFIG (GLOBAL SIMPLE)
// =========================
uniform float spotCutOff;      // cos(angle)
uniform float spotOuterCutOff; // cos(angle)

void main()
{
    // Tomar color con alpha
    vec4 texColor = texture(texture1, TexCoord);

    // Opcional: descartar píxeles casi transparentes
    if (texColor.a < 0.1)
        discard;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // =========================
    // AMBIENT BASE
    // =========================
    vec3 ambient = 0.08 * texColor.rgb;
    result += ambient;

    // =========================
    // LIGHT LOOP
    // =========================
    for (int i = 0; i < numLights; i++)
    {
        vec3 L;
        float attenuation = 1.0;

        // POINT LIGHT
        if (lights[i].type == 0)
        {
            L = normalize(lights[i].position - FragPos);
            float dist = length(lights[i].position - FragPos);
            attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        }
        // DIRECTIONAL LIGHT
        else if (lights[i].type == 1)
        {
            L = normalize(-lights[i].direction);
            attenuation = 1.0;
        }
        // SPOT LIGHT
        else if (lights[i].type == 2)
        {
            vec3 lightDir = normalize(lights[i].position - FragPos);
            L = lightDir;

            float theta = dot(lightDir, normalize(-lights[i].direction));
            float epsilon = max(spotCutOff - spotOuterCutOff, 0.0001);
            float intensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

            attenuation = intensity;

            float dist = length(lights[i].position - FragPos);
            attenuation *= 1.0 / (1.0 + 0.09 * dist);
        }

        // DIFFUSE
        float diff = max(dot(N, L), 0.0);

        // SPECULAR (Blinn-Phong)
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), 32.0);

        vec3 lightColor = lights[i].color * lights[i].intensity;

        vec3 diffuse = diff * lightColor * texColor.rgb;
        vec3 specular = spec * lightColor * 0.25;

        result += (diffuse + specular) * attenuation;
    }

    // =========================
    // TONEMAPPING
    // =========================
    result = result / (result + vec3(1.0));

    // Mantener alpha de la textura
    FragColor = vec4(result, texColor.a);
}
