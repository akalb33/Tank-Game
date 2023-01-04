#include "stdafx.h"
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "PlayerLives.h"
#include "PlayerBullet.h"
#include "Particle.h"
#include "ScoreManager.h"
#include "Background.h"
#include "Boundary.h"
#include "Util.h"
#include <assert.h>
#include <SFML/System/Time.hpp>
#include <SFML/Network.hpp> 
#include <random>

Game::Game()
   : mScreen(0.0f, 0.0f, 1920.0f, 1080.0f, false, "Tanks")
   , mTextureManager()
   , mSoundManager()
   , mMenu()
   , mLevelSpeed(0.0f)
   , mLives(0)
   , mpTank1(0)
   , mpTank2(0)
{
   Util::InitRandomizer();
   mScreen.setVerticalSyncEnabled(true);

   Character::SetTextureManager(&mTextureManager);
   Character::SetSoundManager(&mSoundManager);
   Character::SetScreen(&mScreen);
   SetMaxCharacters();

   sf::Font font;
   assert(font.loadFromFile("arial.ttf"));
   sf::Text text;
   text.setFont(font);

   ScoreManager::Load();
   mMenu.SetMenuTitle("Tanks");
   mMenu.SetText(text);
   mMenu.AddSetting("Create a Game");
   mMenu.AddSetting("Join a Game");
   mMenu.AddSetting("Exit");
   mMenu.Load();

   sf::TcpListener* pServerListener = 0;
   sf::TcpSocket socket;
   bool isServer = false;


   struct InformationMessage
   {
      bool tank1;
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

   /*struct ClientMessage
   {
      float x;
   };

   struct ServerResponse
   {
      int i;
      int j;
   };

   union Message
   {
      ClientMessage clientMessage;
      ServerResponse serverResponse;
      char c[sizeof(clientMessage)];
   };*/

   Message message;

   bool goToMenu = true;

   sf::Clock clock;
   while (mScreen.isOpen())
   {
      sf::Event event;
      while (mScreen.pollEvent(event) || goToMenu)
      {
         if (event.type == sf::Event::Closed)
            mScreen.close();
         else if (event.type == sf::Event::Resized)
            mScreen.HandleResizeEvent();
         else if (event.type == sf::Event::KeyReleased || goToMenu)
         {
            if (event.key.code == sf::Keyboard::Escape || goToMenu)
            {
               std::string selection = mMenu.ShowMenu(mScreen);
               if ((selection == "Exit") || (selection == "WINDOW_CLOSED_EVENT"))
                  mScreen.close();
               else if (selection == "Create a Game")
               {
                  isServer = true;
                  sf::IpAddress lanAddress = sf::IpAddress::getLocalAddress();
                  std::cout << "Server local ip is " << lanAddress << "\n";
                  pServerListener = new sf::TcpListener();
                  if (pServerListener->listen(53001) != sf::Socket::Done)
                  {
                     std::cout << "Failed to listen.";
                  }

                  std::cout << "Now listening.\n";

                  // accept a new connection
                  if (pServerListener->accept(socket) != sf::Socket::Done)
                  {
                     std::cout << "Timed out. Couldn't find a client.";
                     assert(false);
                  }
                  std::cout << "Success! Client with remote IP \"" << socket.getRemoteAddress() << "\" just connected.";
                  StartNewGame(true);
               }
               else if (selection == "Join a Game")
               {
                  isServer = false;
                  //sf::IpAddress ipAddress = sf::IpAddress::getLocalAddress();
                  //std::cout << "Attempting to connect to local ip of 192.168.0.184.\n";
                  std::cout << "Attempting to connect to local ip of 192.168.0.168.\n";
                  sf::Socket::Status status = socket.connect("192.168.0.168", 53001);
                  //sf::Socket::Status status = socket.connect("192.168.0.184", 53001);
                  if (status != sf::Socket::Done)
                  {
                     std::cout << "Timed out. Couldn't find a server.";
                     assert(false);
                  }
                  std::cout << "Success! Connected to server with remote IP \"" << socket.getRemoteAddress() << "\".\n";
                  StartNewGame(false);
               }

               goToMenu = false;

               clock.restart();
            }
         }
      }

      mScreen.clear();
      Character::ProcessAll(clock.restart().asSeconds());
      Character::DisplayAll();
      ScoreManager::DrawScores(mScreen);

      if (mpTank1->IsReadyToSendInformation())
      {                 
         message.informationMessage.tank1 = true;
         message.informationMessage.angle = mpTank1->mAngle;
         message.informationMessage.xPosition = mpTank1->mX;
         message.informationMessage.yPosition = mpTank1->mY;
         message.informationMessage.xVelocity = mpTank1->mVelocityX;
         message.informationMessage.yVelocity = mpTank1->mVelocityY;      
         socket.send(message.c, sizeof(message.informationMessage));
         mpTank1->SetReadyToSendInformation(false);
      }

      if (mpTank2->IsReadyToSendInformation())
      {
         message.informationMessage.tank1 = false;
         message.informationMessage.angle = mpTank2->mAngle;
         message.informationMessage.xPosition = mpTank2->mX;
         message.informationMessage.yPosition = mpTank2->mY;
         message.informationMessage.xVelocity = mpTank2->mVelocityX;
         message.informationMessage.yVelocity = mpTank2->mVelocityY;
         socket.send(message.c, sizeof(message.informationMessage));
         mpTank2->SetReadyToSendInformation(false);
      }

      socket.setBlocking(false);
      size_t received = 0;
      if (socket.receive(message.c, sizeof(Message), received) == sf::Socket::Status::Done)
      {
         if (message.informationMessage.tank1)
         {
            mpTank1->mAngle = message.informationMessage.angle;
            mpTank1->mX = message.informationMessage.xPosition;
            mpTank1->mY = message.informationMessage.yPosition;
            mpTank1->mVelocityX = message.informationMessage.xVelocity;
            mpTank1->mVelocityY = message.informationMessage.yVelocity;
         }
         else
         {
            mpTank2->mAngle = message.informationMessage.angle;
            mpTank2->mX = message.informationMessage.xPosition;
            mpTank2->mY = message.informationMessage.yPosition;
            mpTank2->mVelocityX = message.informationMessage.xVelocity;
            mpTank2->mVelocityY = message.informationMessage.yVelocity;
         }
      }
      socket.setBlocking(true);

      mScreen.DisplayNewScreen();

      if (Character::FindCharacter<Player>(Character::dead) != 0)
      {
         if (mLives > 0)
         {
            mLives--;
            UpdatePlayerLives();
            Player* pPlayer = Character::Resurrect<Player>();
            pPlayer->Reset();
         }
      }
   }

   mMenu.Save();
   ScoreManager::Save();

   delete pServerListener;
}

Game::~Game()
{

}

void Game::SetMaxCharacters()
{
   Character::SetMaxLimit<Player>(2);
   Character::SetMaxLimit<PlayerBullet>(2);
   Character::SetMaxLimit<PlayerLives>(2);
   Character::SetMaxLimit<Background>(1);
   Character::SetMaxLimit<Particle>(500);
   Character::SetMaxLimit<Boundary>(100);
}

void Game::UpdateMenuOptions()
{
}

void Game::StartNewGame(bool tankIsOurOwn)
{
   Character::KillAll();
   Background* pBackground = Character::Resurrect<Background>();
   assert(pBackground != 0);
   pBackground->mX = 960.0f;
   pBackground->mY = 540.0f;
   mLives = 2;
   UpdatePlayerLives();

   Player* pPlayer = Character::Resurrect<Player>();
   assert(pPlayer != 0);
   pPlayer->Reset();
   pPlayer->mX = 1200.0f;
   pPlayer->mY = 800.0f;
   pPlayer->CalculateBarrelTipDisplacementFromCenterOfSprite(34.0f, 0.0f);
   mpTank1 = pPlayer;	
   
   Player* pPlayer2 = Character::Resurrect<Player>();
   assert(pPlayer2 != 0);
   pPlayer2->Reset();
   pPlayer2->mX = 1200.0f;
   pPlayer2->mY = 500.0f;
   pPlayer2->CalculateBarrelTipDisplacementFromCenterOfSprite(34.0f, 0.0f);
   mpTank2 = pPlayer2;

   if (tankIsOurOwn)
   {
      mpTank1->SetTankAsOurOwn(true);
      mpTank2->SetTankAsOurOwn(false);
   }
   else
   {
      mpTank1->SetTankAsOurOwn(false);
      mpTank2->SetTankAsOurOwn(true);
   }

   Boundary* pTopBoundary = Character::Resurrect<Boundary>();
   assert(pTopBoundary != 0);
   pTopBoundary->Init();
   pTopBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pTopBoundary->AddBoundaryPoint(500.0f, 200.0f);
   pTopBoundary->AddBoundaryPoint(1420.0f, 200.0f);
   pTopBoundary->AddBoundaryPoint(1420.0f, 190.0f);
   pTopBoundary->AddBoundaryPoint(500.0f, 190.0f);

   Boundary* pLeftBoundary = Character::Resurrect<Boundary>();
   assert(pLeftBoundary != 0);
   pLeftBoundary->Init();
   pLeftBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pLeftBoundary->AddBoundaryPoint(500.0f, 200.0f);
   pLeftBoundary->AddBoundaryPoint(500.0f, 880.0f);
   pLeftBoundary->AddBoundaryPoint(490.0f, 880.0f);
   pLeftBoundary->AddBoundaryPoint(490.0f, 200.0f);

   Boundary* pBottomBoundary = Character::Resurrect<Boundary>();
   assert(pBottomBoundary != 0);
   pBottomBoundary->Init();
   pBottomBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pBottomBoundary->AddBoundaryPoint(500.0f, 880.0f);
   pBottomBoundary->AddBoundaryPoint(1420.0f, 880.0f);
   pBottomBoundary->AddBoundaryPoint(1420.0f, 890.0f);
   pBottomBoundary->AddBoundaryPoint(500.0f, 890.0f);

   Boundary* pRightBoundary = Character::Resurrect<Boundary>();
   assert(pRightBoundary != 0);
   pRightBoundary->Init();
   pRightBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pRightBoundary->AddBoundaryPoint(1420.0f, 880.0f);
   pRightBoundary->AddBoundaryPoint(1420.0f, 200.0f);
   pRightBoundary->AddBoundaryPoint(1430.0f, 200.0f);
   pRightBoundary->AddBoundaryPoint(1430.0f, 880.0f);

   Boundary* pMiddleObstacle = Character::Resurrect<Boundary>();
   assert(pMiddleObstacle != 0);
   pMiddleObstacle->Init();
   pMiddleObstacle->mPolygon.SetOrigin(0.0f, 0.0f);
   pMiddleObstacle->AddBoundaryPoint(990.0f, 600.0f);
   pMiddleObstacle->AddBoundaryPoint(990.0f, 480.0f);
   pMiddleObstacle->AddBoundaryPoint(930.0f, 480.0f);
   pMiddleObstacle->AddBoundaryPoint(930.0f, 600.0f);

   Boundary* pTopLeftObstacle = Character::Resurrect<Boundary>();
   assert(pTopLeftObstacle != 0);
   pTopLeftObstacle->Init();
   pTopLeftObstacle->mPolygon.SetOrigin(0.0f, 0.0f);
   pTopLeftObstacle->AddBoundaryPoint(590.0f, 275.0f);
   pTopLeftObstacle->AddBoundaryPoint(590.0f, 450.0f);
   pTopLeftObstacle->AddBoundaryPoint(700.0f, 450.0f);
   pTopLeftObstacle->AddBoundaryPoint(700.0f, 275.0f);

   Boundary* pTopRightObstacle = Character::Resurrect<Boundary>();
   assert(pTopRightObstacle != 0);
   pTopRightObstacle->Init();
   pTopRightObstacle->mPolygon.SetOrigin(0.0f, 0.0f);
   pTopRightObstacle->AddBoundaryPoint(1330.0f, 275.0f);
   pTopRightObstacle->AddBoundaryPoint(1330.0f, 450.0f);
   pTopRightObstacle->AddBoundaryPoint(1220.0f, 450.0f);
   pTopRightObstacle->AddBoundaryPoint(1220.0f, 275.0f);

   Boundary* pBottomLeftObstacle = Character::Resurrect<Boundary>();
   assert(pBottomLeftObstacle != 0);
   pBottomLeftObstacle->Init();
   pBottomLeftObstacle->mPolygon.SetOrigin(0.0f, 0.0f);
   pBottomLeftObstacle->AddBoundaryPoint(590.0f, 880.0f - 75.0f);
   pBottomLeftObstacle->AddBoundaryPoint(590.0f, 880.0f - 250.0f);
   pBottomLeftObstacle->AddBoundaryPoint(700.0f, 880.0f - 250.0f);
   pBottomLeftObstacle->AddBoundaryPoint(700.0f, 880.0f - 75.0f);

   Boundary* pBottomRightObstacle = Character::Resurrect<Boundary>();
   assert(pBottomRightObstacle != 0);
   pBottomRightObstacle->Init();
   pBottomRightObstacle->mPolygon.SetOrigin(0.0f, 0.0f);
   pBottomRightObstacle->AddBoundaryPoint(1330.0f, 880.0f - 75.0f);
   pBottomRightObstacle->AddBoundaryPoint(1330.0f, 880.0f - 250.0f);
   pBottomRightObstacle->AddBoundaryPoint(1220.0f, 880.0f - 250.0f);
   pBottomRightObstacle->AddBoundaryPoint(1220.0f, 880.0f - 75.0f);


   pTopBoundary->mPolygon.AdjustPoints(pTopBoundary->mX, pTopBoundary->mY, pTopBoundary->mAngle, pTopBoundary->mScale);
   pLeftBoundary->mPolygon.AdjustPoints(pLeftBoundary->mX, pLeftBoundary->mY, pLeftBoundary->mAngle, pLeftBoundary->mScale);
   pBottomBoundary->mPolygon.AdjustPoints(pBottomBoundary->mX, pBottomBoundary->mY, pBottomBoundary->mAngle, pBottomBoundary->mScale);
   pRightBoundary->mPolygon.AdjustPoints(pRightBoundary->mX, pRightBoundary->mY, pRightBoundary->mAngle, pRightBoundary->mScale);
   pMiddleObstacle->mPolygon.AdjustPoints(pMiddleObstacle->mX, pMiddleObstacle->mY, pMiddleObstacle->mAngle, pMiddleObstacle->mScale);
   pTopLeftObstacle->mPolygon.AdjustPoints(pTopLeftObstacle->mX, pTopLeftObstacle->mY, pTopLeftObstacle->mAngle, pTopLeftObstacle->mScale);
   pTopRightObstacle->mPolygon.AdjustPoints(pTopRightObstacle->mX, pTopRightObstacle->mY, pTopRightObstacle->mAngle, pTopRightObstacle->mScale);
   pBottomLeftObstacle->mPolygon.AdjustPoints(pBottomLeftObstacle->mX, pBottomLeftObstacle->mY, pBottomLeftObstacle->mAngle, pBottomLeftObstacle->mScale);
   pBottomRightObstacle->mPolygon.AdjustPoints(pBottomRightObstacle->mX, pBottomRightObstacle->mY, pBottomRightObstacle->mAngle, pBottomRightObstacle->mScale);

   //pBoundary->DrawBoundary(mScreen);
   StartNewLevel();
}
void Game::StartNewLevel()
{
}

void Game::UpdatePlayerLives()
{
   PlayerLives* pPlayerLives = 0;
   do
   {
      pPlayerLives = Character::FindCharacter<PlayerLives>(Character::alive);
      if (Character::FindCharacter<PlayerLives>(Character::alive) != 0)
      {
         pPlayerLives->Kill();
      }

   }while(pPlayerLives != 0);

   for (int x = 0; x < mLives; x++)
   {
      pPlayerLives = Character::Resurrect<PlayerLives>();
      assert(pPlayerLives != 0);
      pPlayerLives->mX = 50.0f + x * 50.0f;
      pPlayerLives->mY = 1040.0f;
   }
}