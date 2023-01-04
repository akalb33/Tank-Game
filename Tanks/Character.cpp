#include "stdafx.h"
#include "Character.h"
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <iostream>

std::vector<Character*> Character::mCharacters;
std::vector<Character*> Character::mDisplayList;
TextureManager* Character::mpTextureManager = 0;
SoundManager* Character::mpSoundManager = 0;
Screen* Character::mpScreen = 0;

// User should never have to change this class.
Character::Character(const std::type_info& type)
   : mStatusManager(type)
   , mSprite()
   , mProcessOrder(0)
   , mDisplayOrder(0)
   , mX(0.0f)
   , mY(0.0f)
   , mAngle(0.0f)
   , mScale(1.0f)
   , mVelocityX(0.0f)
   , mVelocityY(0.0f)
   , mAngularVelocity(0.0f)
   , mPolygon()
   , mType(type)
   , mPreviousX(-mX)
   , mPreviousY(-mY)
   , mPreviousAngle(-mAngle)
   , mPreviousScale(-mScale)
{
}

Character::~Character()
{
}

void Character::ProcessAll(float deltaTime)
{
   for (std::vector<Character*>::iterator it = mCharacters.begin();
      it != mCharacters.end();
      it++)
   {
      if ((*it)->GetStatus() != dead)
      {
         (*it)->Process(deltaTime);
      }
   }
}

void Character::DisplayAll()
{
   for (std::vector<Character*>::iterator it = mDisplayList.begin();
      it != mDisplayList.end();
      it++)
   {
      if ((*it)->GetStatus() != dead)
      {
         (*it)->Display();
         // hitboxes
         //(*it)->mPolygon.DisplayCollisionBoundary(*mpScreen);
      }
   }
}

void Character::SetProcessOrder(unsigned int order)
{
   mProcessOrder = order;
   std::vector<Character*>::iterator it =
      std::find(mCharacters.begin(), mCharacters.end(), this);

   if(it != mCharacters.end())
   {
      mCharacters.erase(it);

      if (mCharacters.empty())
      {
         mCharacters.push_back(this);
      }
      else
      {
         for(it = mCharacters.begin();
             ((it != mCharacters.end()) && ((*it)->mProcessOrder < mProcessOrder));
            it++)
         {
            continue;
         }

         mCharacters.insert(it, this);
      }
   }
}

void Character::SetDisplayOrder(unsigned int order)
{
   mDisplayOrder = order;
   std::vector<Character*>::iterator it =
      std::find(mDisplayList.begin(), mDisplayList.end(), this);

   if(it != mDisplayList.end())
   {
      mDisplayList.erase(it);

      if (mDisplayList.empty())
      {
         mDisplayList.push_back(this);
      }
      else
      {
         for(it = mDisplayList.begin();
             ((it != mDisplayList.end()) && ((*it)->mDisplayOrder < mDisplayOrder));
             it++)
         {
            continue;
         }

         mDisplayList.insert(it, this);
      }
   }
}

void Character::SetTexture(TextureManager::Textures textureId)
{
   mSprite.setTexture(GetTextureManager()->GetTexture(textureId), true);
   mPolygon = GetTextureManager()->GetPolygon(textureId);
   mSprite.setOrigin(mPolygon.GetOriginX(), mPolygon.GetOriginY());
}

void Character::Display()
{
   mSprite.setPosition(mX, mY);
   mSprite.setRotation(mAngle);
   mSprite.setScale(mScale, mScale);
   mpScreen->draw(mSprite);
}

bool Character::MoveTowardLocation(float destinationX, 
                                   float destinationY, 
                                   float velocity, 
                                   float deltaTime)
{
   bool arrived = false;

   if((destinationX != mX) || (destinationY != mY))
   {
      float dx = destinationX - mX;
      float dy = destinationY - mY;

      float x2 = dx * dx;
      float y2 = dy * dy;

      float magnitude = velocity * deltaTime / sqrt(x2 + y2);

      mX += dx * magnitude;
      mY += dy * magnitude;

      if(x2 > y2)
      {
         if(dx > 0.0f)
         {
            if(mX > destinationX)
               arrived = true;
         }
         else if(mX < destinationX)
         {
            arrived = true;
         }
      }
      else
      {
         if (dy > 0.0f)
         {
            if (mY > destinationY)
               arrived = true;
         }
         else if (mY < destinationY)
         {
            arrived = true;
         }
      }

      if(arrived)
      {
         mX = destinationX;
         mY = destinationY;
      }
   }
   else
   {
      arrived = true;
   }

   return arrived;
}

void Character::WrapPosition()
{
   float minX = mpScreen->GetViewX();
   float maxX = minX + mpScreen->GetViewWidth();
   float minY = mpScreen->GetViewY();
   float maxY = minY + mpScreen->GetViewHeight();

   if(mX  < minX)
      mX = maxX;
   else if (mX > maxX)
      mX = minX;

   if(mY  < minY)
      mY = maxY;
   else if (mY > maxY)
      mY = minY;   
}

bool Character::IsOffScreen()
{
   bool result = false;

   float minX = mpScreen->GetViewX();
   float maxX = minX + mpScreen->GetViewWidth();
   float minY = mpScreen->GetViewY();
   float maxY = minY + mpScreen->GetViewHeight();

   if((mX < minX) || (mX > maxX) || (mY < minY) || (mY > maxY))
      result = true;

   return result;
}
