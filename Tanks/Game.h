#pragma once
#include "Screen.h"
#include "Character.h"
#include "Player.h"
#include "PlayerBullet.h"
#include <SFML/Network.hpp> 
#include "Menu.h"
#include "TextureManager.h"

class Game : public IMenuCallBack
{
public:
   Game();
   ~Game();

private:
   struct TankInfo
   {
      //StatusManager::Status status;
      float x;
      float y;
      float angle;
      float angularVelocity;
      float xVelocity;
      float yVelocity;
      bool yourTankHit;
      bool myTankHit;
      bool invincible;
   };

   struct BulletInfo
   {
      // If 0 means not a bullet at all 
      int sequenceId;
      float angle;
      float x;
      float y;
   };


   struct InformationMessage
   {
      TankInfo tankInfo;
      unsigned int numberOfBullets;
      BulletInfo bulletInfo[PlayerBullet::mMaxBulletCount];
   };

   union Message
   {
      InformationMessage informationMessage;
      char c[1];
   };

   void SetMaxCharacters();
   virtual void UpdateMenuOptions();
   void HandleCommunications();
   void StartNewGame();

   Screen mScreen;
   TextureManager mTextureManager;
   SoundManager mSoundManager;
   Menu mMenu;

   bool mClient;
   Message mMessage;
   sf::TcpSocket mSocket;
   Player* mpOurTank;
   Player* mpTheirTank;

   // false is waiting for message 
   bool mSendingMessage;


};
