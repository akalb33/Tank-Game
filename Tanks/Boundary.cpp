#include "stdafx.h"
#include <assert.h>
#include "Util.h"
#include "Boundary.h"
#include "Character.h"
#include "Polygon.h"
#include <cmath>

Boundary::Boundary()
   : Character(typeid(Boundary))
{
   mBrick.loadFromFile("brick.png");
   mShape.setTexture(&mBrick);
   SetDisplayOrder(8);
}

Boundary::~Boundary()
{
}

void Boundary::Process(float deltaTime)
{
}

void Boundary::Display()
{
   mpScreen->draw(mShape);
}

void Boundary::Init(bool textureSolid)
{
   if (textureSolid)
   {
      mShape.setTexture(0);
      mShape.setFillColor(sf::Color::Black);
   }
   else
   {
      mBrick.loadFromFile("brick.png");
      mShape.setTexture(&mBrick);
   }
   mPoints.clear();
}

bool Boundary::CalculateAngleOfClosestSide(float contactX, float contactY, float previousX, float previousY, float& contactAngle)
{
   std::vector<Polygon::Point> points;
   points.push_back(Polygon::Point(contactX, contactY));
   points.push_back(Polygon::Point(previousX, previousY));

   bool done = false; 
   for (std::vector<Polygon::Point>::iterator it = mPoints.begin(); !done && it != mPoints.end(); it++)
   {
      std::vector<Polygon::Point> boundaryLine;
      boundaryLine.push_back(*it);
      if (it == mPoints.end() - 1)
      {
         boundaryLine.push_back(*(mPoints.begin()));
      }   
      else
      {
         boundaryLine.push_back(*(it + 1));
      }

      if (Polygon::DoPolygonLinesIntersect(points, boundaryLine))
      {
         Polygon::Point first = *it;
         Polygon::Point second = *(mPoints.begin());
         if (it != mPoints.end() - 1)
         {
            second = *(it + 1);
         }
         contactAngle = Util::GetAngleFromRectangle(first.mX - second.mX, first.mY - second.mY);
         done = true;
      }
   }
   return done;
}

void Boundary::AddBoundaryPoint(float x, float y)
{
   mPolygon.AddPoint(x, y);
   mPoints.push_back(Polygon::Point(x, y));
   mShape.setPointCount(mShape.getPointCount() + 1);
   mShape.setPoint(mShape.getPointCount() - 1, sf::Vector2f(x, y));
}

