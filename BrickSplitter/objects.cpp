#include <SFML/Graphics.hpp>
#include <iostream> 
#include <chrono>

using namespace std;
using namespace sf;

// Structures
// structure == public class, used for data sets
struct Ball
{
	CircleShape shape;
	Vector2f velocity{ -ballVelocity, -ballVelocity };

	Ball(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setRadius(ballRadius);
		shape.setFillColor(Color::Red);
		shape.setOrigin(ballRadius, ballRadius);
	}

	void update(FrameTime mFT) {
		shape.move(velocity * mFT);

		if (left() < 0) velocity.x = ballVelocity;
		else if (right() > WINDOW_WIDTH) velocity.x = -ballVelocity;

		if (top() < 0) velocity.y = ballVelocity;
		else if (bottom() > WINDOW_HEIGHT) velocity.y = -ballVelocity;
	}

	float x() const noexcept { return shape.getPosition().x; }
	float y() const noexcept { return shape.getPosition().y; }
	float left() const noexcept { return x() - shape.getRadius(); }
	float right() const noexcept { return x() + shape.getRadius(); }
	float top() const noexcept { return y() - shape.getRadius(); }
	float bottom() const noexcept { return y() + shape.getRadius(); }
};

struct Rectangle {
	RectangleShape shape;

	float x() const noexcept { return shape.getPosition().x; }
	float y() const noexcept { return shape.getPosition().y; }
	float left() const noexcept { return x() - shape.getSize().x / 2.f; }
	float right() const noexcept { return x() + shape.getSize().x / 2.f; }
	float top() const noexcept { return y() - shape.getSize().y / 2.f; }
	float bottom() const noexcept { return y() + shape.getSize().y / 2.f; }
};

struct Paddle : public Rectangle {
	Vector2f velocity;

	bool moveLeft = false, moveRight = false;

	Paddle(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Black);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}

	void update(FrameTime mFT) {
		shape.move(velocity * mFT);

		if (Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A)) {
			moveLeft = true;
		}
		else {
			moveLeft = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D)) {
			moveRight = true;
		}
		else {
			moveRight = false;
		}

		if (moveLeft && left() > 0) {
			velocity.x = -paddleVelocity;
		}
		else if (moveRight && right() < WINDOW_WIDTH) {
			velocity.x = paddleVelocity;
		}
		else velocity.x = 0;
	}
};

struct Brick : Rectangle {
	bool destroyed = false;

	Brick(float X, float Y) {
		shape.setPosition(X, Y);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Black);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}
};

template <class T1, class T2>
bool isIntersecting(T1& mA, T2& mB) {
	return mA.right() >= mB.left() && mA.left() <= mB.right() &&
		mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall) {
	if (!isIntersecting(mPaddle, mBall)) return;

	mBall.velocity.y = -ballVelocity;

	if (mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity;
	else mBall.velocity.x = ballVelocity;
}

void testCollision(Brick& mBrick, Ball& mBall) {
	if (!isIntersecting(mBrick, mBall)) return;

	mBrick.destroyed = true;

	float overlapLeft{ mBall.right() - mBrick.left() };
	float overlapRight{ mBrick.right() - mBall.left() };
	float overlapTop{ mBall.bottom() - mBrick.top() };
	float overlapBottom{ mBrick.bottom() - mBall.top() };

	bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
	bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

	float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
	float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

	if (abs(minOverlapX) < abs(minOverlapY)) mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
	else mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}