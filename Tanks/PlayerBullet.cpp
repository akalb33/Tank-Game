#include "stdafx.h"
#include "Util.h"
#include "PlayerBullet.h"
#include "Boundary.h"
#include "Particle.h"
#include "ScoreManager.h"
#include "Character.h"

int PlayerBullet::mNextSequenceId = 0;

PlayerBullet::PlayerBullet()
   : Character(typeid(PlayerBullet))
   , mBoundariesContactedCount(0)
   , mPreviousX(0.0f)
   , mPreviousY(0.0f)
   , mSequenceId(0)
   , mOurBullet(true)
   , mMsgX(0.0f)
   , mMsgY(0.0f)
{
   SetTexture(TextureManager::playerBullet);
   SetDisplayOrder(170);
   mScale = 0.1f;
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Reset()
{
   mBoundariesContactedCount = 0;
   mVelocityX = 0;
   mVelocityY = 0;
}

void PlayerBullet::Process(float deltaTime)
{
   if (mOurBullet)
   {
      ApplyInertia(deltaTime);
      std::vector<Boundary*> mBoundariesContacted;
      if (Character::Collision<Boundary>(mBoundariesContacted))
      {
         std::vector<Boundary*>::iterator it = mBoundariesContacted.begin();
         if (mBoundariesContactedCount == 3)
         {
            Kill();
            Reset();
         }
         else
         {                                           
            Boundary* pBoundary = *it;
            float angle = 0.0f;
            if (pBoundary->CalculateAngleOfClosestSide(mX, mY, mX - cos(mAngle * Util::RAD_PER_DEG) * 6.0f, mY - sin(mAngle * Util::RAD_PER_DEG) * 6.0f, angle))
            {
               mX = mX - cos(mAngle * Util::RAD_PER_DEG) * 6.0f;
               mY = mY - sin(mAngle * Util::RAD_PER_DEG) * 6.0f;
               mAngle = angle * 2.0f - mAngle;
               Util::VectorToRectangle(200.0f, mAngle, mVelocityX, mVelocityY);
               mBoundariesContactedCount++;
            }
         }
      }
      mPreviousX = mX;
      mPreviousY = mY;
   }
   else
   {
      MoveTowardLocation(mMsgX, mMsgY, 200.0f, deltaTime);
   }                                                          
}