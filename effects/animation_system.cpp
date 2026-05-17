#include "animation_system.hpp"

AnimationSystem::AnimationSystem() {

    animationTime = 0.0f;
}

void AnimationSystem::update() {

    animationTime += 0.016f;

    if (
        animationTime > 1000.0f
    ) {

        animationTime = 0.0f;
    }
}

float AnimationSystem::getAnimationTime() {

    return animationTime;
}
