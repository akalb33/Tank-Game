#pragma once
#include <random>
#include <cmath>
#include <time.h>
#include <stdlib.h>

class Util
{
public:
   static void InitRandomizer()
   {
      srand(static_cast<unsigned int>(time(0)));
   }

   static int RandomInteger(int min, int max)
   {
      return min + rand() % (max - min + 1); 
   }

   static float RandomFloat(float min, float max)
   {
      return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
   }

   static void VectorToRectangle(float magnitude, float angle, float& x, float& y)
   {
      angle *= RAD_PER_DEG;
      x = magnitude * cos(angle);
      y = magnitude * sin(angle);
   }

   static void RectangleToVector(float x, float y, float& magnitude, float& angle)
   {
      angle = atan2f(y, x) / RAD_PER_DEG;
      magnitude = sqrtf(x * x + y * y);
   }

   static float GetAngleFromRectangle(float x, float y)
   {
      return atan2f(y, x) / RAD_PER_DEG;
   }
   
   static const float RAD_PER_DEG;   
};
