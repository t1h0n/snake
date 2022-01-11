#pragma once
#include "CAnimation.hpp"
#include <cmath>
#include <memory>
namespace animation
{
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

inline static const float pi_v = std::acos(-1.0F);
struct Linear
{
    float operator()(float t) { return t; }
};
struct InSine
{
    float operator()(float t) { return 1.0F - std::cos((t * pi_v) / 2.0F); }
};
struct OutSine
{
    float operator()(float t) { return std::sin((t * pi_v) / 2.0F); }
};
namespace detail
{
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
} // namespace detail
template <typename Obj, typename ValueType, typename ValueSetter, typename DurationType = std::chrono::milliseconds>
class ValueSettingAnimationImpl : public CAnimation<DurationType>
{
    static_assert(std::is_invocable_v<ValueSetter, Obj&, ValueType const&, ValueType const&, float>);

public:
    ValueSettingAnimationImpl(const std::shared_ptr<Obj>& obj, ValueType const& start, ValueType const& end, DurationType const& duration,
                              EasingType type = EasingType::Linear)
        : m_Object{obj}, m_StartValue{start}, m_EndValue{end}, m_CurrentTime{static_cast<typename DurationType::rep>(0)},
          m_Duration{duration}, m_TimeTransformer{detail::mapEnumToFunctor(type)}
    {
        assert(m_StartValue != m_EndValue && m_Duration >= IAnimationImpl<DurationType>::ZERO_DURATION);
    }
    ValueSettingAnimationImpl(const std::shared_ptr<Obj>& obj, ValueType const& start, ValueType const& end, DurationType const& duration,
                              const std::function<float(float)>& time_transforemer)
        : m_Object{obj}, m_StartValue{start}, m_EndValue{end}, m_CurrentTime{static_cast<typename DurationType::rep>(0)},
          m_Duration{duration}, m_TimeTransformer{time_transforemer}
    {
        assert(m_TimeTransformer && m_StartValue != m_EndValue && m_Duration >= IAnimationImpl<DurationType>::ZERO_DURATION);
    }
    virtual void play_impl(DurationType const& t) override
    {
        m_CurrentTime += t;
        // clang-format off
        const float scale_factor = [this]() {
                if (m_CurrentTime < m_Duration)
                {
                    const float dt = std::chrono::duration_cast<std::chrono::duration<float, typename DurationType::period>>(m_CurrentTime).count() 
                                   / std::chrono::duration_cast<std::chrono::duration<float, typename DurationType::period>>(m_Duration).count();
                    return m_TimeTransformer(dt);
                }
                else
                {
                    CAnimation<DurationType>::m_Finished = true;
                    return m_TimeTransformer(1.0F);
                }
            }();
        // clang-format on
        if (!m_Object.expired())
        {
            m_ValueSetter(*m_Object.lock(), m_StartValue, m_EndValue, scale_factor);
        }
        else
        {
            CAnimation<DurationType>::m_Finished = true;
        }
    }

protected:
    std::weak_ptr<Obj> m_Object;
    ValueType m_StartValue;
    ValueType m_EndValue;
    DurationType m_CurrentTime;
    DurationType m_Duration;
    std::function<float(float)> m_TimeTransformer;
    ValueSetter m_ValueSetter;
};
} // namespace animation