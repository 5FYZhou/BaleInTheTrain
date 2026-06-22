#pragma once
#include "Constants.h"
#include "UI/AnimationManager.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <deque>


class RewardAnimation
{
public:
    std::shared_ptr<Animation> anim;
    std::deque<CardView> cards;

    bool hasInit = false;
    AnimationManager& aniMgr;

private:
    sf::Vector2f backpack = {1735.f, 67.f};

    CardView MakeFanCard(PileType type, int index, int count, sf::Vector2f center, float yOffset, float spacingX, float angleRange)
    {
        CardView cv;
        cv.texType = cardTexMap.at(type);

        float mid = (count - 1) / 2.f;
        float offset = index - mid;

        float t = (count <= 1) ? 0.f : offset / mid;

        cv.basePosition = {
            center.x + offset * spacingX,
            center.y + (t * t) * 25.f + yOffset};

        cv.rotation = t * angleRange;
        cv.alpha = 255.f;

        return cv;
    }

public:
    RewardAnimation(AnimationManager& ani) : aniMgr(ani){}

    void Set(const std::vector<PileType> &input)
    {
        std::cout<<"set"<<std::endl;
        if (hasInit)
            return;
        hasInit = true;
        
        anim = aniMgr.Create("reward_card");
        anim->Reset();

        // position
        anim->AddVec2(0, backpack, 1000, EaseType::CubicInOut);

        // scale
        anim->AddFloat(1, 0.f, 1000, EaseType::CubicInOut);

        // alpha
        anim->AddFloat(2, 0.f, 1000, EaseType::CubicInOut);

        cards.clear();

        std::vector<PileType> strike, defend;

        for (auto c : input)
        {
            if (c == PileType::Strike)
                strike.push_back(c);
            else
                defend.push_back(c);
        }

        auto build = [&](std::vector<PileType> &group, sf::Vector2f center)
        {
            int n = (int)group.size();

            for (int i = 0; i < n; i++)
            {
                CardView fan = MakeFanCard(group[i], i, n, center, 0.f, 80.f, 18.f);

                cards.push_back(fan);

                CardView& rc = cards.back();

                anim->AddVec2(0, rc.basePosition);
                anim->AddFloat(1, rc.scale);
                anim->AddFloat(2, rc.alpha);
            }
        };

        build(strike, {960.f, 380.f});
        build(defend, {960.f, 500.f});
    }

    void Start()
    {
        anim->Start();
    }
};