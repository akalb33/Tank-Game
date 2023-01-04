#pragma once

#include "Screen.h"
#include <string>
#include <vector>
#include <fstream>

class IMenuCallBack
{
public:
   IMenuCallBack() {}
   virtual ~IMenuCallBack() {}
   virtual void UpdateMenuOptions() = 0;
};

// To create a menu, do the following steps:
//
//    sf::Font font;
//    assert(font.loadFromFile("arial.ttf"));
//    sf::Text text;
//    text.setFont(font);  You could also set color and make it bold, etc.
//
//    Menu menu;
//    menu.SetMenuTitle("Main Menu");
//    menu.SetText(text);
//    menu.SetMenuCallBack(myCallbackFunction);   This is optional.
//
//    The following 3 lines add a setting called Difficulty with two options, easy and hard.
//    menu.AddSetting("Difficulty:");
//    menu.AddSettingOption("Difficulty:", "easy", true);   Make easy be the default
//    menu.AddSettingOption("Difficulty:", "hard");
//
//    menu.AddSubMenu(videoSettingsMenu);   This adds an option for the user to jump to another menu.
//
//    Add a setting with no options. This text will be returned from ShowMenu() if user selects it.
//    menu.AddSetting("Restart Game");
//
//    This loads previous settings from a file. Just remember to call menu.Save() before 
//       exiting your game.
//    menu.Load();  
//
//    std::string result = menu.ShowMenu();
//
//    if(result == "Restart Game")
//       do work to reset the game
//
//    if(menu.GetSelection("Difficulty:") == "easy")
//       do work to make the game easy

class Menu
{
public:
   Menu();

   void SetMenuTitle(const std::string& name) { mName = name; }

   // Tells menu to use the specified text which includes font, color, bold, italics, etc.
   // The menu will automatically set the point size for the font when it is displayed. 
   void SetText(const sf::Text& text) { mText = text; }

   // Setting name must be unique to this menu. You can have a setting with
   //    no options. If the user selects it, it will be the string returned
   //    from the call to ShowMenu().
   // NOTE: All Menus automatically have a setting with no options called "Back".
   //    It is the last option. You do not need to it. 
   void AddSetting(const std::string& title);

   // Option name must be unique for this setting, and 
   //    there should only be one default.
   void AddSettingOption(const std::string& settingName, 
                         const std::string& optionName,
                         bool isDefault = false);

   // This creates an line in the menu, and if the user selects it, it will go
   //    into that new menu's settings.
   void AddSubMenu(Menu& menu);

   // Brings up this menu. This function will return when the user selects a
   //    setting with no options. The title of the selected setting is returned.
   // NOTE: All Menus have a setting with no options called "Back". It is the
   //    last option.
   std::string ShowMenu(Screen& screen);

   // Returns selected option.
   const std::string& GetSelection(const std::string& settingName);
   
   // Only call these on the top level menu, to save/load data for all menus.
   //    Call Load() after you have added all the menu settings and options, and
   //    call Save() prior to exiting the game. This will automatically generate
   //    a text file named "Settings", if one does not already exist.
   void Load();
   void Save();

   // The following functions are to add cross option dependencies.
   // SetMenuCallBack() is used to select the user function that will perform the
   //    cross option dependencies.
   static void SetMenuCallBack(IMenuCallBack& menuCallback) { mpMenuCallBack = &menuCallback; }

   // Asserts if menu option does not exist.
   void ChooseSettingOption(const std::string& settingName,
                            const std::string& optionName);
   void EnableSetting(const std::string& name);
   void DisableSetting(const std::string& name);
   void EnableSettingOption(const std::string& settingName, 
                            const std::string& optionName);
   void DisableSettingOption(const std::string& settingName, 
                             const std::string& optionName);

private:
   struct Option
   {
      Option(const std::string name)
         : mName(name)
         , mEnabled(true)
      { }

      std::string mName;
      bool mEnabled;
   };

   struct Setting
   {
      Setting(const std::string name)
         : mpSubMenu(0)
         , mName(name)
         , mOptions()
         , mOptionIndex(0)
         , mOptionIndexDefault(0)
         , mEnabled(true)
      { }

      Setting(Menu& subMenu)
         : mpSubMenu(&subMenu)
         , mName(subMenu.mName)
         , mOptions()
         , mOptionIndex(0)
         , mEnabled(true)
      { }

      Menu* mpSubMenu; // NULL means it is a setting not a submenu.
      std::string mName;
      std::vector<Option> mOptions;
      unsigned int mOptionIndex;
      unsigned int mOptionIndexDefault;
      bool mEnabled;
   };

   // Returns -1 if setting is not found.
   int FindSetting(const std::string& name) const;

   // Returns -1 if option is not found.
   int FindOption(const std::string& settingName, const std::string& optionName) const;

   std::string KeyReleaseEvent(sf::Keyboard::Key key, Screen& screen);

   std::string CheckMouseEvent(Screen& screen);

   bool MoveSettingUp();
   bool MoveSettingDown();
   bool MoveOptionUp();
   bool MoveOptionDown();

   // setting enabled plus a minimum of one enabled option
   bool SettingIsAvailable(unsigned int settingIndex) const;
   void EnsureSelectedOptionIsEnabled(); // If not, it will change the setting to the first enabled one.
   void EnsureThatDisabledSettingOrOptionIsNotSelected();
   void DrawMenu(Screen& screen, float deltaTime);
   float CalculatePulsatingScale(float deltaTime);
   void DetermineDimensions(const Screen& screen,
                            sf::Text& text,
                            float& titleX,
                            float& titleY,
                            float& titleScale,
                            float& settingX,
                            float& settingY,
                            float& optionX,
                            float& deltaY) const;

   bool LoadSettingsFromFile(std::ifstream& file);
   void SaveSettingsToFile(std::ofstream& file);
   void SetToDefaultOptions();

   static IMenuCallBack* mpMenuCallBack;

   std::string mName;
   sf::Text mText;
   std::vector<Setting> mSettings;
   unsigned int mSettingIndex;
   bool mChangingOption;
};
