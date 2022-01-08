#pragma once
#include "CAnimation.hpp"
#include <cmath>
#include <memory>

enum class EasingType
{
    Linear,
    InSine,
    OutSine,
    // InOutSine, //TODO: Implement
    // InCubic,
    // OutCubic,
    // InOutCubic,
    // InQuint,
    // OutQuint,
    // InOutQuint,
    // InCirc,
    // OutCirc,
    // InOutCirc,
    // InElastic,
    // OutElastic,
    // InOutElastic,
    // InQuad,
    // OutQuad,
    // InOutQuad,
    // InQuart,
    // OutQuart,
    // InOutQuart,
    // InExpo,
    // OutExpo,
    // InOutExpo,
    // InBack,
    // OutBack,
    // InOutBack,
    // InBounce,
    // OutBounce,
    // InOutBounce
};

inline static const float pi_v = std::acos(-1.0f);
struct Linear
{
    float operator()(float t) { return t; }
};
struct InSine
{
    float operator()(float t) { return 1.0f - std::cos((t * pi_v) / 2.0f); }
};
struct OutSine
{
    float operator()(float t) { return 1.0f - std::sin((t * pi_v) / 2.0f); }
};

std::function<float(float)> mapEnumToFunctor(EasingType type)
{
    switch (type)
    {
        case EasingType::Linear:
            return Linear();
        case EasingType::InSine:
            return InSine();
        case EasingType::OutSine:
            return OutSine();
        default:
            break;
    }
    return Linear();
}

template <typename Obj, typename ValueType, typename ValueSetter,
          typename = std::enable_if_t<std::is_invocable_v<ValueSetter, Obj&, ValueType const&>>>
class ValueSettingAnimation : public CAnimation
{
public:
    ValueSettingAnimation(std::weak_ptr<Obj> obj, ValueType start, ValueType end, std::chrono::milliseconds duration,
                          EasingType type = EasingType::Linear)
        : m_Object{std::move(obj)}, m_Start{std::move(start)}, m_End{std::move(end)}, m_CurrentTime{0},
          m_Duration{duration}, m_TimeTransformer{mapEnumToFunctor(type)}
    {
    }
    ValueSettingAnimation(std::weak_ptr<Obj> obj, ValueType start, ValueType end, std::chrono::milliseconds duration,
                          std::function<float(float)> time_transforemer)
        : m_Object{std::move(obj)}, m_Start{std::move(start)}, m_End{std::move(end)}, m_CurrentTime{0},
          m_Duration{duration}, m_TimeTransformer{std::move(time_transforemer)}
    {
    }
    virtual void play_impl(std::chrono::milliseconds ms) override
    {
        m_CurrentTime += ms;
        // clang-format off
        const float scale_factor = [this, ms]() {
                if (m_CurrentTime < m_Duration)
                {
                    const float dt = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::milliseconds::period>>(m_Duration - m_CurrentTime).count() 
                                   / std::chrono::duration_cast<std::chrono::duration<float, std::chrono::milliseconds::period>>(m_Duration).count();
                    return m_TimeTransformer(dt);
                }
                else
                {
                    m_Finished = true;
                    return m_TimeTransformer(1.0f);
                }
            }();
        // clang-format on
        if (!m_Object->expired())
        {
            m_ValueSetter(*m_Object->lock(), (m_End - m_Start) * scale_factor);
        }
        else
        {
            m_Finished = true;
        }
    }

protected:
    std::weak_ptr<Obj> m_Object;
    ValueType m_Start;
    ValueType m_End;
    std::chrono::milliseconds m_CurrentTime;
    std::chrono::milliseconds m_Duration;
    std::function<float(float)> m_TimeTransformer;
    ValueSetter m_ValueSetter;
};
