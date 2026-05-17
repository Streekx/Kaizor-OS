#pragma once

class AnimationSystem {

private:

    float animationTime;

public:

    AnimationSystem();

    void update();

    float getAnimationTime();
};
