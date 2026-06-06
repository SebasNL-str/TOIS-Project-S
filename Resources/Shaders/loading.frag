#version 330 core
out vec4 FragColor;

in vec2 vPos; 

uniform vec3 barColor;
uniform float globalTime; 
uniform bool isSpinner;
uniform vec3 screenSize; // x = width, y = height, z = aspectRatio

// Función matemática para dibujar trazos rectos
float DrawSegment(vec2 p, vec2 a, vec2 b, float thickness) {
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return smoothstep(thickness, thickness - 0.01, length(pa - ba * h));
}

void main() {
    if (isSpinner) {
        // DETECTAR PUNTO CENTRAL: OpenGL asigna valores válidos a gl_PointCoord 
        // únicamente si el fragmento se encuentra dentro del área de un GL_POINT.
        // Si la distancia de vPos al origen es menor a un umbral, estamos en el centro.
        if (length(vPos) < 0.01) {
            // Mapeamos gl_PointCoord (va de 0 a 1) a un plano local centrado de -1.0 a 1.0
            vec2 p = gl_PointCoord * 2.0 - 1.0;
            p.y = -p.y; // Invertir el eje Y de OpenGL para que la S no salga al revés

            // Parámetros de la S (escala y grosor relativo al punto de 160.0f)
            float sScale = 0.45; 
            float thickness = 0.12; 

            // Puntos clave del trazo de la letra S
            vec2 ptTopRight = vec2(sScale * 0.7, sScale);
            vec2 ptTopLeft  = vec2(-sScale * 0.7, sScale);
            vec2 ptMidLeft  = vec2(-sScale * 0.7, 0.0);
            vec2 ptMidRight = vec2(sScale * 0.7, 0.0);
            vec2 ptBotRight = vec2(sScale * 0.7, -sScale);
            vec2 ptBotLeft  = vec2(-sScale * 0.7, -sScale);

            // Construcción del trazo de la S
            float sLetter = 0.0;
            sLetter += DrawSegment(p, ptTopRight, ptTopLeft, thickness);
            sLetter += DrawSegment(p, ptTopLeft, ptMidLeft, thickness);
            sLetter += DrawSegment(p, ptMidLeft, ptMidRight, thickness);
            sLetter += DrawSegment(p, ptMidRight, ptBotRight, thickness);
            sLetter += DrawSegment(p, ptBotRight, ptBotLeft, thickness);

            if (sLetter > 0.0) {
                FragColor = vec4(barColor, 1.0); // Pintar la S en Beige brillante
            } else {
                discard; // Volver invisible el resto del cuadrado negro del punto central
            }
        } 
        else {
            // --- LÓGICA DE LOS 8 CÍRCULOS DEL ANILLO (MARQUESINA) ---
            float angle = atan(vPos.y, vPos.x);
            float normalizedAngle = (angle + 3.14159265) / (2.0 * 3.14159265);
            
            int circleIndex = int(floor(normalizedAngle * 8.0));
            if (circleIndex > 7) circleIndex = 7;
            if (circleIndex < 0) circleIndex = 0;

            float speed = 6.0; 
            int activeIndex = int(floor(globalTime * speed)) % 8;

            vec3 finalColor;
            if (circleIndex == activeIndex) {
                finalColor = barColor; 
            } else {
                finalColor = vec3(0.08, 0.08, 0.08); 
            }
            
            FragColor = vec4(finalColor, 1.0);
        }
    } else {
        // Barra de progreso beige sólida normal
        FragColor = vec4(barColor, 1.0);
    }
}

