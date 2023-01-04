#include "stdafx.h"
#include "Util.h"
#include "Background.h"
#include "PlayerBullet.h"
#include "Character.h"

Background::Background()
   : Character(typeid(Background))
{
   SetTexture(TextureManager::background);
   SetDisplayOrder(1);
}

Background::~Background()
{
}

void Background::Process(float deltaTime)
      {
  
}