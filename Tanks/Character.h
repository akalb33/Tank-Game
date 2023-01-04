#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <typeinfo>
#include <vector>
#include "Screen.h"
#include "StatusManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Polygon.h"
#include "Screen.h"

// User should never have to change this class.
class Character 
{
public:
   enum Status
   {
      alive,
      dying,
      dead
   };
   Character(const std::type_info& type);
   virtual ~Character();
   static void SetTextureManager(TextureManager* pTextureManager) { mpTextureManager = pTextureManager; }
   static TextureManager* GetTextureManager() { return mpTextureManager; }
   static void SetSoundManager(SoundManager* pSoundManager) { mpSoundManager = pSoundManager; }
   static SoundManager* GetSoundManager() { return mpSoundManager; }
   static void SetScreen(Screen* pScreen) { mpScreen = pScreen; }
   static Screen* GetScreen() { return mpScreen; }
   static void ProcessAll(float deltaTime);
   static void DisplayAll();

   template <typename T>
   static void SetMaxLimit(unsigned int quantity)
   {
      for (unsigned int i = 0; i < quantity; i++)
      {
         T* pCharacterTemp = new T();

         mCharacters.push_back(pCharacterTemp);
         pCharacterTemp->SetProcessOrder(pCharacterTemp->mProcessOrder);

         mDisplayList.push_back(pCharacterTemp);
         pCharacterTemp->SetDisplayOrder(pCharacterTemp->mDisplayOrder);
      }
   }

   template <typename T>
   static T* Resurrect()
   {
      T* pCharacter = 0;
      for(std::vector<Character*>::iterator it = mCharacters.begin();
          (it != mCharacters.end()) && (pCharacter == 0);
          it++)
      {
         if ((*it)->GetType() == typeid(T) && (*it)->GetStatus() == dead)
         {
            (*it)->SetStatus(StatusManager::alive);
            pCharacter = reinterpret_cast<T*>(*it);
         }
      }
      return pCharacter;
   }

   template <typename T>
   static T* FindCharacter(Character::Status status)
   {
      T* pCharacter = 0;
      for(std::vector<Character*>::iterator it = mCharacters.begin();
          (it != mCharacters.end()) && (pCharacter == 0);
          it++)
      {
         if ((*it)->GetType() == typeid(T) && (*it)->GetStatus() == status)
            pCharacter = reinterpret_cast<T*>(*it);
      }
      return pCharacter;
   }

   template <typename T>
   static T* FindCharacter()
   {
      T* pCharacter = 0;
      for (std::vector<Character*>::iterator it = mCharacters.begin();
         (it != mCharacters.end()) && (pCharacter == 0);
         it++)
      {
         if ((*it)->GetType() == typeid(T))
            pCharacter = reinterpret_cast<T*>(*it);
      }
      return pCharacter;
   }

   template <typename T>
   static void FindAllCharacters(Character::Status status, std::vector<T*>& characters)
   {
      for (std::vector<Character*>::iterator it = mCharacters.begin();
           (it != mCharacters.end());
           it++)
      {
         if ((*it)->GetType() == typeid(T) && (*it)->GetStatus() == status)
            characters.push_back(reinterpret_cast<T*>(*it));
      }
   }

   static void KillAll()
   {
      for (std::vector<Character*>::iterator it = mCharacters.begin();
         (it != mCharacters.end());
         it++)
      {
         (*it)->SetStatus(StatusManager::dead);
      }
   }

   void SetProcessOrder(unsigned int order);
   unsigned int GetProcessOrder() const { return mProcessOrder; }

   void SetDisplayOrder(unsigned int order);
   unsigned int GetDisplayOrder() const { return mDisplayOrder; }

   void SetTexture(TextureManager::Textures textureId);

   void AdjustPoints()
   {
      if((mX != mPreviousX) || (mY != mPreviousY) ||
         (mAngle != mPreviousAngle) || (mScale != mPreviousScale))
      {
         mPreviousX = mX;
         mPreviousY = mY;
         mPreviousAngle = mAngle;
         mPreviousScale = mScale;
         mPolygon.AdjustPoints(mX, mY, mAngle, mScale);
      }
   }

   float GetMinX() { AdjustPoints(); return mPolygon.GetAdjustedMinX(); }
   float GetMaxX() { AdjustPoints(); return mPolygon.GetAdjustedMaxX(); }
   float GetMinY() { AdjustPoints(); return mPolygon.GetAdjustedMinY(); }
   float GetMaxY() { AdjustPoints(); return mPolygon.GetAdjustedMaxY(); }

   template <typename T>
   bool Collision(std::vector<T*>& characters, bool includeDying = false)
   {
      bool found = false;

      AdjustPoints();

      characters.clear();
      for (std::vector<Character*>::iterator it = mCharacters.begin();
         it != mCharacters.end();
         it++)
      {
         if (((*it)->GetType() == typeid(T)) &&
             (*it != this) &&
             ((*it)->GetStatus() == alive) || (includeDying && ((*it)->GetStatus() == dying)))
         {
            (*it)->AdjustPoints();

            if(mPolygon.Collision((*it)->mPolygon))
            {
               characters.push_back(reinterpret_cast<T*>(*it));
               found = true;
            }
         }
      }
      return found;
   }

   template <typename T>
   bool DetectCollisionAndKillBoth(bool includeDying = false)
   {
      bool found = false;

      AdjustPoints();

      for (std::vector<Character*>::iterator it = mCharacters.begin();
           it != mCharacters.end() && !found;
           it++)
      {
         if (((*it)->GetType() == typeid(T)) &&
             (*it != this) &&
             ((*it)->GetStatus() == alive) || (includeDying && ((*it)->GetStatus() == dying)))
         {
            (*it)->AdjustPoints();

            if(mPolygon.Collision((*it)->mPolygon))
            {
               (*it)->Kill();
               Kill();
               found = true;
            }
         }
      }
      return found;
   }


   bool MoveTowardLocation(float destinationX, 
                           float destinationY, 
                           float velocity, 
                           float deltaTime);

   void WrapPosition();

   bool IsOffScreen();

   void ApplyInertia(float deltaTime)
   {
      mX += deltaTime * mVelocityX;
      mY += deltaTime * mVelocityY;
      mAngle += deltaTime * mAngularVelocity;
   }

   float mX;
   float mY;
   float mAngle;
   float mScale;
   float mVelocityX;
   float mVelocityY;
   float mAngularVelocity;

public:
   sf::Sprite mSprite;

   StatusManager mStatusManager;

   unsigned int mProcessOrder;
   unsigned int mDisplayOrder;
   const std::type_info& GetType() { return mType; }
   virtual void Process(float deltaTime) = 0;
   virtual void Display();
   virtual void Kill() { mStatusManager.SetStatus(StatusManager::dead); }
   StatusManager::Status GetStatus() const { return mStatusManager.GetStatus(); }
   void SetStatus(StatusManager::Status status) { mStatusManager.SetStatus(status); }
   static std::vector<Character*> mCharacters;
   static std::vector<Character*> mDisplayList;
   static TextureManager* mpTextureManager;
   static SoundManager* mpSoundManager;
   static Screen* mpScreen;

   Polygon mPolygon;
   const std::type_info& mType;
   float mPreviousX;
   float mPreviousY;
   float mPreviousAngle;
   float mPreviousScale;
};
