#include "stdafx.h"
#include <assert.h>
#include "StatusManager.h"

// User should never have to change this class.

std::vector<StatusManager::Totals> StatusManager::mTotals;


StatusManager::StatusManager(const std::type_info& type)
   : mStatus(dead)
   , mType(type)
{

   std::vector<Totals>::iterator it = find(mTotals.begin(), mTotals.end(), type);
   if (it == mTotals.end())
   {
      mTotals.push_back(Totals(type));
   }
   else
   {
      it->mDead++;
   }
}


void StatusManager::SetStatus(Status status)
{
   std::vector<Totals>::iterator it = find(mTotals.begin(), mTotals.end(), mType);
   assert(it != mTotals.end());
   if (status == alive)
   {
      it->mAlive++;
   }
   else if (status == dead)
   {
      it->mDead++;
   }
   else
   {
      it->mDying++;
   }

   if (mStatus == alive)
   {
      it->mAlive--;
   }
   else if (mStatus == dead)
   {
      it->mDead--;
   }
   else
   {
      it->mDying--;
   }
   mStatus = status;
}

unsigned int StatusManager::GetAliveCount(const std::type_info& type)
{
   std::vector<Totals>::iterator it = find(mTotals.begin(), mTotals.end(), type);
   assert(it != mTotals.end());
   return it->mAlive;
}

unsigned int StatusManager::GetDyingCount(const std::type_info& type)
{
   std::vector<Totals>::iterator it = find(mTotals.begin(), mTotals.end(), type);
   assert(it != mTotals.end());
   return it->mDying;
}

unsigned int StatusManager::GetDeadCount(const std::type_info& type)
{
   std::vector<Totals>::iterator it = find(mTotals.begin(), mTotals.end(), type);
   assert(it != mTotals.end());
   return it->mDead;
}

