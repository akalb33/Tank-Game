// CollisionLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Polygon.h"
#include "Util.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <assert.h>

Polygon::Polygon()
   : mOrigin(Point(0.0f, 0.0f))
   , mInitialVectors()
   , mAdjustedPoints()
   , mAdjustedMin(0.0f, 0.0f)
   , mAdjustedMax(0.0f, 0.0f)
   , mOriginSet(false)
{
}

void Polygon::SetOrigin(float x, float y)
{
   // Start over. Clear all points.
   mInitialVectors.clear();
   mAdjustedPoints.clear();

   mOrigin.mX = x;
   mOrigin.mY = y;
   mOriginSet = true;
}

void Polygon::AddPoint(float x, float y)
{
   assert(mOriginSet);

   x -= mOrigin.mX;
   y -= mOrigin.mY;
   float angle = 0.0f;

   if(x == 0.0f)
   {
      if(y > 0.0f)
         angle = 90.0f * Util::RAD_PER_DEG;
      else
         angle = -90.0f * Util::RAD_PER_DEG;
   }
   else
   {
      angle = atan(y / x);

      if(x < 0.0f)
         angle += 180.0f * Util::RAD_PER_DEG;
   }

   float degAngle = angle / Util::RAD_PER_DEG;

   float magnitude = sqrt(x * x + y * y);
   mInitialVectors.push_back(Vector(angle, magnitude));
   mAdjustedPoints.push_back(Point(0.0f, 0.0f));
}

void Polygon::AdjustPoints(float x, float y, float angle, float scale)
{
   angle *= Util::RAD_PER_DEG;

   for (unsigned int i = 0; i < mInitialVectors.size(); ++i)
   {
      float adjustedMagnitude = scale * mInitialVectors[i].mMagnitude;
      float adjustedAngle = angle + mInitialVectors[i].mAngle;
      float adjustedX = x + adjustedMagnitude * cos(adjustedAngle);
      float adjustedY = y + adjustedMagnitude * sin(adjustedAngle);

      mAdjustedPoints[i].mX = adjustedX;
      mAdjustedPoints[i].mY = adjustedY;

      if (i == 0)
      {
         mAdjustedMin.mX = adjustedX;
         mAdjustedMax.mX = adjustedX;
         mAdjustedMin.mY = adjustedY;
         mAdjustedMax.mY = adjustedY;
      }
      else
      {
         if (adjustedX < mAdjustedMin.mX)
            mAdjustedMin.mX = adjustedX;
         else if (adjustedX > mAdjustedMax.mX)
            mAdjustedMax.mX = adjustedX;

         if (adjustedY < mAdjustedMin.mY)
            mAdjustedMin.mY = adjustedY;
         else if (adjustedY > mAdjustedMax.mY)
            mAdjustedMax.mY = adjustedY;
      }
   }
}

bool Polygon::Collision(Polygon& otherPolygon)
{
   bool result = false;

   const std::vector<Point>& otherAdjustedPoints = otherPolygon.GetAdjustedPoints();

   // There need to be at least 4 points to have a collision. For example, one polygon
   //    is a single point and the other has 3 (triangle). Or each polygon has two points
   //    which means each polygon is a line. 
   if((mAdjustedPoints.size() + otherAdjustedPoints.size()) > 3)
   {

      float maxX1 = GetAdjustedMaxX();
      float minX2 = otherPolygon.GetAdjustedMinX();
      float minx1 = GetAdjustedMinX();
      float mmax2 = otherPolygon.GetAdjustedMaxX();
      float maxy1 = GetAdjustedMaxY();
      float miny2 = otherPolygon.GetAdjustedMinY();
      float miny1 = GetAdjustedMinY();
      float maxy2 = otherPolygon.GetAdjustedMaxY();

      // Do fast check to try to eliminate most intersections quickly.
      if(!((GetAdjustedMaxX() < otherPolygon.GetAdjustedMinX()) ||
           (GetAdjustedMinX() > otherPolygon.GetAdjustedMaxX()) ||
           (GetAdjustedMaxY() < otherPolygon.GetAdjustedMinY()) ||
           (GetAdjustedMinY() > otherPolygon.GetAdjustedMaxY())))
      {
         result = DoPolygonLinesIntersect(mAdjustedPoints, otherAdjustedPoints);

         if(!result)
         {
            result = IsAPointInsideAPolygon(otherAdjustedPoints[0].mX,
                                            otherAdjustedPoints[0].mY,
                                            mAdjustedPoints);
            if(!result)
            {
               result = IsAPointInsideAPolygon(mAdjustedPoints[0].mX,
                                               mAdjustedPoints[0].mY,
                                               otherAdjustedPoints);
            }
         }
      }
   }

   return result;
}

void Polygon::DisplayCollisionBoundary(Screen& screen)
{
   if (mAdjustedPoints.size() > 1)
   {
      sf::VertexArray lines(sf::LinesStrip, mAdjustedPoints.size() + 1);

      for (unsigned int i = 0; i < mAdjustedPoints.size(); ++i)
      {
         lines[i].position = sf::Vector2f(mAdjustedPoints[i].mX, mAdjustedPoints[i].mY);
         lines[i].color = sf::Color::Red;
      }

      lines[mAdjustedPoints.size()].position = sf::Vector2f(mAdjustedPoints[0].mX, mAdjustedPoints[0].mY);
      lines[mAdjustedPoints.size()].color = sf::Color::Red;

      screen.draw(lines);
   }
}


bool Polygon::DoPolygonLinesIntersect(const std::vector<Point>& adjustedPoints1,
                                      const std::vector<Point>& adjustedPoints2)
{
   // This algorithm will only be performed if there is at least one line in each polygon.
   if ((adjustedPoints1.size() > 1) && (adjustedPoints2.size() > 1))
   {
      float x1, y1, x2, y2, x3, y3, x4, y4;
      std::vector<Point>::const_iterator it1;
      std::vector<Point>::const_iterator it2;

      if(adjustedPoints1.size() == 2)
      {
         it1 = adjustedPoints1.begin();
         x2 = it1->mX;
         y2 = it1->mY;
         it1++;
      }
      else
      {
         it1 = adjustedPoints1.end() - 1;
         x2 = it1->mX;
         y2 = it1->mY;
         it1 = adjustedPoints1.begin();
      }

      do
      {
         x1 = x2;
         y1 = y2;
         x2 = it1->mX;
         y2 = it1->mY;

         if (adjustedPoints2.size() == 2)
         {
            it2 = adjustedPoints2.begin();
            x4 = it2->mX;
            y4 = it2->mY;
            it2++;
         }
         else
         {
            it2 = adjustedPoints2.end() - 1;
            x4 = it2->mX;
            y4 = it2->mY;
            it2 = adjustedPoints2.begin();
         }

         float run1 = x2 - x1;
         if (abs(run1) < 0.3f)
         {
            run1 = 0.3f;
            x2 = x1 + 0.3f;
         }

         float m1 = (y2 - y1) / run1;
         float b1 = y1 - m1 * x1;

         do
         {
            x3 = x4;
            y3 = y4;
            x4 = it2->mX;
            y4 = it2->mY;

            float run2 = x4 - x3;
            if (abs(run2) < 0.3f)
            {
               run2 = 0.3f;
               x4 = x3 + 0.3f;
            }

            float m2 = (y4 - y3) / run2;

            // If the slopes are the same, then the lines cannot intercept. This
            //    also eliminates a divide by zero below.
            if (m1 != m2)
            {
               // Assume that each line segment is a full line. Where would intercept?
               // Equation:  xIntercept = (b2 - b1) / (m1 - m2)  where b = y - m * x
               float xIntercept = (y3 - m2 * x3 - b1) / (m1 - m2);

               if(IsBetween(xIntercept, x1, x2) && IsBetween(xIntercept, x3, x4))
                  return true;
            }

            it2++;
         } while(it2 != adjustedPoints2.end());

         it1++;
      } while(it1 != adjustedPoints1.end());
   }

   return false;
}

bool Polygon::IsAPointInsideAPolygon(float x,
                                     float y,
                                     const std::vector<Point>& adjustedPoints)
{

   bool result = false;

   // The point can only be in the polygon if the polygon has 3 or more sides.
   if(adjustedPoints.size() > 2)
   {
      // Assume a vector that starts at (x, y) and that points straight up
      //    toward point toward y = -INF.  Count the number of times that
      //    this vector crosses each line of the polygon. If an odd number
      //    of times, the point must exist inside the polygon. 
      float x1, x2, y1, y2;
      std::vector<Point>::const_iterator it = adjustedPoints.end() - 1;
      x2 = it->mX;
      y2 = it->mY;
      it = adjustedPoints.begin();

      do
      {
         x1 = x2;
         y1 = y2;
         x2 = it->mX;
         y2 = it->mY;

         if(IsBetween(x, x1, x2))
         {
            // To be in between we know x1 != x2.
            float slope = (y2 - y1) / (x2 - x1);
            float yIntercept = y1 + slope * (x - x1);

            if(y > yIntercept)
               result = !result;
         }

         it++;
      } while (it != adjustedPoints.end());
   }

   return result;
}
