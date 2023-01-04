#pragma once

#include "Character.h"
#include "Polygon.h"

class Boundary : public Character
{
public:
   // Boundaries can have as many sides as you add points, but must remain convex for filling to work.
   Boundary();
   virtual ~Boundary();
   virtual void Process(float deltaTime);  
   virtual void Display();
   void Init(bool textureSolid = false);
   bool CalculateAngleOfClosestSide(float contactX, float contactY, float previousX, float previousY, float& angle);
   void AddBoundaryPoint(float x, float y);

private:
   std::vector<Polygon::Point> mPoints;
   sf::ConvexShape mShape;
   sf::Texture mBrick;
};
