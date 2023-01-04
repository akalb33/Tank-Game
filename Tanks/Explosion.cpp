#include "stdafx.h"
#include "Util.h"
#include "Explosion.h"
#include "PlayerBullet.h"
#include "Character.h"

Explosion::Explosion()
   : Character(typeid(Explosion))
{
   SetTexture(TextureManager::explosion);
   SetDisplayOrder(200);
   mScale = 0.1f;
}

void Explosion::Reset()
{
   mScale = 0.1f;
}

Explosion::~Explosion()
{
}

void Explosion::Process(float deltaTime)
      {
  
}