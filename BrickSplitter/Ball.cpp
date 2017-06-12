#include "Ball.h"

constexpr float ballRadius{ 10.0f }, ballVelocity{ 0.5f };
  
CircleShape shape;
Vector2f velocity = { -ballVelocity, -ballVelocity };

// Initialize ball entity
Ball::Ball(float X, float Y) {                
    shape.setPosition(X, Y);
    shape.setRadius(ballRadius);
    shape.setFillColor(Color::Red);
    shape.setOrigin(ballRadius, ballRadius);
}

void Ball::update(FrameTime mFT) {
    shape.move(velocity * mFT);

    if (left() < 0) velocity.x = ballVelocity;
    else if (right() > 600) velocity.x = -ballVelocity;

    if (top() < 0) velocity.y = ballVelocity;
    else if (bottom() > 800) velocity.y = -ballVelocity;
}

float Ball::x() const noexcept { return shape.getPosition().x; }
float Ball::y() const noexcept { return shape.getPosition().y; }
float Ball::left() const noexcept { return x() - shape.getRadius(); }
float Ball::right() const noexcept { return x() + shape.getRadius(); }
float Ball::top() const noexcept { return y() - shape.getRadius(); }
float Ball::bottom() const noexcept { return y() + shape.getRadius(); }