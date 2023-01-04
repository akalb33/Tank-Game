#include "stdafx.h"
#include "Util.h"
#include "Player.h"
#include <assert.h>
#include "Boundary.h"
#include "SFML/Network.hpp"
#include "Explosion.h"
#include "Particle.h"
#include "PlayerBullet.h"
#include "Character.h"
#include <iostream>

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
   , mCurrentNumberOfBullets(0)
   , mpExplosion(0)
   , mYourTankHit(false)
   , mPlayerLives()
   , mInvincible(false)
   , mTimeAccumulator(0.0f)
   , mFirstLivePositionX(0.0f)
   , mFirstLivePositionY(0.0f)
{
   SetTexture(TextureManager::player1);
   SetDisplayOrder(180);
}

Player::~Player()
{
}

void Player::Reset()
{
   mScale = 0.5f;
   mAngle = -90.0f;
   for (int i = 0; i < 2; i++)
   {
      PlayerLives* pPlayerLives = Character::Resurrect<PlayerLives>();
      assert(pPlayerLives != 0);
      pPlayerLives->mY = 800.0f;
      if (mTankIsOurOwn)
      {
         pPlayerLives->mX = 50.0f + i * 50.0f;
      }
      else
      {
         pPlayerLives->mX = 1700.0f + i * 50.0f;
      }
      mPlayerLives.push_back(pPlayerLives);
   }
}

void Player::Process(float deltaTime)
{
   if (mInvincible)
   {
      mTimeAccumulator += deltaTime;
      if (mTimeAccumulator > 6.0f)
      {
         mTimeAccumulator = 0.0f;
         mInvincible = false;
         mSprite.setColor(sf::Color(255, 255, 255));
      }
   }
   if (GetStatus() == dying)
   {
      ProcessDying(deltaTime);
   }
   else
   {
      if (mTankIsOurOwn)
      {
         HandleOurTankProcess(deltaTime);
      }
      else
      {
         HandleOtherTankProcess(deltaTime);
      }
   }
}

void Player::HandleOurTankProcess(float deltaTime)
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

   std::vector<PlayerBullet*> pBullets;
   if (Collision<PlayerBullet>(pBullets))
   {
      for (std::vector<PlayerBullet*>::iterator it = pBullets.begin(); it != pBullets.end(); it++)
      {
         if ((*it)->IsOurBullet())
         {
            Kill();
            (*it)->Kill();
            mMyTankHit = true;
         }
      }
   }
   HandleFiringLogic();
}

void Player::HandleFiringLogic()
{
// Firing Logic
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
   {
      if (!mSpacebarPreviouslyPressed)
      {
         unsigned int count = 0;
         std::vector<PlayerBullet*> bullets;
         Character::FindAllCharacters<PlayerBullet>(Character::Status::alive, bullets);
         for (std::vector<PlayerBullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
         {
            if ((*it)->IsOurBullet())
            {
               count++;
            }
         }
         if (count < PlayerBullet::mMaxBulletCount)
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
               pBullet->SetOurBullet(true);
               pBullet->AssignSequenceId();
            }
         }
      }
      mSpacebarPreviouslyPressed = true;
   }
   else
   {
      mSpacebarPreviouslyPressed = false;
   }
}

void Player::HandleOtherTankProcess(float deltaTime)
{
   MoveTowardLocation(mMsgX, mMsgY, 110.0f, deltaTime);

   float deltaAngle = 100.0f * deltaTime;

   if (deltaAngle >= abs(mMsgAngle - mAngle))
      mAngle = mMsgAngle;
   else if (mMsgAngle > mAngle)
      mAngle += deltaAngle;
   else
      mAngle -= deltaAngle;

   std::vector<PlayerBullet*> pBullets;
   if (Collision<PlayerBullet>(pBullets))
   {
      for (std::vector<PlayerBullet*>::iterator it = pBullets.begin(); it != pBullets.end(); it++)
      {
         if ((*it)->IsOurBullet())
         {
            (*it)->Kill();
            mYourTankHit = true;
            Kill();
         }
      }
   }
}

void Player::Kill()
{
   if (!mInvincible)
   {
      GetSoundManager()->Play(SoundManager::explosion);
      mpExplosion = Character::Resurrect<Explosion>();
      assert(mpExplosion != 0);
      mpExplosion->mX = mX;
      mpExplosion->mY = mY;
      mpExplosion->mScale = 0.1f;
      SetStatus(StatusManager::dying);
      SetTankInvincible(true);
   }
}

void Player::CalculateBarrelTipDisplacementFromCenterOfSprite(float x, float y)
{
   mBarrelDisplacement = sqrt(x * x + y * y);
}

void Player::ProcessDying(float deltaTime)
{
   mpExplosion->mScale += 0.05f * deltaTime;
   if (mpExplosion->mScale > 0.2f)
   {
      mpExplosion->Kill();
      if (!mPlayerLives.empty())
      {
         mPlayerLives.back()->Kill();
         mPlayerLives.pop_back();
         SetStatus(StatusManager::alive);
      }
      else
      {
         SetStatus(StatusManager::dead);
      }
   }
}

void Player::SetTankInvincible(bool invincible) 
{ 
   mInvincible = invincible; 
   if (invincible)
   {
      mSprite.setColor(sf::Color(255, 255, 255, 50));
   }
   else
   {
      mSprite.setColor(sf::Color(255, 255, 255));
   }
}