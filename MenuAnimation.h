
#pragma once

// std includes
#include <cstddef>
#include <vector>

// GLM include
#include <glm.hpp>

// Modos de comportamiento lógicos para el movimiento de la interfaz || Logical behavior modes for interface movement
enum class MenuIntroMotionMode
{
    Grouped,
    Independent,
    SequentialReveal
};

// Identificadores de los componentes visuales del menú || Identifiers for the menu visual components
enum class MenuAnimatedElement
{
    Title,
    Subtitle,
    Item,
    Footer
};

// Parámetros de configuración temporal y geométrica de la animación || Temporal and geometric configuration parameters for the animation
struct MenuElementAnimationSettings
{
    float durationSeconds = 5.0f;
    float startScaleMultiplier = 1.65f;
    glm::vec2 startOffset = glm::vec2(0.0f, 0.0f);
    glm::vec2 finalOffset = glm::vec2(0.0f, 0.0f);
};

// Matriz de estado final con las transformaciones del fragmento || Final state matrix containing fragment transformations
struct MenuElementTransform
{
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float scale = 1.0f;
    float opacity = 1.0f;
};


// Configuracion editable de la animacion inicial del menu || Editable menu intro animation settings
struct MenuIntroAnimationSettings
{
    bool enabled = true;
    MenuIntroMotionMode motionMode = MenuIntroMotionMode::SequentialReveal;

    // Posicion general del menu. En Grouped todos los textos viajan juntos y conservan su layout.
    // General menu position. In Grouped all texts move together and keep their layout.
    float startCenterXPercent = 0.50f;
    float startCenterYPercent = 0.50f;
    float finalCenterXPercent = 0.50f;
    float finalCenterYPercent = 0.50f;
    float groupedDurationSeconds = 2.15f;
    float groupedStartScaleMultiplier = 0.0f;

    // SequentialReveal no mueve el texto: solo lo revela uno por uno en su posicion final.
    // SequentialReveal does not move text: it only reveals each line in its final position.
    float startDelaySeconds = 0.20f;
    float revealDelaySeconds = 0.30f;
    float revealDurationSeconds = 0.28f;
    float titleRevealOrder = 0.0f;
    float subtitleRevealOrder = 1.0f;
    float firstItemRevealOrder = 2.0f;
    float itemRevealStep = 1.0f;
    float footerRevealOrderOffset = 1.0f;

    // Ajustes para Independent. Puedes modificar cada bloque y tambien cada item por indice.
    // Independent settings. You can edit each block and every item by index.
    MenuElementAnimationSettings title;
    MenuElementAnimationSettings subtitle;
    MenuElementAnimationSettings item;
    MenuElementAnimationSettings footer;
    std::vector<MenuElementAnimationSettings> itemOverrides;
};

class MenuIntroAnimation
{
public:
    MenuIntroAnimation();

    // Configurar y recuperar los parametros esteticos de la animacion || Configure and retrieve animation aesthetic parameters
    void Configure(const MenuIntroAnimationSettings& newSettings);
    const MenuIntroAnimationSettings& GetSettings() const;
    MenuIntroAnimationSettings& EditSettings();

    // Controlar el ciclo de ejecucion de la introduccion || Control the introduction execution cycle
    void Start();
    void Skip();
    bool HasPlayed() const;
    bool IsPlaying() const;

    // Calcular las coordenadas de origen de la interfaz || Calculate interface origin coordinates
    glm::vec2 GetMenuOrigin(float screenWidth, float screenHeight, float menuWidth, float menuHeight);

    // Calcular transformaciones dinamicas para componentes individuales || Calculate dynamic transformations for individual components
    MenuElementTransform GetElementTransform(
        MenuAnimatedElement element,
        std::size_t itemIndex,
        const glm::vec2& finalPosition,
        float finalScale,
        float screenWidth,
        float screenHeight,
        float menuWidth,
        float menuHeight);

private:
    // Variables de control de estado y temporizacion || State control and timing variables
    MenuIntroAnimationSettings settings;
    bool played;
    bool playing;
    double startedAtSeconds;

    // Rutinas internas para el cálculo de interpolaciones y progreso || Internal routines for interpolation and progress calculation
    const MenuElementAnimationSettings& GetIndependentSettings(MenuAnimatedElement element, std::size_t itemIndex) const;
    float GetLongestIndependentDuration() const;
    float GetRevealOrder(MenuAnimatedElement element, std::size_t itemIndex) const;
    float GetElapsedSeconds() const;
    float GetProgress(float durationSeconds) const;
    float EaseOutCubic(float value) const;
    double NowSeconds() const;
};

