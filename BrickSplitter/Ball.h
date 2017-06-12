#pragma once

#include "Common.h"

class Ball
{    
public:    
    Ball(float X, float Y);

    CircleShape shape;
    Vector2f velocity;
    float ballRadius;
    float ballVelocity;

    void update(FrameTime mFT);    

    float x() const noexcept;
    float y() const noexcept;
    float left() const noexcept;
    float right() const noexcept;
    float top() const noexcept;
    float bottom() const noexcept;
};