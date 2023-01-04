#include "stdafx.h"
#include "Util.h"
#include "Player.h"
#include "Boundary.h"
#include "SFML/Network.hpp"
#include "Particle.h"
#include "PlayerBullet.h"
#include "Character.h"

Player::Player()
   : Character(typeid(Player))
   , mSpacebarPreviouslyPressed(false)
   , mBarrelDisplacement(0.0f)
   , mPreviousX(0.0f)
   , mPreviousY(0.0f)
   , mPreviousAngle(0.0f)
   , mTankIsOurOwn(true)
   , mMsgX(0.0f)
   , mMsgY(0.0f)
   , mMsgAngle(0.0f)
{
   SetTexture(TextureManager::player);
   SetDisplayOrder(180);
   mScale = 0.1f;
   mAngle = -90.0f;
}

Player::~Player()
{
}

void Player::Reset()
{ 
   mAngle = -90.0f;
}

void Player::Process(float deltaTime)
{
   if (mTankIsOurOwn)
   {
      if (GetStatus() == dying)
      {
         ProcessDying(deltaTime);
      }
      else
      {
         mPreviousAngle = mAngle;
         mPreviousX = mX;
         mPreviousY = mY;

         // Movement Logic
         mAngularVelocity = 0.0f;
         mVelocityX = 0.0f;
         mVelocityY = 0.0f;
         std::vector<Boundary*> mBoundaries;
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
         {
            mAngularVelocity = -90.0f;
            //mAngle -= 2.0f;
            if (Collision<Boundary>(mBoundaries))
            {
               mAngle = mPreviousAngle;
            }
         }
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
         {
            mAngularVelocity = 90.0f;
            //mAngle += 2.0f;
            if (Collision<Boundary>(mBoundaries))
            {
               mAngle = mPreviousAngle;
            }
         }
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
         {
            Util::VectorToRectangle(100.0f, mAngle, mVelocityX, mVelocityY);
         }
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
         {
            Util::VectorToRectangle(-100.0f, mAngle, mVelocityX, mVelocityY);
         }


         ApplyInertia(deltaTime);
         if (Collision<Boundary>(mBoundaries))
         {
            mX = mPreviousX;
            mY = mPreviousY;
            mAngle = mPreviousAngle;
         }


         // Firing Logic
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
         {
            if (!mSpacebarPreviouslyPressed)
            {
               PlayerBullet* pBullet = Resurrect<PlayerBullet>();
               if (pBullet != 0)
               {
                  //GetSoundManager()->Stop(SoundManager::bulletNoise);
                  GetSoundManager()->Play(SoundManager::bulletNoise);
                  float barrelTipX = 0.0f;
                  float barrelTipY = 0.0f;
                  Util::VectorToRectangle(mBarrelDisplacement, mAngle, barrelTipX, barrelTipY);
                  pBullet->mX = mX + barrelTipX;
                  pBullet->mY = mY + barrelTipY;
                  Util::VectorToRectangle(200.0f, mAngle, pBullet->mVelocityX, pBullet->mVelocityY);
                  pBullet->mAngle = mAngle;
                  pBullet->AssignSequenceId();
               }
            }
            mSpacebarPreviouslyPressed = true;
         }
         else
         {
            mSpacebarPreviouslyPressed = false;
         }
      }
   }
   else
   {
      MoveTowardLocation(mMsgX, mMsgY, 110.0f, deltaTime);

      float deltaAngle = 100.0f * deltaTime;

      if (deltaAngle >= abs(mMsgAngle - mAngle))
         mAngle = mMsgAngle;
      else if (mMsgAngle > mAngle)
         mAngle += deltaAngle;
      else
         mAngle -= deltaAngle;

      //ApplyInertia(deltaTime);
   }
}

void Player::Kill()
{
   GetSoundManager()->Play(SoundManager::explosion);
   SetStatus(StatusManager::dying);
   mAngularVelocity = Util::RandomFloat(-500.0f, 500.0f);
   Particle::GenerateParticles(100, mX, mY);
}

void Player::CalculateBarrelTipDisplacementFromCenterOfSprite(float x, float y)
{
   mBarrelDisplacement = sqrt(x * x + y * y);
}

void Player::ProcessDying(float deltaTime)
{
   ApplyInertia(deltaTime);
   mVelocityY += 1500.0f * deltaTime;
   if (mY > 5080.0f)
   {
      SetStatus(StatusManager::dead);
   }
}
