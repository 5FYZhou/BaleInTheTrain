#pragma once

#include <SFML/System/Vector2.hpp>
#include <Tweeny/tweeny.h>
#include <iostream>

#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

enum class EaseType
{
    Linear,
    QuadraticInOut,
    CubicInOut
};

class Animation
{
public:
    std::string name;
    bool playing = false;

    explicit Animation(std::string n = "")
        : name(std::move(n)) { std::cout << name << std::endl; }

#pragma region ITack
private:
    struct ITrack
    {
        virtual void Start() = 0;
        virtual void Update(float dt) = 0;
        virtual bool Finished() const = 0;
        virtual ~ITrack() = default;
    };

    template <typename T>
    struct Track : ITrack
    {
        bool inited = false;

        T *value = nullptr;

        T from;
        T to;

        int durationMs;
        EaseType ease;

        tweeny::tween<T> tween;

        bool started = false;
        bool finished = false;

        Track(T &bind, T target, int dur, EaseType e, bool ini)
            : value(&bind), from(bind), to(target), durationMs(dur), ease(e), inited(ini) {}

        void Start() override
        {
            if (!inited)
            {
                std::cout << "[Warning] Track hasn't init \n";
                return;
            }
            from = *value; // ⭐关键：启动时取当前值
            // std::cout<<"track"<<from<<endl;

            tween = tweeny::from(from).to(to).during(durationMs);

            switch (ease)
            {
            case EaseType::Linear:
                tween.via(tweeny::easing::linear);
                break;
            case EaseType::QuadraticInOut:
                tween.via(tweeny::easing::quadraticInOut);
                break;
            case EaseType::CubicInOut:
                tween.via(tweeny::easing::cubicInOut);
                break;
            }

            started = true;
        }

        void Update(float dt) override
        {
            if (!inited || !started || finished)
                return;

            *value = tween.step(static_cast<int>(dt * 1000.f));

            if (tween.progress() >= 1.f)
            {
                finished = true;
                *value = to;
            }
        }

        bool Finished() const override
        {
            return finished;
        }
    };
#pragma endregion

private:
    bool finished = false;
    std::vector<std::unique_ptr<ITrack>> tracks;

    struct FloatPreset
    {
        float target;
        int duration;
        EaseType ease;
    };

    struct Vec2Preset
    {
        sf::Vector2f target;
        int duration;
        EaseType ease;
    };

    std::unordered_map<int, FloatPreset> floatPresets;
    std::unordered_map<int, Vec2Preset> vec2Presets;

    bool started = false;

public:
    const auto &GetTracks() const { return tracks; }
    bool IsFinished() const { return finished; }

    void Reset()
    {
        tracks.clear();

        floatPresets.clear();
        vec2Presets.clear();

        finished = false;
    }

    //------------------------------------------
    // 带参数版本
    //------------------------------------------

    void AddFloat(int id, float &bind, float target, int dur, EaseType ease)
    {
        floatPresets[id] = {target, dur, ease};
        tracks.push_back(std::make_unique<Track<float>>(bind, target, dur, ease, true));
    }

    void AddVec2(int id, sf::Vector2f &bind, sf::Vector2f target, int dur, EaseType ease)
    {
        vec2Presets[id] = {target, dur, ease};
        float &x = bind.x;
        float &y = bind.y;
        tracks.push_back(std::make_unique<Track<float>>(x, target.x, dur, ease, true));
        tracks.push_back(std::make_unique<Track<float>>(y, target.y, dur, ease, true));
    }

    //------------------------------------------
    // 仅创建ID版本
    //------------------------------------------

    void AddFloat(int id, float target, int dur, EaseType ease)
    {
        floatPresets[id] = {target, dur, ease};
    }

    void AddVec2(int id, sf::Vector2f target, int dur, EaseType ease)
    {
        vec2Presets[id] = {target, dur, ease};
    }

    //------------------------------------------
    // 仅 ID 复用版本
    //------------------------------------------

    void AddFloat(int id, float &bind)
    {
        auto it = floatPresets.find(id);
        if (it == floatPresets.end())
            throw std::runtime_error("AddFloat: unknown id");

        auto &p = it->second;

        tracks.push_back(std::make_unique<Track<float>>(bind, p.target, p.duration, p.ease, true));
    }

    void AddVec2(int id, sf::Vector2f &bind)
    {
        auto it = vec2Presets.find(id);
        if (it == vec2Presets.end())
            throw std::runtime_error("AddVec2: unknown id");

        auto &p = it->second;
        float &x = bind.x;
        float &y = bind.y;
        tracks.push_back(std::make_unique<Track<float>>(x, p.target.x, p.duration, p.ease, true));
        tracks.push_back(std::make_unique<Track<float>>(y, p.target.y, p.duration, p.ease, true));
    }

    //------------------------------------------
    void Start()
    {
        std::cout << "Animation:start" << std::endl;
        playing = true;
        for (auto &t : tracks)
            t->Start();
    }

    void Update(float dt)
    {
        for (auto &t : tracks)
            t->Update(dt);

        tracks.erase(std::remove_if(tracks.begin(), tracks.end(),
                                    [](auto &t)
                                    { return t->Finished(); }),
                     tracks.end());

        finished = tracks.empty();
        if (finished)
        {
            playing = false;
        }
    }
    bool HasRegisteredPreset() const
    {
        return !floatPresets.empty() || !vec2Presets.empty();
    }
    bool IsIdle() const
    {
        return tracks.empty() && floatPresets.empty() && vec2Presets.empty();
    }
};

class AnimationManager
{
private:
    std::unordered_map<std::string, std::shared_ptr<Animation>> animations;

public:
    std::shared_ptr<Animation> Create(const std::string &name)
    {
        auto it = animations.find(name);

        if (it != animations.end())
        {
            if (it->second->playing)
            {
                std::cout << "[Warning] Cannot recreate Animation '"
                          << name << "' while playing.\n";

                return it->second; // 或 return nullptr（看你策略）
            }

            return it->second;
        }

        auto anim = std::make_shared<Animation>(name);
        animations[name] = anim;
        return anim;
    }

    std::shared_ptr<Animation> Get(const std::string &name)
    {
        auto it = animations.find(name);

        if (it == animations.end())
            return nullptr;

        if (it->second->playing)
        {
            std::cout << "[Warning] Animation '" << name
                      << "' is currently playing.\n";
        }

        return it->second;
    }

    void Update(float dt)
    {
        std::vector<std::string> toRemove;

        for (auto it = animations.begin(); it != animations.end(); ++it)
        {
            auto &anim = it->second;

            anim->Update(dt);

            if (anim->IsIdle())
                toRemove.push_back(it->first);
        }

        for (auto &n : toRemove)
            animations.erase(n);
    }

    bool IsPlaying(const std::string &name)
    {
        auto it = animations.find(name);
        return it != animations.end() && it->second->playing;
    }
};

