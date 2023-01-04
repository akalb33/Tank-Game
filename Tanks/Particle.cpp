#include "stdafx.h"
#include "Util.h"
#include "Particle.h"
#include "Character.h"

Particle::Particle()
   : Character(typeid(Particle))
   , mDuration(0.0f)
{
   SetTexture(TextureManager::particle);
   SetDisplayOrder(140);
   mScale = 0.5f;
}

Particle::~Particle()
{
}

void Particle::Process(float deltaTime)
{
   ApplyInertia(deltaTime);
   mDuration -= deltaTime;
   if (mDuration <= 0.0f)
   {
      Kill();
   }
}

void Particle::GenerateParticles(int number, float x, float y)
{
   for (int i = 0; i < number; i++)
   {
      Particle* pParticle = Resurrect<Particle>();
      if (pParticle != 0)
      {
         pParticle->Initialize(x, y);
      }
   }
}


void Particle::Initialize(float x, float y)
{
   mX = x;
   mY = y;
   mVelocityX = Util::RandomFloat(-1000.0f, 1000.0f);
   mVelocityY = Util::RandomFloat(-1000.0f, 1000.0f);
   mDuration = Util::RandomFloat(0.2f, 0.5f);
}

