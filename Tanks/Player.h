#pragma once

#include "Character.h"
#include "PlayerLives.h"
#include "Explosion.h"

class Player : public Character
{
public:
   Player();
   virtual ~Player();
   void Reset();
   virtual void Process(float deltaTime);
   virtual void Kill();
   void CalculateBarrelTipDisplacementFromCenterOfSprite(float x, float y);
   void SetTankAsOurOwn(bool own) { mTankIsOurOwn = own; }
   bool IsTankOurOwn() { return mTankIsOurOwn; }
   int GetPlayerLivesCount() { return mPlayerLives.size(); }
   bool IsTankInvincible() { return mInvincible; }
   void SetTankInvincible(bool invincible);

private:
   void HandleOurTankProcess(float deltaTime);
   void HandleFiringLogic();
   void HandleOtherTankProcess(float deltaTime);

   bool mSpacebarPreviouslyPressed;
   void ProcessDying(float deltaTime);
   float mBarrelDisplacement;
   float mPreviousX;
   float mPreviousY;
   float mPreviousAngle;
   bool mTankIsOurOwn;
   unsigned int mCurrentNumberOfBullets;
   Explosion* mpExplosion;
   std::vector<PlayerLives*> mPlayerLives;
   bool mInvincible;
   float mTimeAccumulator;
   float mFirstLivePositionX;
   float mFirstLivePositionY;
public: 
   bool mMyTankHit;
   bool mYourTankHit;
   float mMsgX;
   float mMsgY;
   float mMsgAngle;
};
