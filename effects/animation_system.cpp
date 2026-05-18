#include "animation_system.hpp"

AnimationSystem::AnimationSystem() {

    animationValue = 0.0f;
}

void AnimationSystem::update() {

    if (animationValue < 1.0f) {

        animationValue += 0.015f;
    }

    if (animationValue > 1.0f) {

        animationValue = 1.0f;
    }
}

float AnimationSystem::value() {

    return animationValue;
}
