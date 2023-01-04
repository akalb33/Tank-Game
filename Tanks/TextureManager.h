#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <vector>
#include "Polygon.h"

class TextureManager
{
public:
   enum Textures
   {
      player1,
      player2,
      playerBullet,
      background,
      particle,
      explosion,
   };

   TextureManager()
   {
      Init();

      AddTexture(player1, "Dark Tank.png");
      AddPoint(player1, 7.0f, 1.0f);
      AddPoint(player1, 413.0f, 1.0f);
      AddPoint(player1, 379.0f, 46.0f);
      AddPoint(player1, 464.0f, 91.0f);
      AddPoint(player1, 464.0f, 99.0f);
      AddPoint(player1, 378.0f, 100.0f);
      AddPoint(player1, 379.0f, 135.0f);
      AddPoint(player1, 415.0f, 135.0f);
      AddPoint(player1, 413.0f, 181.0f);
      AddPoint(player1, 8.0f, 181.0f);

      AddTexture(player2, "Light Tank.png");
      AddPoint(player2, 1.0f, 1.0f);
      AddPoint(player2, 278.0f, 1.0f);
      AddPoint(player2, 278.0f, 25.0f);
      AddPoint(player2, 263.0f, 27.0f);
      AddPoint(player2, 262.0f, 69.0f);
      AddPoint(player2, 296.0f, 70.0f);
      AddPoint(player2, 296.0f, 75.0f);
      AddPoint(player2, 262.0f, 76.0f);
      AddPoint(player2, 262.0f, 109.0f);
      AddPoint(player2, 278.0f, 110.0f);
      AddPoint(player2, 278.0f, 137.0f);
      AddPoint(player2, 1.0f, 137.0f);

      AddTexture(playerBullet, "bullet.png");
      AddPoint(playerBullet, 125.0f, 118.0f);

      AddTexture(background, "background.png");
      AddTexture(particle, "Particle.png");
      AddTexture(explosion, "explosion.png");

   }

   // This automatically sets default origin and dimensions based on texture
   //    size. Call SetOrigin() and AddPoint() to overwrite these values.
   void AddTexture(Textures textureId, std::string fileName);

   const sf::Texture& GetTexture(Textures textureId) const;

   // This will set the origin of the texture. Rotation, movement and scaling
   //    are relative to the origin. Default dimensions are automatically set
   //    based on the size of the texture. Call AddPoint() to overwrite these.
   void SetOrigin(Textures textureId, float x, float y);

   // The first time this is called, the dimensions will be a point.
   // The second time this is called, the dimensions will be a line.
   // Three or more calls will define a polygon with an automatic line drawn
   //    from the last point added to the first point that was added.
   void AddPoint(Textures textureId, float x, float y);

   const Polygon& GetPolygon(Textures textureId);

private:
   enum State
   {
      noTexture,
      textureWithDefaultDimensions,
      originSetDefaultDimensions,
      pointsAdded
   };

   void Init();
   void SetDefaultDimensions(Textures textureId);

   static const unsigned int MAX_TEXTURES = 100;
   sf::Texture mTexture[MAX_TEXTURES];
   Polygon mPolygon[MAX_TEXTURES];
   State mState[MAX_TEXTURES];
};
