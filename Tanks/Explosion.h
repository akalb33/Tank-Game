#pragma once

#include "Character.h"

class Explosion : public Character
{
public:
   Explosion();
   virtual ~Explosion();
   void Reset();
   virtual void Process(float deltaTime);
private:
};
