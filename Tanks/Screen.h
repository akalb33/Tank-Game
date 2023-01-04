#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>

// User should never have to change this class.
class Screen : public sf::RenderWindow
{
public:
   // All arguments that refer to view, refer to the sf::View concept. It has
   //    nothing to do with the actual desktop window pixel size or position.
   //    It is the player's view of the world (world coordiates). All shapes
   //    and sprites are in world coordinates.
   // The mouse cursor defaults to be shown. This option can be changed by calling
   //    SetMouseCursorVisible().
   // Arguments:
   //    viewX, viewY - The origin of the view in terms of world coordinates.
   //    viewY - The origin of the view in terms of world coordinates.
   //    viewWidth, viewHeight - The size of the view in terms of world
   //       coordinates. This also fixes the aspect ratio of the displayed window
   //       so that sprites and shapes will maintain their proper aspect ratio.
   //       Therefore, if you do not wish for black bars to be shown when in full
   //       screen, the aspect ratio of viewWidth to viewHeight should be set to
   //       match the screen, typically 1.7778.
   //   isFullScreen - Set to true to make full screen and false to be windowed.
   //   title - The title to be show at the top when in windowed mode.
   Screen(float viewX,
          float viewY,
          float viewWidth,
          float viewHeight,
          bool isFullScreen,
          const std::string& title);

   virtual ~Screen();

   // If a sf::Event::Resized is detected, call this routine which will maintain
   //    the proper aspect ratio.
   void HandleResizeEvent();

   // Switch between full screen mode and windowed mode. For example, this can
   //    be called in response to the F11 key being pressed.
   void ToggleFullScreen();

   // This allows selection of full screen or windowed mode. 
   void SetToFullScreen(bool isFullScreen);

   // This sets the mouse cursor either visible or invisible, the mouse is
   //    visible by default.
   void SetMouseCursorVisible(bool visible);

   // Returns whether or not the mouse cursor is visible or invisible.
   bool GetMouseCursorVisible() const { return mIsMouseCursorVisible; }

   // This changes the view of the world (sprite/shapes). For example, this would
   //    be called in a side scroller game as the player moves. 
   void SetViewPosition(float x, float y);
   const sf::View& GetView() { return mView; }
   float GetViewX() { return mViewX; }
   float GetViewY() { return mViewY; }
   float GetViewWidth() { return mViewWidth; }
   float GetViewHeight() { return mViewHeight; }


   // This returns the mouse position in terms of world coordinates based on the view.
   sf::Vector2f GetMouseViewPosition();

   enum MouseButtonStatus
   {
      JUST_PRESSED,
      JUST_RELEASED,
      PRESSED,
      NOT_PRESSED
   };
   // Only call this once per evaluation of status, because it could change when called.
   MouseButtonStatus MouseLeftButtonStatus();

   // This should be called instead of calling display() on the sf::RenderWindow
   //    directly. This is because when in full screen mode, black bars need to
   //    be drawn prior to calling display() to cover up sprites which may be at
   //    the edge of the screen but slightly outside the view window. This 
   //    function does that automatically.
   void DisplayNewScreen();

   void OutputFramerate(float interval);

private:
   void CreateFullScreenWindow();
   void CreateNonFullScreenWindow();
   void MatchDesktopRatioToViewRatio();

private:
   sf::View mView;
   float mViewX;
   float mViewY;
   float mViewWidth;
   float mViewHeight;
   float mViewXOffsetForFullScreen;
   float mViewYOffsetForFullScreen;
   bool mIsFullScreen;
   bool mIsMouseCursorVisible;
   std::string mTitle;
   float mBar1X;
   float mBar1Y;
   float mBar2X;
   float mBar2Y;
   sf::RectangleShape mBar1;
   sf::RectangleShape mBar2;
   sf::Vector2i mDesktopPosition;
   unsigned int mDesktopWidth;
   unsigned int mDesktopHeight;
   unsigned int mPreviousDesktopWidth;
   unsigned int mPreviousDesktopHeight;
   sf::Clock mClock;
   float mTimeAccumulator;
   int mFrameCounter;
   float mMaxSecondsPerFrame;
   float mOneSecondAccumulator;
   float mMinAverageFPS;
};

