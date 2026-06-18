
#pragma once

// std includes
#include <cstddef>
#include <vector>

// GLM include
#include <glm.hpp>

enum class MenuIntroMotionMode
{
    Grouped,
    Independent,
    SequentialReveal
};

enum class MenuAnimatedElement
{
    Title,
    Subtitle,
    Item,
    Footer
};

struct MenuElementAnimationSettings
{
    float durationSeconds = 5.0f;
    float startScaleMultiplier = 1.65f;
    glm::vec2 startOffset = glm::vec2(0.0f, 0.0f);
    glm::vec2 finalOffset = glm::vec2(0.0f, 0.0f);
};

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

    void Configure(const MenuIntroAnimationSettings& newSettings);
    const MenuIntroAnimationSettings& GetSettings() const;
    MenuIntroAnimationSettings& EditSettings();

    void Start();
    void Skip();
    bool HasPlayed() const;
    bool IsPlaying() const;

    glm::vec2 GetMenuOrigin(float screenWidth, float screenHeight, float menuWidth, float menuHeight);
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
    MenuIntroAnimationSettings settings;
    bool played;
    bool playing;
    double startedAtSeconds;

    const MenuElementAnimationSettings& GetIndependentSettings(MenuAnimatedElement element, std::size_t itemIndex) const;
    float GetLongestIndependentDuration() const;
    float GetRevealOrder(MenuAnimatedElement element, std::size_t itemIndex) const;
    float GetElapsedSeconds() const;
    float GetProgress(float durationSeconds) const;
    float EaseOutCubic(float value) const;
    double NowSeconds() const;
};
