/**
BrickSplitter
Entities/Block.cpp

@author   Mark Arneman (mark)
*/

#include <Core.h>
#include <Ball.cpp>
#include <Block.cpp>

namespace ME
{
    namespace Collision
    {
        template <class T1, class T2>
        bool isIntersecting(T1& mA, T2& mB) {
            return mA.right() >= mB.left() && mA.left() <= mB.right() &&
                mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
        }

        void testCollision(Paddle& mPaddle, Entity::Ball& mBall) {
            if (!isIntersecting(mPaddle, mBall)) return;

            mBall.velocity.y = -ballVelocity;

            if (mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity;
            else mBall.velocity.x = ballVelocity;
        }

        void testCollision(Brick& mBrick, Entity::Ball& mBall) {
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
    }    
}