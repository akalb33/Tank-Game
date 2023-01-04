
#pragma once
#include "Screen.h"
#include <vector>
#include <cmath>

class Polygon
{
public:
   Polygon();

   // Set original of the polygon. The polygon will move, rotate, and scale 
   //    relative to this point.
   // NOTE: Calling this, resets all previous AddPoint() calls.
   void SetOrigin(float x, float y);

   float GetOriginX() const { return mOrigin.mX; }
   float GetOriginY() const { return mOrigin.mY; }

   void AddPoint(float x, float y);

   // Call prior to Collision(), GetAdjustedMinX(), GetAdjustedMinY(),
   //    GetAdjustedMaxX(), or GetAdjustedMaxY() unless the polygon has
   //    not moved, changed size, or rotated since the last call.
   // x, y, angle, and float are all relative to the original that was set.
   void AdjustPoints(float x, float y, float angle, float scale);

   // Do not call these until AdjustPoints() is called first unless the 
   //    polygon has not moved, changed size, or rotated since the last call.
   float GetAdjustedMinX() const { return mAdjustedMin.mX; }
   float GetAdjustedMinY() const { return mAdjustedMin.mY; }
   float GetAdjustedMaxX() const { return mAdjustedMax.mX; }
   float GetAdjustedMaxY() const { return mAdjustedMax.mY; }

   // Do not call until AdjustPoints() is called first (and on the other
   //    polygon) unless the polygon has not moved, changed size, or 
   //    rotated since the last call.
   bool Collision(Polygon& otherPolygon);

   void DisplayCollisionBoundary(Screen& screen);

   struct Point
   {
      Point(float x, float y)
         : mX(x)
         , mY(y)
      {
      }
      float mX;
      float mY;
   };

   static bool DoPolygonLinesIntersect(const std::vector<Point>& adjustedPoints1,
                                       const std::vector<Point>& adjustedPoints2);
   static bool IsAPointInsideAPolygon(float x,
                                      float y,
                                      const std::vector<Point>& adjustedPoints);

private:

   struct Vector
   {
      Vector(float angle, float magnitude)
         : mAngle(angle)
         , mMagnitude(magnitude)
      {
      }
      float mAngle;
      float mMagnitude;
   };

   const std::vector<Point>& GetAdjustedPoints() const { return mAdjustedPoints; }

   static bool IsBetween(float value, float x1, float x2)
   {
      if(value < x1)
      {
         if(value > x2)
            return true;
      }
      else
      {
         if (value < x2)
            return true;
      }
      return false;
   }

private:
   Point mOrigin;
   std::vector<Vector> mInitialVectors;
   std::vector<Point> mAdjustedPoints;
   Point mAdjustedMin;
   Point mAdjustedMax;
   bool mOriginSet;
};

