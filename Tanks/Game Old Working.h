#pragma once
#include "Screen.h"
#include "Character.h"
#include "Player.h"
#include <SFML/Network.hpp> 
#include "Menu.h"
#include "TextureManager.h"

class Game : public IMenuCallBack
{
public:
   Game();
   ~Game();

private:
   struct InformationMessage
   {
      float xPosition;
      float yPosition;
      float angle;
      float xVelocity;
      float yVelocity;
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
   void StartNewLevel();
   void UpdatePlayerLives();

   Screen mScreen;
   TextureManager mTextureManager;
   SoundManager mSoundManager;
   Menu mMenu;

   float mLevelSpeed;
   int mLives;

   Message mMessage;
   sf::TcpSocket mSocket;
   Player* mpOurTank;
   Player* mpTheirTank;

   // false is waiting for message 
   bool mSendingMessage;


};
