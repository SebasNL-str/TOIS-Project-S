#pragma once

// GLM include
#include <glm.hpp>

// Configuracion editable de la animacion inicial del menu || Editable menu intro animation settings
struct MenuIntroAnimationSettings
{
    bool enabled = true;
    float durationSeconds = 1.15f;
    float startCenterXPercent = 0.50f;
    float endCenterXPercent = 0.29f;
    float centerYPercent = 0.50f;
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

    glm::vec2 GetPanelPosition(float screenWidth, float screenHeight, float panelWidth, float panelHeight);

private:
    MenuIntroAnimationSettings settings;
    bool played;
    bool playing;
    double startedAtSeconds;

    float GetProgress() const;
    float EaseOutCubic(float value) const;
    double NowSeconds() const;
};
