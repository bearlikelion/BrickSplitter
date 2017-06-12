/**
BrickSplitter
main.cpp

@author   Mark Arneman (mark)
@created  6/9/2017 7:28 AM
*/

#include <chrono>

#include "Common.h"
#include "Ball.h"

constexpr unsigned int WINDOW_WIDTH{ 800 }, WINDOW_HEIGHT{ 600 };
constexpr float paddleWidth{ 60.0f }, paddleHeight{ 20.0f }, paddleVelocity{ 0.6f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };
constexpr float ftStep{ 1.0f }, ftSlice{ 1.0f };
 
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

    mBall.velocity.y = -mBall.ballVelocity;;

    if (mBall.x() < mPaddle.x()) mBall.velocity.x = -mBall.ballVelocity;    
    else mBall.velocity.x = mBall.ballVelocity;
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

    if (abs(minOverlapX) < abs(minOverlapY)) mBall.velocity.x = ballFromLeft ? -mBall.ballVelocity : mBall.ballVelocity;
    else mBall.velocity.y = ballFromTop ? -mBall.ballVelocity : mBall.ballVelocity;
}    

struct Game
{
    // Window and frame time calculations
    RenderWindow window{ { WINDOW_WIDTH, WINDOW_HEIGHT }, "[marneman] Brick Splitter" };
    FrameTime lastFt{ 0.f }, currentSlice{ 0.f };
    bool running{ false };

    // Define entities               
    Ball * ball = new Ball(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 75);
    Paddle paddle{ WINDOW_WIDTH / 2, WINDOW_HEIGHT - 50 };
    vector<Brick> bricks;        

    // Construct game
    Game()
    {
        ContextSettings settings;
        settings.antialiasingLevel = 8;
        settings.majorVersion = 0;
        settings.minorVersion = 1;

        // window.setVerticalSyncEnabled = true;    
        window.setFramerateLimit(300);

        for (int x = 0; x < countBlocksX; x++) {
            for (int y = 0; y < countBlocksY; y++) {
                bricks.emplace_back((x + 1) * (paddleWidth + 3) + 22, (y + 2) * (paddleHeight + 3));
            }
        }
    }

    // Run game loop
    void run()
    {
        running = true;

        while (running && window.isOpen())
        {
            // Start time interval
            auto timePoint1(chrono::high_resolution_clock::now());
            window.clear(Color::White); // "Clear" the window from the previous frame

            // Game phases
            inputPhase();
            updatePhase();
            drawPhase();

            // End time interval and calculate time elapsed
            auto timePoint2(chrono::high_resolution_clock::now());
            auto elapsedTime(timePoint2 - timePoint1);

            // Convert frame time <ft> to milliseconds
            FrameTime ft{ chrono::duration_cast<chrono::duration<float, milli>>(elapsedTime).count() };
            lastFt = ft;

            // Frames per second
            auto ftSeconds(ft / 1000.f);
            auto fps(1.f / ftSeconds);
            window.setTitle("FT: " + to_string(ft) + "    FPS: " + to_string(fps));
        }
    }

    void inputPhase()
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                break;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) running = false; // Quit if Escape is pressed
    }

    void updatePhase()
    {
        currentSlice += lastFt;

        for (; currentSlice >= ftSlice; currentSlice -= ftSlice) {
            ball->update(ftStep);            
            paddle.update(ftStep);
            testCollision(paddle, *ball);

            for (auto& brick : bricks) testCollision(brick, *ball);
            bricks.erase(remove_if(begin(bricks), end(bricks), [](const Brick& mBrick) {
                return mBrick.destroyed;
            }), end(bricks));
        }
    }

    void drawPhase()
    {                    
        window.draw(ball->shape);
        window.draw(paddle.shape);
        for (auto& brick : bricks) window.draw(brick.shape);
        window.display();
    }
};

int main() {
    cout << "Initialize";
    Game{}.run();
    return 0;
}