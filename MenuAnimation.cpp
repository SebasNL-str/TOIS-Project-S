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

glm::vec2 MenuIntroAnimation::GetMenuOrigin(float screenWidth, float screenHeight, float menuWidth, float menuHeight)
{
    return glm::vec2(
        screenWidth * settings.finalCenterXPercent - menuWidth * 0.5f,
        screenHeight * settings.finalCenterYPercent - menuHeight * 0.5f);
}

MenuElementTransform MenuIntroAnimation::GetElementTransform(
    MenuAnimatedElement element,
    std::size_t itemIndex,
    const glm::vec2& finalPosition,
    float finalScale,
    float screenWidth,
    float screenHeight,
    float menuWidth,
    float menuHeight)
{
    MenuElementTransform transform;
    transform.position = finalPosition;
    transform.scale = finalScale;
    transform.opacity = 1.0f;

    if (!playing)
    {
        return transform;
    }

    if (settings.motionMode == MenuIntroMotionMode::SequentialReveal)
    {
        float revealOrder = GetRevealOrder(element, itemIndex);
        float revealStart = revealOrder * settings.revealDelaySeconds;
        float revealElapsed = GetElapsedSeconds() - revealStart;
        float progress = settings.revealDurationSeconds <= 0.0f
            ? (revealElapsed >= 0.0f ? 1.0f : 0.0f)
            : revealElapsed / settings.revealDurationSeconds;

        progress = std::max(0.0f, std::min(progress, 1.0f));
        transform.opacity = EaseOutCubic(progress);

        if (element == MenuAnimatedElement::Footer &&
            GetElapsedSeconds() >= revealStart + settings.revealDurationSeconds)
        {
            Skip();
        }

        return transform;
    }

    if (settings.motionMode == MenuIntroMotionMode::Grouped)
    {
        float progress = GetProgress(settings.groupedDurationSeconds);
        float easedProgress = EaseOutCubic(progress);
        glm::vec2 startOrigin(
            screenWidth * settings.startCenterXPercent - menuWidth * 0.5f,
            screenHeight * settings.startCenterYPercent - menuHeight * 0.5f);
        glm::vec2 finalOrigin = GetMenuOrigin(screenWidth, screenHeight, menuWidth, menuHeight);
        glm::vec2 localPosition = finalPosition - finalOrigin;
        glm::vec2 origin = startOrigin + (finalOrigin - startOrigin) * easedProgress;
        float startScale = finalScale * settings.groupedStartScaleMultiplier;

        transform.position = origin + localPosition;
        transform.scale = startScale + (finalScale - startScale) * easedProgress;

        if (progress >= 1.0f)
        {
            Skip();
        }

        return transform;
    }

    const MenuElementAnimationSettings& elementSettings = GetIndependentSettings(element, itemIndex);
    float progress = GetProgress(elementSettings.durationSeconds);
    float easedProgress = EaseOutCubic(progress);
    glm::vec2 startPosition = finalPosition + elementSettings.startOffset;
    glm::vec2 endPosition = finalPosition + elementSettings.finalOffset;
    float startScale = finalScale * elementSettings.startScaleMultiplier;

    transform.position = startPosition + (endPosition - startPosition) * easedProgress;
    transform.scale = startScale + (finalScale - startScale) * easedProgress;

    if (GetProgress(GetLongestIndependentDuration()) >= 1.0f)
    {
        Skip();
    }

    return transform;
}

const MenuElementAnimationSettings& MenuIntroAnimation::GetIndependentSettings(MenuAnimatedElement element, std::size_t itemIndex) const
{
    if (element == MenuAnimatedElement::Item && itemIndex < settings.itemOverrides.size())
    {
        return settings.itemOverrides[itemIndex];
    }

    switch (element)
    {
    case MenuAnimatedElement::Title:
        return settings.title;
    case MenuAnimatedElement::Subtitle:
        return settings.subtitle;
    case MenuAnimatedElement::Footer:
        return settings.footer;
    case MenuAnimatedElement::Item:
    default:
        return settings.item;
    }
}

float MenuIntroAnimation::GetLongestIndependentDuration() const
{
    float longestDuration = std::max(
        std::max(settings.title.durationSeconds, settings.subtitle.durationSeconds),
        std::max(settings.item.durationSeconds, settings.footer.durationSeconds));

    for (const MenuElementAnimationSettings& itemSettings : settings.itemOverrides)
    {
        longestDuration = std::max(longestDuration, itemSettings.durationSeconds);
    }

    return longestDuration;
}

float MenuIntroAnimation::GetRevealOrder(MenuAnimatedElement element, std::size_t itemIndex) const
{
    switch (element)
    {
    case MenuAnimatedElement::Title:
        return settings.titleRevealOrder;
    case MenuAnimatedElement::Subtitle:
        return settings.subtitleRevealOrder;
    case MenuAnimatedElement::Footer:
        return settings.firstItemRevealOrder +
            static_cast<float>(itemIndex) * settings.itemRevealStep +
            settings.footerRevealOrderOffset;
    case MenuAnimatedElement::Item:
    default:
        return settings.firstItemRevealOrder +
            static_cast<float>(itemIndex) * settings.itemRevealStep;
    }
}

float MenuIntroAnimation::GetElapsedSeconds() const
{
    if (!playing)
    {
        return 0.0f;
    }

    float elapsed = static_cast<float>(NowSeconds() - startedAtSeconds) - settings.startDelaySeconds;
    return std::max(0.0f, elapsed);
}

float MenuIntroAnimation::GetProgress(float durationSeconds) const
{
    if (!playing)
    {
        return 1.0f;
    }

    if (durationSeconds <= 0.0f)
    {
        return 1.0f;
    }

    float progress = GetElapsedSeconds() / durationSeconds;
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
