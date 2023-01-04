#pragma once

#include "Character.h"

class PlayerBullet : public Character
{
public:
   PlayerBullet();
   virtual ~PlayerBullet();    
   void Reset();
   
   virtual void Process(float deltaTime);

   unsigned int GetSequenceId() { return mSequenceId; }
   void AssignSequenceId() { mSequenceId = mNextSequenceId++; }
   void SetSequenceId(int id) { mSequenceId = id; }
   static void SetNextSequenceId(int id) { mNextSequenceId = id; }
   void SetOurBullet(bool isOurBullet) { mOurBullet = isOurBullet; }
   bool IsOurBullet() { return mOurBullet; }
private:
   int mBoundariesContactedCount;
   float mPreviousX = 0.0f;
   float mPreviousY = 0.0f;

   int mSequenceId;
   static int mNextSequenceId;
   bool mOurBullet;
public:
   float mMsgX;
   float mMsgY;
   static const unsigned int mMaxBulletCount = 2;
};
