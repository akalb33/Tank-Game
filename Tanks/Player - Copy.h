#pragma once

#include "Character.h"

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

private:
   bool mSpacebarPreviouslyPressed;
   void ProcessDying(float deltaTime);
   float mBarrelDisplacement;
   float mPreviousX;
   float mPreviousY;
   float mPreviousAngle;
   bool mTankIsOurOwn;
   unsigned int mCurrentNumberOfBullets;
public: 
   float mMsgX;
   float mMsgY;
   float mMsgAngle;
};
