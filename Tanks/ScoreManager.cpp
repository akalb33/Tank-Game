#include "stdafx.h"
#include "ScoreManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <assert.h>


ScoreManager::Score ScoreManager::mHighScore;
ScoreManager::Score* ScoreManager::mpSelectedScore = 0;
std::vector<ScoreManager::Score> ScoreManager::mScores;
bool ScoreManager::mHighScoreInUse = false;

//*****************************************************************************
ScoreManager::ScoreManager()
{
   assert(false);
}


void ScoreManager::AddPlayer(float rightAlignedX, float y, const sf::Text& text, float fontHeight)
{
   Score score;
   score.mText = text;
   score.mText.setPosition(rightAlignedX, y);
   score.mRightAlignedX = rightAlignedX;
   score.mText.setCharacterSize(static_cast<unsigned int> (fontHeight + 0.5f));
   UpdateTextAndRightAlign(score);
   mScores.push_back(score);
}

void ScoreManager::SelectPlayer(unsigned int number)
{
   assert(number < mScores.size());
   mpSelectedScore = &mScores[number];
}

void ScoreManager::SetHighScorePosition(float rightAlignedX, float y, const sf::Text& text, float fontHeight)
{
   mHighScoreInUse = true;

   mHighScore.mText = text;
   mHighScore.mText.setPosition(rightAlignedX, y);
   mHighScore.mRightAlignedX = rightAlignedX;
   mHighScore.mText.setCharacterSize(static_cast<unsigned int> (fontHeight + 0.5f));
   UpdateTextAndRightAlign(mHighScore);
}

void ScoreManager::AddPoints(int points)
{
   assert(mpSelectedScore != 0);
   mpSelectedScore->mScore += points;
   UpdateTextAndRightAlign(*mpSelectedScore);

   if (mpSelectedScore->mScore > mHighScore.mScore)
   {
      mHighScore.mScore = mpSelectedScore->mScore;
      UpdateTextAndRightAlign(mHighScore);
   }
}

void ScoreManager::ResetScore()
{
   mpSelectedScore->mScore = 0;
   mpSelectedScore->mText.setString(std::to_string(mpSelectedScore->mScore));
}

void ScoreManager::DrawScores(Screen& screen)
{
   for (std::vector<Score>::iterator it = mScores.begin(); it != mScores.end(); it++)
   {
      screen.draw(it->mText);
   }
   if (mHighScoreInUse)
   {
      screen.draw(mHighScore.mText);
   }
}

void ScoreManager::UpdateTextAndRightAlign(Score& score)
{
   if (score.mScore == 0)
      score.mText.setString("0");
   else
      score.mText.setString(std::to_string(score.mScore));

   score.mText.setPosition(score.mRightAlignedX - score.mText.getGlobalBounds().width,
                           score.mText.getPosition().y);
}

//*****************************************************************************
void ScoreManager::Load()
{
   std::ifstream file;
   file.open("HighScore");

   if(!file.fail())
      file >> mHighScore.mScore;

   file.close();
}

//*****************************************************************************
void ScoreManager::Save()
{
   std::ofstream file;
   file.open("HighScore");
   file << mHighScore.mScore;
   file.close();
}



