#pragma once

#include "Screen.h"
#include <string>
#include <vector>
#include <fstream>

class ScoreManager
{
public:
   ScoreManager();


   // Tells ScoreManager to use the specified text which includes font, color, bold, italics, etc.
   // Font height has to be entered in world coordinate size here.
   static void AddPlayer(float rightAlignedX, float y, const sf::Text& text, float fontHeight);


   static void SelectPlayer(unsigned int number);

   static void SetHighScorePosition(float rightAlignedX, float y, const sf::Text& text, float fontHeight);

   // Only call these on the top level menu, to save/load data for all scores.
   //    Call Load() after you have added all the menu settings and options, and
   //    call Save() prior to exiting the game. This will automatically generate
   //    a text file named "Settings", if one does not already exist.
   static void Load();
   static void Save();

   static void AddPoints(int points);

   static void ResetScore();


   static void DrawScores(Screen& screen);

   struct Score
   {
      Score()
      {
         sf::Text text;
         mText = text;
         mScore = 0;
         mRightAlignedX = 0.0f;
      }
      sf::Text mText;
      unsigned int mScore;
      float mRightAlignedX;
   };

private:

   static void UpdateTextAndRightAlign(Score& score);

   static Score mHighScore;

   static bool mHighScoreInUse;

   static Score* mpSelectedScore;


   static std::vector<Score> mScores;

};
