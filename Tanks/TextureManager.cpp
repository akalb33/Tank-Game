#include "stdafx.h"
#include "TextureManager.h"
#include <iostream>
#include <assert.h>


void TextureManager::Init()
{
   for(unsigned int i = 0; i < MAX_TEXTURES; ++i)
      mState[i] = noTexture;
}

void TextureManager::AddTexture(Textures textureId, std::string fileName)
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);
   assert(mState[textureId] == noTexture);
   if(!mTexture[textureId].loadFromFile(fileName))
      assert(false);

   // Set a default origin and dimensions based on the size of the texture.
   sf::Vector2u size = mTexture[textureId].getSize();
   float x = static_cast<float>(size.x) - 1.0f;
   float y = static_cast<float>(size.y) - 1.0f;
   mPolygon[textureId].SetOrigin(x / 2.0f, y / 2.0f);

   SetDefaultDimensions(textureId);

   mState[textureId] = textureWithDefaultDimensions;
}

const sf::Texture& TextureManager::GetTexture(Textures textureId) const
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);
   assert(mState[textureId] != noTexture);
   return mTexture[textureId];
}

void TextureManager::SetOrigin(Textures textureId, float x, float y)
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);
   assert(mState[textureId] == textureWithDefaultDimensions);

   mPolygon[textureId].SetOrigin(x, y);
   SetDefaultDimensions(textureId);

   mState[textureId] = textureWithDefaultDimensions;
}

void TextureManager::AddPoint(Textures textureId, float x, float y)
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);
   assert(mState[textureId] != noTexture);

   if(mState[textureId] != pointsAdded)
   {
      // Since this is first call to AddPoint(), call SetOrigin() which
      //    will get rid of the default points.
      mPolygon[textureId].SetOrigin(mPolygon[textureId].GetOriginX(), 
                                    mPolygon[textureId].GetOriginY());
   }

   mPolygon[textureId].AddPoint(x, y);
   mState[textureId] = pointsAdded;
}

const Polygon& TextureManager::GetPolygon(Textures textureId)
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);
   return mPolygon[textureId];
}

void TextureManager::SetDefaultDimensions(Textures textureId)
{
   assert(static_cast<unsigned int>(textureId) < MAX_TEXTURES);

   // Set dimensions based on the size of the texture.
   sf::Vector2u size = mTexture[textureId].getSize();
   float x = static_cast<float>(size.x) - 1.0f;
   float y = static_cast<float>(size.y) - 1.0f;
   mPolygon[textureId].AddPoint(0.0f, 0.0f);
   mPolygon[textureId].AddPoint(x, 0.0f);
   mPolygon[textureId].AddPoint(x, y);
   mPolygon[textureId].AddPoint(0.0f, y);
}
