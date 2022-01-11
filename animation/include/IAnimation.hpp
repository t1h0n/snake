#pragma once
#include "TypeUtilities.hpp"
#include <chrono>
#include <cstdint>
#include <functional>

namespace animation
{
using AnimationId = std::uint32_t;

template <typename DurationType = std::chrono::milliseconds>
class IAnimationImpl
{
    static_assert(detail::has_member_rep_v<DurationType>);

protected:
    static constexpr auto ZERO_DURATION = DurationType{static_cast<typename DurationType::rep>(0)};

public:
    virtual void play(DurationType const& t) = 0;
    virtual void invokeFinishedCallback() const = 0;
    virtual bool isFinished() const = 0;
    virtual void setFinishedCallback(const std::function<void()>& callback) = 0;

    IAnimationImpl() = default;
    virtual ~IAnimationImpl() = default;
};

using IAnimation = IAnimationImpl<>;
} // namespace animation