#pragma once

#include "Constants.h"
#include <SFML/Graphics.hpp>

class FadeManager {
private:
    FadeState state = FadeState::None;
    float alpha = 0.f;
    static constexpr float FadeSpeed = 520.f;  // Alpha units per second

public:
    FadeManager() = default;

    // State queries
    FadeState GetState() const { return state; }
    float GetAlpha() const { return alpha; }
    bool IsFading() const { return state != FadeState::None; }
    bool IsFadingOut() const { return state == FadeState::FadeOut; }
    bool IsFadingIn() const { return state == FadeState::FadeIn; }

    // Control fade transition
    void StartFadeOut() {
        state = FadeState::FadeOut;
        alpha = 0.f;
    }

    void StartFadeIn() {
        state = FadeState::FadeIn;
        alpha = 255.f;
    }

    void StartFade() {
        StartFadeOut();
    }

    void Complete() {
        alpha = (state == FadeState::FadeOut) ? 255.f : 0.f;
        state = FadeState::None;
    }

    // Update fade animation
    void Update(float dt) {
        if (state == FadeState::FadeOut) {
            alpha += FadeSpeed * dt;
            if (alpha >= 255.f) {
                alpha = 255.f;
                state = FadeState::FadeIn;
            }
        } else if (state == FadeState::FadeIn) {
            alpha -= FadeSpeed * dt;
            if (alpha <= 0.f) {
                alpha = 0.f;
                state = FadeState::None;
            }
        }
    }

    // Get overlay color with current alpha
    sf::Color GetOverlayColor() const {
        return sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha));
    }
};
