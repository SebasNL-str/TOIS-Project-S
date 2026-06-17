#include "MenuAnimation.h"

// std includes
#include <algorithm>
#include <chrono>

MenuIntroAnimation::MenuIntroAnimation() :
    played(false),
    playing(false),
    startedAtSeconds(0.0)
{
}

void MenuIntroAnimation::Configure(const MenuIntroAnimationSettings& newSettings)
{
    settings = newSettings;
}

const MenuIntroAnimationSettings& MenuIntroAnimation::GetSettings() const
{
    return settings;
}

MenuIntroAnimationSettings& MenuIntroAnimation::EditSettings()
{
    return settings;
}

void MenuIntroAnimation::Start()
{
    if (!settings.enabled || played)
    {
        Skip();
        return;
    }

    playing = true;
    startedAtSeconds = NowSeconds();
}

void MenuIntroAnimation::Skip()
{
    playing = false;
    played = true;
}

bool MenuIntroAnimation::HasPlayed() const
{
    return played;
}

bool MenuIntroAnimation::IsPlaying() const
{
    return playing;
}

glm::vec2 MenuIntroAnimation::GetPanelPosition(float screenWidth, float screenHeight, float panelWidth, float panelHeight)
{
    float progress = GetProgress();
    if (playing && progress >= 1.0f)
    {
        Skip();
    }

    float easedProgress = EaseOutCubic(progress);
    float startCenterX = screenWidth * settings.startCenterXPercent;
    float endCenterX = screenWidth * settings.endCenterXPercent;
    float centerX = startCenterX + (endCenterX - startCenterX) * easedProgress;
    float centerY = screenHeight * settings.centerYPercent;

    return glm::vec2(centerX - panelWidth * 0.5f, centerY - panelHeight * 0.5f);
}

float MenuIntroAnimation::GetProgress() const
{
    if (!playing)
    {
        return 1.0f;
    }

    if (settings.durationSeconds <= 0.0f)
    {
        return 1.0f;
    }

    double elapsed = NowSeconds() - startedAtSeconds;
    float progress = static_cast<float>(elapsed / settings.durationSeconds);
    return std::max(0.0f, std::min(progress, 1.0f));
}

float MenuIntroAnimation::EaseOutCubic(float value) const
{
    float clampedValue = std::max(0.0f, std::min(value, 1.0f));
    float inverse = 1.0f - clampedValue;
    return 1.0f - inverse * inverse * inverse;
}

double MenuIntroAnimation::NowSeconds() const
{
    using Clock = std::chrono::steady_clock;
    return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
}
