#include "CAnimationManager.hpp"
#include <algorithm>

void CAnimationManager::cancelAnimationById(AnimationId id)
{
    const auto animation_to_be_removed =
        std::find_if(m_AnimationList.cbegin(), m_AnimationList.cend(),
                     [id](decltype(m_AnimationList)::value_type const& animation) { return animation.id == id; });
    if (animation_to_be_removed != m_AnimationList.cend())
    {
        animation_to_be_removed->ptr->invokeFinishedCallback();
        m_AnimationList.erase(animation_to_be_removed);
    }
}
ScopedAnimationId CAnimationManager::addAnimation(std::unique_ptr<IAnimation> animation)
{
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return {m_AnimationList.back().id, this};
}
AnimationId CAnimationManager::addAnimationRawId(std::unique_ptr<IAnimation> animation)
{
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return m_AnimationList.back().id;
}
void CAnimationManager::play(std::chrono::milliseconds ms)
{
    for (auto& animation_with_id : m_AnimationList)
    {
        animation_with_id.ptr->play(ms);
    }
    const auto new_end = std::remove_if(
        m_AnimationList.begin(), m_AnimationList.end(),
        [](decltype(m_AnimationList)::value_type const& animation) { return animation.ptr->isFinished(); });
    std::for_each(new_end, m_AnimationList.end(), [](decltype(m_AnimationList)::value_type const& animation) {
        animation.ptr->invokeFinishedCallback();
    });
    m_AnimationList.erase(new_end, m_AnimationList.end());
}

AnimationId CAnimationManager::createUniqueAnimationId()
{
    static constexpr AnimationId INITIAL_VALUE = static_cast<AnimationId>(5000);
    static std::atomic<AnimationId> counter{INITIAL_VALUE};
    return counter++;
}