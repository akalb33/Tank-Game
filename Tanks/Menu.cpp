#include "stdafx.h"
#include "Menu.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <assert.h>

IMenuCallBack* Menu::mpMenuCallBack = 0;

//*****************************************************************************
Menu::Menu()
   : mName("Menu")
   , mText()
   , mSettings()
   , mSettingIndex(0)
   , mChangingOption(false)
{
}

//*****************************************************************************
void Menu::AddSetting(const std::string& name)
{
   assert(name != "Back");
   assert(name != "");
   assert(FindSetting(name) == -1);
   mSettings.push_back(Setting(name));
}

//*****************************************************************************
void Menu::AddSettingOption(const std::string& settingName,
                            const std::string& optionName,
                            bool isDefault)
{
   assert(settingName != "");
   assert(optionName != "");
   int settingIndex = FindSetting(settingName);
   assert(settingIndex != -1);
   assert(FindOption(settingName, optionName) == -1);

   if(isDefault)
   {
      mSettings[settingIndex].mOptionIndex = mSettings[settingIndex].mOptions.size();
      mSettings[settingIndex].mOptionIndexDefault = mSettings[settingIndex].mOptions.size();
   }

   mSettings[settingIndex].mOptions.push_back(optionName);
}

//*****************************************************************************
void Menu::AddSubMenu(Menu& menu)
{
   mSettings.push_back(Setting(menu));
}

//*****************************************************************************
std::string Menu::ShowMenu(Screen& screen)
{
   bool previousMouseVisible = screen.GetMouseCursorVisible();
   screen.SetMouseCursorVisible(true);
   
   std::string result = "";
   mSettingIndex = 0;
   mChangingOption = false;

   if(mSettings.empty() || (mSettings[mSettings.size() - 1].mName != "Back"))
      mSettings.push_back(Setting("Back"));

   EnsureThatDisabledSettingOrOptionIsNotSelected();

   sf::Clock clock;

   do
   {
      sf::Event event;
      while (screen.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
            result = "WINDOW_CLOSED_EVENT";
         else if (event.type == sf::Event::Resized)
            screen.HandleResizeEvent();
         else if (event.type == sf::Event::KeyReleased)
         {
            if(result == "")
            {
               result = KeyReleaseEvent(event.key.code, screen);

               if(mpMenuCallBack != 0)
               {
                  mpMenuCallBack->UpdateMenuOptions();
                  EnsureThatDisabledSettingOrOptionIsNotSelected();
               }
            }
         }
      } 

      if (result == "")
      {
         result = CheckMouseEvent(screen);

         if (mpMenuCallBack != 0)
         {
            mpMenuCallBack->UpdateMenuOptions();
            EnsureThatDisabledSettingOrOptionIsNotSelected();
         }

      }

      screen.clear();
      DrawMenu(screen, clock.restart().asSeconds());
      screen.DisplayNewScreen();

   } while(result == "");

   screen.SetMouseCursorVisible(previousMouseVisible);
   
   return result;
}

//*****************************************************************************
const std::string& Menu::GetSelection(const std::string& settingName)
{
   assert(settingName != "");
   int settingIndex = FindSetting(settingName);
   assert(settingIndex != -1);
   assert(!mSettings[settingIndex].mOptions.empty()); 

   return mSettings[settingIndex].mOptions[mSettings[settingIndex].mOptionIndex].mName;
}

//*****************************************************************************
void Menu::Load()
{
   std::ifstream file;
   file.open("Settings");

   if(file.fail() || !LoadSettingsFromFile(file))
      SetToDefaultOptions();

   file.close();

   if(mpMenuCallBack != 0)
   {
      mpMenuCallBack->UpdateMenuOptions();
      EnsureThatDisabledSettingOrOptionIsNotSelected();
   }
}

//*****************************************************************************
void Menu::Save()
{
   std::ofstream file;
   file.open("Settings");
   SaveSettingsToFile(file);
   file.close();
}

//*****************************************************************************
void Menu::ChooseSettingOption(const std::string& settingName,
                               const std::string& optionName)
{
   assert(settingName != "");
   assert(optionName != "");
   int settingIndex = FindSetting(settingName);
   assert(settingIndex != -1);
   assert(!mSettings[settingIndex].mOptions.empty()); 
   int optionIndex = FindOption(settingName, optionName);
   assert(optionIndex != -1);
   mSettings[settingIndex].mOptionIndex = optionIndex;
}

//*****************************************************************************
void Menu::EnableSetting(const std::string& name)
{
   assert(name != "");
   int settingIndex = FindSetting(name);
   assert(settingIndex != -1);
   mSettings[settingIndex].mEnabled = true;
}

//*****************************************************************************
void Menu::DisableSetting(const std::string& name)
{
   assert(name != "");
   int settingIndex = FindSetting(name);
   assert(settingIndex != -1);
   mSettings[settingIndex].mEnabled = false;
   EnsureThatDisabledSettingOrOptionIsNotSelected();
}

//*****************************************************************************
void Menu::EnableSettingOption(const std::string& settingName, 
                               const std::string& optionName)
{
   assert(settingName != "");
   assert(optionName != "");
   int settingIndex = FindSetting(settingName);
   assert(settingIndex != -1);
   assert(!mSettings[settingIndex].mOptions.empty()); 
   int optionIndex = FindOption(settingName, optionName);
   assert(optionIndex != -1);
   mSettings[settingIndex].mOptions[optionIndex].mEnabled = true;
}
//*****************************************************************************
void Menu::DisableSettingOption(const std::string& settingName, 
                                const std::string& optionName)
{
   assert(settingName != "");
   assert(optionName != "");
   int settingIndex = FindSetting(settingName);
   assert(settingIndex != -1);
   assert(!mSettings[settingIndex].mOptions.empty()); 
   int optionIndex = FindOption(settingName, optionName);
   assert(optionIndex != -1);
   mSettings[settingIndex].mOptions[optionIndex].mEnabled = false;
   EnsureThatDisabledSettingOrOptionIsNotSelected();
}

//*****************************************************************************
int Menu::FindSetting(const std::string& name) const
{
   int settingIndex = 0;

   while ((settingIndex < static_cast<int>(mSettings.size())) && 
          (mSettings[settingIndex].mName != name))
      settingIndex++;

   if(settingIndex == mSettings.size())
      settingIndex = -1;

   return settingIndex;
}

//*****************************************************************************
int Menu::FindOption(const std::string& settingName,
                     const std::string& optionName) const
{
   int optionIndex = -1;

   int settingIndex = FindSetting(settingName);
   
   if(settingIndex >= 0)
   {
      for(unsigned int i = 0; (optionIndex == -1) && (i < mSettings[settingIndex].mOptions.size()); i++)
      {
         if(mSettings[settingIndex].mOptions[i].mName == optionName)
            optionIndex = i;
      }
   }

   return optionIndex;
}

//*****************************************************************************
std::string Menu::KeyReleaseEvent(sf::Keyboard::Key key, Screen& screen)
{
   std::string result = "";

   if (!mChangingOption)
   {
      if (key == sf::Keyboard::Up)
      {
         if (!MoveSettingUp())
            MoveSettingDown();
      }
      else if (key == sf::Keyboard::Down)
      {
         if (!MoveSettingDown())
            MoveSettingUp();
      }
      else if (key == sf::Keyboard::Escape)
      {
         result = "Back";
      }
      else if ((key == sf::Keyboard::Return) || (key == sf::Keyboard::Right))
      {
         if (mSettings[mSettingIndex].mpSubMenu != 0)
         {
            std::string subMenuResult = mSettings[mSettingIndex].mpSubMenu->ShowMenu(screen);
            if (subMenuResult != "Back")
               result = subMenuResult;
         }
         else if (mSettings[mSettingIndex].mOptions.empty())
         {
            result = mSettings[mSettingIndex].mName;
         }
         else
         {
            mChangingOption = true;
         }
      }
   }
   else
   {
      if (key == sf::Keyboard::Up)
      {
         if (!MoveOptionUp())
            MoveOptionDown();
      }
      else if (key == sf::Keyboard::Down)
      {
         if (!MoveOptionDown())
            MoveOptionUp();
      }
      else if ((key == sf::Keyboard::Escape) ||
         (key == sf::Keyboard::Return) ||
         (key == sf::Keyboard::Left))
      {
         mChangingOption = false;
      }
   }

   return result;
}

//*****************************************************************************
std::string Menu::CheckMouseEvent(Screen& screen)
{
   std::string result = "";

   if (screen.MouseLeftButtonStatus() == Screen::JUST_PRESSED)
   {
      float titleX, titleY, titleScale, settingX, settingY, optionX, deltaY;

      DetermineDimensions(screen, mText, titleX, titleY, titleScale, settingX, settingY, optionX, deltaY);

      sf::Vector2f position = screen.GetMouseViewPosition();

      for (unsigned int settingIndex = 0; settingIndex < mSettings.size(); ++settingIndex)
      {
         float thisSettingY = settingY + deltaY * settingIndex;

         if((position.y >= (thisSettingY - mText.getCharacterSize() / 2.0f)) &&
            (position.y <= (thisSettingY + mText.getCharacterSize() / 1.5f)) &&
            (position.x >= settingX) &&
            (position.x <= (screen.GetViewWidth() - settingX)) &&
            (mSettings[settingIndex].mEnabled))
         {
            if (mSettings[settingIndex].mOptions.empty())
            {
               result = mSettings[settingIndex].mName;
            }
            else
            {
               if(mSettingIndex != settingIndex)
               {
                  mSettingIndex = settingIndex;
                  mChangingOption = false;
               }
               else
               {
                  if ((mSettings[settingIndex].mOptionIndex == mSettings[settingIndex].mOptions.size() - 1) ||
                      !MoveOptionDown())
                  {
                     for(unsigned int i = 0; i < mSettings[settingIndex].mOptions.size(); i++)
                     {
                        if (!MoveOptionUp())
                           MoveOptionDown();
                     }
                  }
               }
            }
         }
      }
   }

   return result;
}

//*****************************************************************************
bool Menu::MoveSettingUp()
{
   if (mSettingIndex > 0)
   {
      --mSettingIndex;

      while (!SettingIsAvailable(mSettingIndex) && (mSettingIndex > 0))
         --mSettingIndex;
   }

   return SettingIsAvailable(mSettingIndex);
}

//*****************************************************************************
bool Menu::MoveSettingDown()
{
   if (mSettingIndex < (mSettings.size() - 1))
   {
      ++mSettingIndex;

      while (!SettingIsAvailable(mSettingIndex) && (mSettingIndex < (mSettings.size() - 1)))
         ++mSettingIndex;
   }

   return SettingIsAvailable(mSettingIndex);
}

//*****************************************************************************
bool Menu::MoveOptionUp()
{
   if (mSettings[mSettingIndex].mOptionIndex > 0)
   {
      --mSettings[mSettingIndex].mOptionIndex;

      while (!mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled &&
             (mSettings[mSettingIndex].mOptionIndex > 0))
         --mSettings[mSettingIndex].mOptionIndex;
   }

   return mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled;
}

//*****************************************************************************
bool Menu::MoveOptionDown()
{
   if (mSettings[mSettingIndex].mOptionIndex < (mSettings[mSettingIndex].mOptions.size() - 1))
   {
      ++mSettings[mSettingIndex].mOptionIndex;

      while (!mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled &&
             (mSettings[mSettingIndex].mOptionIndex < (mSettings[mSettingIndex].mOptions.size() - 1)))
         ++mSettings[mSettingIndex].mOptionIndex;
   }

   return mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled;
}

//*****************************************************************************
bool Menu::SettingIsAvailable(unsigned int settingIndex) const
{
   bool result = false;

   if(mSettings[settingIndex].mEnabled)
   {
      if(!mSettings[settingIndex].mOptions.empty())
      {
         for(unsigned int optionIndex = 0; 
             !result && (optionIndex < mSettings[settingIndex].mOptions.size());
             ++optionIndex)
         {
            if(mSettings[settingIndex].mOptions[optionIndex].mEnabled)
               result = true;
         }
      }
      else
      {
         result = true;
      }
   }

   return result;
}

//*****************************************************************************
void Menu::EnsureSelectedOptionIsEnabled()
{
   if(!mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled)
   {
      mSettings[mSettingIndex].mOptionIndex = 0;

      if(!mSettings[mSettingIndex].mOptions[mSettings[mSettingIndex].mOptionIndex].mEnabled)
         static_cast<void>(MoveOptionDown());
   }
}

//*****************************************************************************
void Menu::EnsureThatDisabledSettingOrOptionIsNotSelected()
{
   unsigned int currentSettingIndex = mSettingIndex;

   // First, make sure all the selected options for each setting are enabled.
   for (mSettingIndex = 0; mSettingIndex < mSettings.size(); ++mSettingIndex)
   {
      if(!mSettings[mSettingIndex].mOptions.empty())
         EnsureSelectedOptionIsEnabled();
   }

   mSettingIndex = currentSettingIndex;

   if(!SettingIsAvailable(mSettingIndex))
   {
      if (!MoveSettingDown())
         MoveSettingUp();
   }
}

//*****************************************************************************
void Menu::DrawMenu(Screen& screen, float deltaTime)
{
   float titleX, titleY, titleScale, settingX, settingY, optionX, deltaY;                      

   DetermineDimensions(screen, mText, titleX, titleY, titleScale, settingX, settingY, optionX, deltaY);                     

   float pulsatingScale = CalculatePulsatingScale(deltaTime);

   mText.setPosition(titleX, titleY);
   mText.setScale(titleScale, titleScale);
   mText.setString(mName);
   screen.draw(mText);

   for (unsigned int i = 0; i < mSettings.size(); ++i)
   {
      if((i == mSettingIndex) && !mChangingOption)
         mText.setScale(pulsatingScale, pulsatingScale);
      else if(SettingIsAvailable(i))
         mText.setScale(1.0f, 1.0f);
      else
         mText.setScale(0.6f, 0.6f);

      mText.setPosition(settingX, settingY);

      mText.setString(mSettings[i].mName);

      screen.draw(mText);

      if(!mSettings[i].mOptions.empty())
      {
         if((i == mSettingIndex) && mChangingOption)
            mText.setScale(pulsatingScale, pulsatingScale);
         else if(mSettings[i].mOptions[mSettings[i].mOptionIndex].mEnabled)
            mText.setScale(1.0f, 1.0f);
         else
            mText.setScale(0.9f, 0.9f);

         mText.setPosition(optionX, settingY);
         mText.setString(mSettings[i].mOptions[mSettings[i].mOptionIndex].mName);
         screen.draw(mText);
      }

      settingY += deltaY;
   }
}

//*****************************************************************************
float Menu::CalculatePulsatingScale(float deltaTime)
{
   static float pulsatingScale = 1.0f;
   static float pulsatingScaleRate = 0.2f;
   const float pulsatingScaleMin = 0.97f;
   const float pulsatingScaleMax = 1.03f;

   pulsatingScale += pulsatingScaleRate * deltaTime;
   if (pulsatingScale > pulsatingScaleMax)
   {
      pulsatingScale = pulsatingScaleMax;
      pulsatingScaleRate *= -1.0f;
   }
   else if (pulsatingScale < pulsatingScaleMin)
   {
      pulsatingScale = pulsatingScaleMin;
      pulsatingScaleRate *= -1.0f;
   }

   return pulsatingScale;
}

//*****************************************************************************
void Menu::DetermineDimensions(const Screen& screen,
                               sf::Text& text,
                               float& titleX,
                               float& titleY,
                               float& titleScale,
                               float& settingX,
                               float& settingY,
                               float& optionX,
                               float& deltaY) const
{
   sf::View view = screen.getView();

   float screenWidth = view.getSize().x;
   float screenHeight = view.getSize().y;
   float screenX = view.getCenter().x - screenWidth / 2.0f;
   float screenY = view.getCenter().y- screenHeight / 2.0f;;

   // Proportions:
   float topMargin = 1.0f;
   float titleTextHeight = 1.5f;
   float verticalSpacingTitleToSettings = 3.0f;
   float settingTextHeight = 1.0f;
   float settingSpacing = 1.5f;
   float bottomMargin = 1.0f;

   unsigned int numberOfSettings = mSettings.size();
   if(numberOfSettings < 7)
      numberOfSettings = 7;

   float proportionalHeight = topMargin +
                              verticalSpacingTitleToSettings + 
                              settingSpacing * (numberOfSettings - 1) +
                              settingTextHeight +
                              bottomMargin;

   float screenRatio = screenHeight / proportionalHeight;
   topMargin *= screenRatio;
   titleTextHeight *= screenRatio;
   verticalSpacingTitleToSettings *= screenRatio;
   settingTextHeight *= screenRatio;
   settingSpacing *= screenRatio;
   bottomMargin *= screenRatio;

   text.setCharacterSize(static_cast<unsigned int>(settingTextHeight + 0.5f));
   text.setString(mName);
   text.setScale(1.0f, 1.0f);
   text.setOrigin(0.0f, text.getCharacterSize() / 2.0f); 
   sf::Rect<float> titleRect = text.getGlobalBounds();

   float maxSettingWidth = 0.0f;
   float maxOptionWidth = 0.0f;

   for(unsigned int settingIndex = 0; settingIndex < mSettings.size(); settingIndex++)
   {
      text.setString(mSettings[settingIndex].mName);
      sf::Rect<float> settingRect = text.getGlobalBounds();
      if(settingRect.width > maxSettingWidth)
         maxSettingWidth = settingRect.width;

      for(unsigned int optionIndex = 0;
          optionIndex < mSettings[settingIndex].mOptions.size();
          optionIndex++)
      {
         text.setString(mSettings[settingIndex].mOptions[optionIndex].mName);
         sf::Rect<float> optionRect = text.getGlobalBounds();
         if(optionRect.width > maxOptionWidth)
            maxOptionWidth = optionRect.width;
      }
   }

   if(maxOptionWidth > 0.01f)
      maxSettingWidth *= 1.2f;

   titleScale = 1.4f;
   titleX = screenX + screenWidth / 2.0f - titleRect.width * titleScale / 2.0f;
   titleY = screenY + topMargin;
   settingX = screenX + screenWidth / 2.0f - (maxSettingWidth + maxOptionWidth) / 2.0f;
   optionX = settingX + maxSettingWidth;

   if(numberOfSettings > mSettings.size())
      titleY += (numberOfSettings - mSettings.size()) * settingSpacing / 2.0f;
   settingY = titleY + verticalSpacingTitleToSettings;
   deltaY = settingSpacing;       
}

//*****************************************************************************
bool Menu::LoadSettingsFromFile(std::ifstream& file)
{
   bool success = true;
   std::string optionName;

   for(unsigned int settingIndex = 0;
       success && (settingIndex < mSettings.size());
       settingIndex++)
   {
      if(mSettings[settingIndex].mpSubMenu != 0)
      {
         mSettings[settingIndex].mpSubMenu->LoadSettingsFromFile(file);
      }
      else if(!mSettings[settingIndex].mOptions.empty())
      {
         file >> optionName;

         int optionIndex = FindOption(mSettings[settingIndex].mName, optionName);

         if(optionIndex != -1)
            mSettings[settingIndex].mOptionIndex = optionIndex;
         else
            success = false;
      }
   }

   return success;
}

//*****************************************************************************
void Menu::SaveSettingsToFile(std::ofstream& file)
{
   for(unsigned int settingIndex = 0; settingIndex < mSettings.size(); settingIndex++)
   {
      if(mSettings[settingIndex].mpSubMenu != 0)
         mSettings[settingIndex].mpSubMenu->SaveSettingsToFile(file);
      else if(!mSettings[settingIndex].mOptions.empty())
         file << mSettings[settingIndex].mOptions[mSettings[settingIndex].mOptionIndex].mName
              << "\n";
   }
}

//*****************************************************************************
void Menu::SetToDefaultOptions()
{
   for(unsigned int settingIndex = 0; settingIndex < mSettings.size(); settingIndex++)
   {
      if(mSettings[settingIndex].mpSubMenu != 0)
         mSettings[settingIndex].mpSubMenu->SetToDefaultOptions();
      else if(!mSettings[settingIndex].mOptions.empty())
         mSettings[settingIndex].mOptionIndex = mSettings[settingIndex].mOptionIndexDefault;
   }
}
