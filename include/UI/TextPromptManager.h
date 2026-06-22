#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <map>

#include "UI/AnimationManager.h"

enum class PromptStyle
{
    Center,
    Top,
    Bottom
};

struct StyleConfig
{
    sf::Vector2f positionOffset = {0.f, 0.f};

    unsigned size = 40;

    float stayTime = 1.f;
    float moveDistance = 80.f;
    float fadeDuration = 0.8f;

    sf::Color color = sf::Color::White;
};

inline const std::unordered_map<PromptStyle, StyleConfig> styleMap = {
    {PromptStyle::Center,
     {sf::Vector2f(0.f, 540.f),
      40,
      1.f,
      80.f,
      0.8f,
      sf::Color::White}},

    {PromptStyle::Top,
     {sf::Vector2f(0.f, 100.f),
      80,
      2.f,
      20.f,
      1.f,
      sf::Color(255, 220, 120)}},

    {PromptStyle::Bottom,
     {sf::Vector2f(0.f, 820.f),
      36,
      1.2f,
      100.f,
      1.0f,
      sf::Color(180, 220, 255)}}};

class TextPromptManager
{
private:
    struct Prompt
    {
        std::string text;

        sf::Vector2f basePosition;
        sf::Vector2f animOffset;

        float alpha = 255.f;

        unsigned size = 40;

        PromptStyle style = PromptStyle::Center;

        std::string animName;

        bool alive = true;

        Prompt(
            const std::string &t,
            unsigned s,
            sf::Vector2f pos,
            PromptStyle st = PromptStyle::Center)
            : text(t),
              size(s),
              basePosition(pos),
              animOffset(0.f, 0.f),
              style(st)
        {
        }

        sf::Vector2f GetPosition() const
        {
            return {
                basePosition.x + animOffset.x,
                basePosition.y + animOffset.y};
        }
    };

    AnimationManager *animationManager = nullptr;

    float screenWidth = 1920.f;

    int nextId = 0;

    void PushExistingPrompts(
        PromptStyle style,
        float offset)
    {
        for (auto &p : prompts)
        {
            if (!p->alive)
                continue;

            if (p->style != style)
                continue;

            p->basePosition.y -= offset;
        }
    }

public:
    std::vector<std::unique_ptr<Prompt>> prompts;

    explicit TextPromptManager(AnimationManager &mgr, float width = 1920.f)
        : animationManager(&mgr), screenWidth(width)
    {
    }

    // 创建提示
    void Show(
        const std::string &text,
        float y = 540.f,
        unsigned size = 40,
        float stayTime = 1.f,
        float moveDistance = 80.f,
        float fadeDuration = 0.8f)
    {
        auto p = std::make_unique<Prompt>(
            text,
            size,
            sf::Vector2f(screenWidth * 0.5f, y));

        p->alpha = 255.f;

        p->animName = "TextPrompt_" + std::to_string(nextId++);

        auto anim = animationManager->Create(p->animName);
        anim->Reset();

        // 等待时间（停留）
        anim->SetDelay(stayTime);

        // 上移动画
        anim->AddVec2(
            0,
            p->animOffset,
            {0.f,
             -moveDistance},
            static_cast<int>(
                fadeDuration * 1000.f),
            EaseType::QuadraticInOut);

        // 淡出动画
        anim->AddFloat(
            1,
            p->alpha,
            0.f,
            static_cast<int>(fadeDuration * 1000.f),
            EaseType::Linear);

        anim->Start();

        prompts.push_back(std::move(p));
    }

    void Show(
        const std::string &text,
        PromptStyle style)
    {
        std::cout
            << "Create Prompt "
            << text
            << std::endl;
        const auto &cfg = styleMap.at(style);

        //----------------------------------
        // 把同类型旧提示顶上去
        //----------------------------------

        float spacing = cfg.size * 1.2f;

        PushExistingPrompts(
            style,
            spacing);

        //----------------------------------
        // 创建新提示
        //----------------------------------

        auto p = std::make_unique<Prompt>(
            (text),
            cfg.size,
            sf::Vector2f(
                screenWidth * 0.5f +
                    cfg.positionOffset.x,
                cfg.positionOffset.y),
            style);

        p->alpha = 255.f;

        p->animName =
            "TextPrompt_" +
            std::to_string(nextId++);

        auto anim =
            animationManager->Create(
                p->animName);

        anim->Reset();

        //----------------------------------
        // 停留
        //----------------------------------

        anim->SetDelay(cfg.stayTime);

        //----------------------------------
        // 上移
        //----------------------------------

        anim->AddVec2(
            0,
            p->animOffset,
            {0.f,
             -cfg.moveDistance},
            static_cast<int>(
                cfg.fadeDuration * 1000.f),
            EaseType::QuadraticInOut);
        //----------------------------------
        // 淡出
        //----------------------------------

        anim->AddFloat(
            1,
            p->alpha,
            0.f,
            static_cast<int>(
                cfg.fadeDuration *
                1000.f),
            EaseType::Linear);

        anim->Start();

        prompts.push_back(
            std::move(p));
    }
    // 更新逻辑（只管生命周期）
    void Update(float /*dt*/)
    {
        for (auto &p : prompts)
        {
            auto anim = animationManager->Get(p->animName);

            if (anim && anim->IsFinished())
            {
                p->alive = false;
            }
        }

        prompts.erase(
            std::remove_if(
                prompts.begin(),
                prompts.end(),
                [](const std::unique_ptr<Prompt> &p)
                {
                    return !p->alive;
                }),
            prompts.end());
    }

    bool Empty() const
    {
        return prompts.empty();
    }

    size_t Count() const
    {
        return prompts.size();
    }

    void Clear(PromptStyle style)
    {
        prompts.erase(
            std::remove_if(
                prompts.begin(),
                prompts.end(),
                [&](const auto &p)
                {
                    if (p->style != style)
                        return false;

                    if (auto anim =
                            animationManager->Get(
                                p->animName))
                    {
                        anim->Reset();
                    }

                    return true;
                }),
            prompts.end());
    }

    void ClearAll()
    {
        for (auto &p : prompts)
        {
            std::cout << p->text << std::endl;
            if (auto anim =
                    animationManager->Get(
                        p->animName))
            {
                anim->Reset();
            }
        }

        prompts.clear();
    }
};