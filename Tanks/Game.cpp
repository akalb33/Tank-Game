#include "stdafx.h"
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "PlayerLives.h"
#include "PlayerBullet.h"
#include "Explosion.h"
#include "Particle.h"
#include "ScoreManager.h"
#include "Background.h"
#include "Boundary.h"
#include "Util.h"
#include <assert.h>
#include <SFML/System/Time.hpp>
#include <random>

Game::Game()
   : mScreen(0.0f, 0.0f, 1920.0f, 1080.0f, false, "Tanks")
   , mTextureManager()
   , mSoundManager()
   , mMenu()
   , mpOurTank(0)
   , mpTheirTank(0)
   , mClient(false)
   , mMessage()
   , mSocket()
   , mSendingMessage(true)
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
                  sf::IpAddress lanAddress = sf::IpAddress::getLocalAddress();
                  std::cout << "Server local ip is " << lanAddress << "\n";
                  pServerListener = new sf::TcpListener();
                  if (pServerListener->listen(53001) != sf::Socket::Done)
                  {
                     std::cout << "Failed to listen.";
                  }

                  std::cout << "Now listening.\n";

                  // accept a new connection
                  if (pServerListener->accept(mSocket) != sf::Socket::Done)
                  {
                     std::cout << "Timed out. Couldn't find a client.";
                     assert(false);
                  }
                  std::cout << "Success! Client with remote IP \"" << mSocket.getRemoteAddress() << "\" just connected.";
                  mSocket.setBlocking(false);
                  mSendingMessage = false;
                  mClient = false;
                  StartNewGame();
               }
               else if (selection == "Join a Game")
               {
                  //sf::IpAddress ipAddress = sf::IpAddress::getLocalAddress();
                  //std::cout << "Attempting to connect to local ip of 192.168.0.184.\n";
                  std::cout << "Attempting to connect to local ip of 192.168.0.168.\n";
                  sf::Socket::Status status = mSocket.connect("192.168.0.168", 53001);
                  //sf::Socket::Status status = socket.connect("192.168.0.184", 53001);
                  if (status != sf::Socket::Done)
                  {
                     std::cout << "Timed out. Couldn't find a server.";
                     assert(false);
                  }
                  std::cout << "Success! Connected to server with remote IP \"" << mSocket.getRemoteAddress() << "\".\n";
                  mSocket.setBlocking(false);
                  mSendingMessage = true;
                  mClient = true;
                  StartNewGame();
               }

               goToMenu = false;

               clock.restart();
            }
         }
      }

      mScreen.clear();
      float deltaTime = clock.restart().asSeconds();
      if (deltaTime > 0.025f)
      {
         deltaTime = 0.025f;
      }
      Character::ProcessAll(deltaTime);
      Character::DisplayAll();
      ScoreManager::DrawScores(mScreen);

      HandleCommunications();                                                        

      mScreen.DisplayNewScreen();
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
   Character::SetMaxLimit<PlayerBullet>(PlayerBullet::mMaxBulletCount * 2);
   Character::SetMaxLimit<PlayerLives>(4);
   Character::SetMaxLimit<Background>(1);
   Character::SetMaxLimit<Particle>(500);
   Character::SetMaxLimit<Boundary>(100);
   Character::SetMaxLimit<Explosion>(2);
}

void Game::UpdateMenuOptions()
{
}

void Game::HandleCommunications()
{
   if (mSendingMessage)
   {
      //mMessage.informationMessage.tankInfo.status = mpOurTank->GetStatus();
      mMessage.informationMessage.tankInfo.x = mpOurTank->mX;
      mMessage.informationMessage.tankInfo.y = mpOurTank->mY;
      mMessage.informationMessage.tankInfo.angle = mpOurTank->mAngle;
      mMessage.informationMessage.tankInfo.angularVelocity = mpOurTank->mAngularVelocity;
      mMessage.informationMessage.tankInfo.xVelocity = mpOurTank->mVelocityX;
      mMessage.informationMessage.tankInfo.yVelocity = mpOurTank->mVelocityY;
      mMessage.informationMessage.tankInfo.yourTankHit = mpTheirTank->mYourTankHit;
      mMessage.informationMessage.tankInfo.myTankHit = mpOurTank->mMyTankHit; 
      mMessage.informationMessage.tankInfo.invincible = mpOurTank->IsTankInvincible();
      unsigned int count = 0;
      std::vector<PlayerBullet*> bullets;
      Character::FindAllCharacters<PlayerBullet>(Character::Status::alive, bullets);
      for (std::vector<PlayerBullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
      {
         if ((*it)->IsOurBullet())
         {
            mMessage.informationMessage.bulletInfo[count].x = (*it)->mX;
            mMessage.informationMessage.bulletInfo[count].y = (*it)->mY;
            mMessage.informationMessage.bulletInfo[count].angle = (*it)->mAngle;
            mMessage.informationMessage.bulletInfo[count].sequenceId = (*it)->GetSequenceId();
            count++;
         }
      }

      mMessage.informationMessage.numberOfBullets = count;

      sf::Socket::Status status = mSocket.send(mMessage.c, sizeof(mMessage.informationMessage));
      assert(status == sf::Socket::Status::Done);
      mpTheirTank->mYourTankHit = false;
      mpOurTank->mMyTankHit = false;
      mSendingMessage = false;
   }
   else
   {
      size_t received = 0;
      if (mSocket.receive(mMessage.c, sizeof(Message), received) == sf::Socket::Status::Done)
      {
         //assert(received == sizeof(TankInfo) + sizeof(unsigned int) + sizeof(BulletInfo) 
                //* mMessage.informationMessage.numberOfBullets);
         float speedMultiplier = 1.2f;
         //mpTheirTank->SetStatus(mMessage.informationMessage.tankInfo.status);
         mpTheirTank->mMsgX = mMessage.informationMessage.tankInfo.x;
         mpTheirTank->mMsgY = mMessage.informationMessage.tankInfo.y;
         mpTheirTank->mMsgAngle = mMessage.informationMessage.tankInfo.angle;
         mpTheirTank->mVelocityX = mMessage.informationMessage.tankInfo.xVelocity * speedMultiplier;
         mpTheirTank->mAngularVelocity = mMessage.informationMessage.tankInfo.angularVelocity * speedMultiplier;
         mpTheirTank->mVelocityY = mMessage.informationMessage.tankInfo.yVelocity * speedMultiplier;       
         mpTheirTank->SetTankInvincible(mMessage.informationMessage.tankInfo.invincible);
         if (mMessage.informationMessage.tankInfo.yourTankHit)
         {
            mpOurTank->Kill();
         }
         if (mMessage.informationMessage.tankInfo.myTankHit)
         {
            mpTheirTank->SetTankInvincible(false);
            mpTheirTank->Kill();
         }

         std::vector<PlayerBullet*> bullets;
         Character::FindAllCharacters<PlayerBullet>(Character::Status::alive, bullets);

         // if you have bullets that that are theirs but does not match any message seqIDs, then Kill
         for (std::vector<PlayerBullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
         {
            if (!(*it)->IsOurBullet())
            {
               bool found = false;
               for (unsigned int i = 0; i < mMessage.informationMessage.numberOfBullets && !found; i++)
               {
                  if (mMessage.informationMessage.bulletInfo[i].sequenceId == (*it)->GetSequenceId())
                  {
                     found = true;
                  }
               }
               if (!found)
               {
                  (*it)->Kill();
               }
            }
         }

         for (unsigned int i = 0; i < mMessage.informationMessage.numberOfBullets; i++)
         {
            PlayerBullet* pBullet = 0;
            for (std::vector<PlayerBullet*>::iterator it = bullets.begin(); (it != bullets.end()) && (pBullet == 0); it++)
            {
               if (!(*it)->IsOurBullet())
               {
                  if (mMessage.informationMessage.bulletInfo[i].sequenceId == (*it)->GetSequenceId())
                  {
                     pBullet = *it;
                  }
               }
            }
            if (pBullet == 0)
            {
               pBullet = Character::Resurrect<PlayerBullet>();
               assert(pBullet != 0);
               pBullet->SetOurBullet(false);
               pBullet->mX = mMessage.informationMessage.bulletInfo[i].x;
               pBullet->mY = mMessage.informationMessage.bulletInfo[i].y;
               pBullet->SetSequenceId(mMessage.informationMessage.bulletInfo[i].sequenceId);
            }
            pBullet->mMsgX = mMessage.informationMessage.bulletInfo[i].x;
            pBullet->mMsgY = mMessage.informationMessage.bulletInfo[i].y;
            pBullet->mAngle = mMessage.informationMessage.bulletInfo[i].angle;
         }
         // if none of the alive bullets have the seqId then cressurect one, and call SetSequenceId, set x, y, and angle
         // else if one of the alive bullets has that seqId, set currentX, currentY, and angle

         mSendingMessage = true;
      }
   }
}

void Game::StartNewGame()
{
   Character::KillAll();
   Background* pBackground = Character::Resurrect<Background>();
   assert(pBackground != 0);
   pBackground->mX = 960.0f;
   pBackground->mY = 540.0f;

   Player* pPlayer = Character::Resurrect<Player>();
   assert(pPlayer != 0);
   pPlayer->SetTankAsOurOwn(true);
   mpOurTank = pPlayer;
   pPlayer->Reset();
   if (!mClient)
   {
      pPlayer->mX = 1200.0f;
      pPlayer->mY = 800.0f;
   }
   else
   {
      pPlayer->mX = 1200.0f;
      pPlayer->mY = 500.0f;
   }
   
   pPlayer = Character::Resurrect<Player>();
   assert(pPlayer != 0);
   pPlayer->SetTankAsOurOwn(false);
   mpTheirTank = pPlayer;
   pPlayer->Reset();
   if (mClient)
   {
      pPlayer->mX = 1200.0f;
      pPlayer->mY = 800.0f;
      mpOurTank->SetTexture(TextureManager::player1);
      mpTheirTank->SetTexture(TextureManager::player2);
      mpOurTank->CalculateBarrelTipDisplacementFromCenterOfSprite(296.0f, 0.0f);
      mpTheirTank->CalculateBarrelTipDisplacementFromCenterOfSprite(464.0f, 0.0f);
   }
   else
   {
      pPlayer->mX = 1200.0f;
      pPlayer->mY = 500.0f;
      mpOurTank->SetTexture(TextureManager::player2);
      mpTheirTank->SetTexture(TextureManager::player1);
      mpOurTank->CalculateBarrelTipDisplacementFromCenterOfSprite(464.0f, 0.0f);
      mpTheirTank->CalculateBarrelTipDisplacementFromCenterOfSprite(296.0f, 0.0f);
   }


   Boundary* pTopBoundary = Character::Resurrect<Boundary>();
   assert(pTopBoundary != 0);
   pTopBoundary->Init(true);
   pTopBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pTopBoundary->AddBoundaryPoint(500.0f, 200.0f);
   pTopBoundary->AddBoundaryPoint(1420.0f, 200.0f);
   pTopBoundary->AddBoundaryPoint(1420.0f, 190.0f);
   pTopBoundary->AddBoundaryPoint(500.0f, 190.0f);

   Boundary* pLeftBoundary = Character::Resurrect<Boundary>();
   assert(pLeftBoundary != 0);
   pLeftBoundary->Init(true);
   pLeftBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pLeftBoundary->AddBoundaryPoint(500.0f, 200.0f);
   pLeftBoundary->AddBoundaryPoint(500.0f, 880.0f);
   pLeftBoundary->AddBoundaryPoint(490.0f, 880.0f);
   pLeftBoundary->AddBoundaryPoint(490.0f, 200.0f);

   Boundary* pBottomBoundary = Character::Resurrect<Boundary>();
   assert(pBottomBoundary != 0);
   pBottomBoundary->Init(true);
   pBottomBoundary->mPolygon.SetOrigin(0.0f, 0.0f);
   pBottomBoundary->AddBoundaryPoint(500.0f, 880.0f);
   pBottomBoundary->AddBoundaryPoint(1420.0f, 880.0f);
   pBottomBoundary->AddBoundaryPoint(1420.0f, 890.0f);
   pBottomBoundary->AddBoundaryPoint(500.0f, 890.0f);

   Boundary* pRightBoundary = Character::Resurrect<Boundary>();
   assert(pRightBoundary != 0);
   pRightBoundary->Init(true);
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
}
