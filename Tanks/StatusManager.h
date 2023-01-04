#pragma once

#include <typeinfo>
#include <vector>

// User should never have to change this class.
class StatusManager 
{
public:
   enum Status
   {
      alive,
      dying,
      dead
   };

   StatusManager(const std::type_info& type);

   Status GetStatus() const { return mStatus; }

   void SetStatus(Status status);
   static unsigned int GetAliveCount(const std::type_info& type);
   static unsigned int GetDyingCount(const std::type_info& type);
   static unsigned int GetDeadCount(const std::type_info& type);


private:
   Status mStatus;
   const std::type_info& mType;

   struct Totals
   {
      Totals(const std::type_info& type)
         : mType(type)
         , mDead(1)
         , mDying(0)
         , mAlive(0)
      {
      }

      bool operator==(const Totals& rhs)
      {
         return (this->mType == rhs.mType);
      }

      const std::type_info& mType;
      unsigned int mDead;
      unsigned int mDying;
      unsigned int mAlive;
   };

   static std::vector<Totals> mTotals;
};
