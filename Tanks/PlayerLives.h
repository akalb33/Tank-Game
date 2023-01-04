#pragma once

#include "Character.h"

class PlayerLives : public Character
{
public:
   PlayerLives();
   virtual ~PlayerLives();
   virtual void Process(float deltaTime);
public:

private:
};
