#version 330 core
// Color de salida del fragmento || Fragment output color
out vec4 FragColor;

// Coordenadas de entrada desde el Vertex Shader || Input coordinates from Vertex Shader
in vec2 vPos; 

// Variables uniformes de control || Uniform control variables
uniform vec3 barColor;
uniform float globalTime; 
uniform bool isSpinner;
uniform vec3 screenSize; 
uniform float loadingOpacity;

// Funcion matematica para dibujar trazos rectos || Mathematical function to draw straight lines
float DrawSegment(vec2 p, vec2 a, vec2 b, float thickness) {
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return smoothstep(thickness, thickness - 0.01, length(pa - ba * h));
}

void main() {
    // Validar si es la pantalla de carga || Check if it is the loading screen
    if (isSpinner) {
        
        // Detectar si el fragmento esta en el punto central || Detect if fragment is in the central point
        if (length(vPos) < 0.01) {
            
            // Mapear coordenadas a espacio local centrado || Map coordinates to local centered space
            vec2 p = gl_PointCoord * 2.0 - 1.0;
            p.y = -p.y; 

            // Dimensiones relativas del glifo || Relative dimensions of the glyph
            float sScale = 0.45; 
            float thickness = 0.12; 

            // Puntos de control para la letra S || Control points for the letter S
            vec2 ptTopRight = vec2(sScale * 0.7, sScale);
            vec2 ptTopLeft  = vec2(-sScale * 0.7, sScale);
            vec2 ptMidLeft  = vec2(-sScale * 0.7, 0.0);
            vec2 ptMidRight = vec2(sScale * 0.7, 0.0);
            vec2 ptBotRight = vec2(sScale * 0.7, -sScale);
            vec2 ptBotLeft  = vec2(-sScale * 0.7, -sScale);

            // Combinar segmentos de la geometria || Combine geometry segments
            float sLetter = 0.0;
            sLetter += DrawSegment(p, ptTopRight, ptTopLeft, thickness);
            sLetter += DrawSegment(p, ptTopLeft, ptMidLeft, thickness);
            sLetter += DrawSegment(p, ptMidLeft, ptMidRight, thickness);
            sLetter += DrawSegment(p, ptMidRight, ptBotRight, thickness);
            sLetter += DrawSegment(p, ptBotRight, ptBotLeft, thickness);

            // Pintar texto o descartar el fragmento || Paint text or discard the fragment
            if (sLetter > 0.0) {
                FragColor = vec4(barColor, loadingOpacity); 
            } else {
                discard; 
            }
        } 
        else {
            // Calcular angulo polar del fragmento || Calculate polar angle of the fragment
            float angle = atan(vPos.y, vPos.x);
            float normalizedAngle = (angle + 3.14159265) / (2.0 * 3.14159265);
            
            // Obtener indice del circulo del anillo || Get circle index of the ring
            int circleIndex = int(floor(normalizedAngle * 8.0));
            if (circleIndex > 7) circleIndex = 7;
            if (circleIndex < 0) circleIndex = 0;

            // Calcular circulo activo segun el tiempo || Calculate active circle based on time
            float speed = 6.0; 
            int activeIndex = int(floor(globalTime * speed)) % 8;

            // Asignar color de la animacion circular || Assign color of the circular animation
            vec3 finalColor;
            if (circleIndex == activeIndex) {
                finalColor = barColor; 
            } else {
                finalColor = vec3(0.08, 0.08, 0.08); 
            }
            
            FragColor = vec4(finalColor, loadingOpacity);
        }
    } else {
        // Color solido para la barra de progreso estandar || Solid color for standard progress bar
        FragColor = vec4(barColor, loadingOpacity);
    }
}


