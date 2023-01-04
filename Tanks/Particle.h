#pragma once

#include "Character.h"

class Particle : public Character
{
public:
   Particle();
   virtual ~Particle();
   virtual void Process(float deltaTime);
   static void GenerateParticles(int number, float x, float y);
private:
   void Initialize(float x, float y);

   float mDuration;
};
