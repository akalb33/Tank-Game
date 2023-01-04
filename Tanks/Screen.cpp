#include "stdafx.h"
#include "Screen.h"
#include <SFML/Window/Mouse.hpp>
#include <iostream>


// User should never have to change this class.
Screen::Screen(float viewX,
    float viewY,
    float viewWidth,
    float viewHeight,
    bool isFullScreen,
    const std::string& title)
    : mView()
    , mViewX(viewX)
    , mViewY(viewY)
    , mViewWidth(viewWidth)
    , mViewHeight(viewHeight)
    , mViewXOffsetForFullScreen(0.0f)
    , mViewYOffsetForFullScreen(0.0f)
    , mIsFullScreen(false)
    , mIsMouseCursorVisible(false)
    , mTitle(title)
    , mBar1X(0.0f)
    , mBar1Y(0.0f)
    , mBar2X(0.0f)
    , mBar2Y(0.0f)
    , mBar1()
    , mBar2()
    , mDesktopPosition(0, 0) // Initial window starting position.
    , mDesktopWidth(0)
    , mDesktopHeight(0)
    , mPreviousDesktopWidth(0)
    , mPreviousDesktopHeight(0)
    , mTimeAccumulator(mClock.restart().asSeconds())
    , mFrameCounter(0)
    , mMaxSecondsPerFrame(0.0f)
    , mOneSecondAccumulator(0.0f)
    , mMinAverageFPS(10000000.0f)
{
   // Make the initial non-full screen window size be 2/3 the desktop resolution.
   //    These values will be adjusted in MatchDesktopRatioToViewRatio() before
   //    the window is created.
   sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
   mDesktopWidth = static_cast<unsigned int>(static_cast<float>(videoMode.width) * 0.66f + 0.5f);
   mDesktopHeight = static_cast<unsigned int>(static_cast<float>(videoMode.height) * 0.66f + 0.5f);
   mPreviousDesktopWidth = mDesktopWidth;
   mPreviousDesktopHeight = mDesktopHeight;

   // Create the bars for full screen mode ahead of time for efficiency. Grab the
   //    first video mode in the vector which SFML says is the best choice. 
   videoMode = *sf::VideoMode::getFullscreenModes().begin();

   float desiredXYRatio = mViewWidth / mViewHeight;
   float actualXYRatio = static_cast<float>(videoMode.width) / static_cast<float>(videoMode.height);

   mBar1.setFillColor(sf::Color::Black);
   mBar2.setFillColor(sf::Color::Black);

   if (desiredXYRatio > actualXYRatio)
   {
      // Top and bottom bars are required.
      float barHeight = (mViewWidth / actualXYRatio - mViewHeight) / 2.0f;
      mBar1.setSize(sf::Vector2f(mViewWidth, barHeight));
      mBar2.setSize(sf::Vector2f(mViewWidth, barHeight));

      mBar1X = 0.0f;
      mBar1Y = -barHeight;

      mBar2X = 0.0f;
      mBar2Y = mViewHeight;

      mViewXOffsetForFullScreen = 0.0f;
      mViewYOffsetForFullScreen = barHeight;
   }
   else
   {
      // Left and right bars are required.
      float barWidth = (actualXYRatio * mViewHeight - mViewWidth) / 2.0f;
      mBar1.setSize(sf::Vector2f(barWidth, mViewHeight));
      mBar2.setSize(sf::Vector2f(barWidth, mViewHeight));

      mBar1X = -barWidth;
      mBar1Y = 0.0f;

      mBar2X = mViewWidth;
      mBar2Y = 0.0f;

      mViewXOffsetForFullScreen = barWidth;
      mViewYOffsetForFullScreen = 0.0f;
   }

   CreateNonFullScreenWindow();
   SetToFullScreen(isFullScreen);
   SetMouseCursorVisible(true);
}

Screen::~Screen()
{
}

void Screen::HandleResizeEvent()
{
   // If full screen, this function was probably called in error.
   if(!mIsFullScreen)
   {
      mDesktopPosition = getPosition();
      sf::Vector2u size = getSize();
      mDesktopWidth = size.x;
      mDesktopHeight = size.y;

      CreateNonFullScreenWindow();
   }
}

void Screen::ToggleFullScreen()
{
   SetToFullScreen(!mIsFullScreen);
}

void Screen::SetToFullScreen(bool isFullScreen)
{
   // Save off current desktop position of window, to restore later.
   if(!mIsFullScreen)
   {
      mDesktopPosition = getPosition();
      mPreviousDesktopWidth = mDesktopWidth;
      mPreviousDesktopHeight = mDesktopHeight;
   }
 
   mIsFullScreen = isFullScreen;

   if(isFullScreen)
   {
      CreateFullScreenWindow();
   }
   else
   {
      mDesktopWidth = mPreviousDesktopWidth;
      mDesktopHeight = mPreviousDesktopHeight;

      CreateNonFullScreenWindow();
   }

   setMouseCursorVisible(!isFullScreen);
}

void Screen::SetMouseCursorVisible(bool visible)
{
   mIsMouseCursorVisible = visible;
   setMouseCursorVisible(visible);
}

void Screen::SetViewPosition(float x, float y)
{
   mViewX = x;
   mViewY = y;

   if(mIsFullScreen)
      mView.reset(sf::Rect<float>(mViewX - mViewXOffsetForFullScreen,
                                  mViewY - mViewYOffsetForFullScreen,
                                  mViewWidth + mViewXOffsetForFullScreen * 2.0f,
                                  mViewHeight + mViewYOffsetForFullScreen * 2.0f));
   else
      mView.reset(sf::Rect<float>(mViewX, mViewY, mViewWidth, mViewHeight));

   setView(mView);
}

sf::Vector2f Screen::GetMouseViewPosition()
{
   sf::Vector2f mousePosition;

   if (mIsFullScreen)
   {
      float ratioViewToPixels = (mViewWidth + 2.0f * mViewXOffsetForFullScreen) / mDesktopWidth;
      mousePosition.x = sf::Mouse::getPosition(*this).x * ratioViewToPixels + mViewX - mViewXOffsetForFullScreen;
      mousePosition.y = sf::Mouse::getPosition(*this).y * ratioViewToPixels + mViewY - mViewYOffsetForFullScreen;
   }
   else
   {
      float ratioViewToPixels = mViewWidth / mDesktopWidth;
      mousePosition.x = sf::Mouse::getPosition(*this).x * ratioViewToPixels + mViewX;
      mousePosition.y = sf::Mouse::getPosition(*this).y * ratioViewToPixels + mViewY;
   }

   return mousePosition;
}

Screen::MouseButtonStatus Screen::MouseLeftButtonStatus()
{
   static bool previouslyPressed = false;
   bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
   Screen::MouseButtonStatus buttonStatus = NOT_PRESSED;
   if (pressed)
   {
      if(previouslyPressed)
      {
         buttonStatus = PRESSED;
      }
      else
      {
         buttonStatus = JUST_PRESSED;
      }
   }
   else if (previouslyPressed)
   {
      buttonStatus = JUST_RELEASED;
   }

   previouslyPressed = pressed;

   return buttonStatus;
}

void Screen::DisplayNewScreen()
{
   static bool middleButtonPreviouslyPressed = false;
   if(mIsFullScreen)
   {
      mBar1.setPosition(mViewX + mBar1X, mViewY + mBar1Y);
      mBar2.setPosition(mViewX + mBar2X, mViewY + mBar2Y);

      draw(mBar1);
      draw(mBar2);
   }
   // uncomment to debug and to display mouse/cursor coordinates when center button is pressed
   if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
   {
      if (!middleButtonPreviouslyPressed)
      {
         std::cout << GetMouseViewPosition().x << " " << GetMouseViewPosition().y << "\n";
         middleButtonPreviouslyPressed = true;
      }
   }
   else
   {
      middleButtonPreviouslyPressed = false;
   }


   display();
}
// Call everytime you DisplayAll();

void Screen::OutputFramerate(float interval)
{
    float deltaTime = mClock.restart().asSeconds();
    mOneSecondAccumulator += deltaTime;
    if (deltaTime > mMaxSecondsPerFrame)
    {
        mMaxSecondsPerFrame = deltaTime;
    }
    mFrameCounter++;

    if (mOneSecondAccumulator > 1.0f)
    {
        mTimeAccumulator += mOneSecondAccumulator;
        float avg = mFrameCounter / mOneSecondAccumulator;
        if (avg < mMinAverageFPS)
        {
            mMinAverageFPS = avg;
        }
        mOneSecondAccumulator = 0.0f;
        mFrameCounter = 0;

        if (mTimeAccumulator > interval)
        {
            std::cout << "AVG FPS: " << mMinAverageFPS << " MIN FPS: " << 1.0f / mMaxSecondsPerFrame << "\n";
            mTimeAccumulator = 0.0f;
            mMaxSecondsPerFrame = 0.0f;
            mMinAverageFPS = 10000000.0f;
        }
    }


}

void Screen::CreateFullScreenWindow()
{
   create(*sf::VideoMode::getFullscreenModes().begin(), mTitle, sf::Style::Fullscreen);
   SetViewPosition(mViewX, mViewY);

   // In fullscreen mode, mDesktopHeight and mDesktopWidth include the bars
   //   if they are present.
   sf::Vector2u desktopSize = getSize();
   mDesktopHeight = desktopSize.y;
   mDesktopWidth = desktopSize.x;
}

void Screen::CreateNonFullScreenWindow()
{
   MatchDesktopRatioToViewRatio();
   create(sf::VideoMode(mDesktopWidth, mDesktopHeight), mTitle);
   setPosition(mDesktopPosition);
   SetViewPosition(mViewX, mViewY);
}

void Screen::MatchDesktopRatioToViewRatio()
{
   float desiredXYRatio = mViewWidth / mViewHeight;
   float actualXYRatio = static_cast<float>(mDesktopWidth) / static_cast<float>(mDesktopHeight);

   if (desiredXYRatio > actualXYRatio)
   {
      mDesktopHeight = static_cast<unsigned int>(static_cast<float>(mDesktopWidth) / desiredXYRatio + 0.5f);
   }
   else
   {
      mDesktopWidth = static_cast<unsigned int>(static_cast<float>(mDesktopHeight) * desiredXYRatio + 0.5f);
   }

   const unsigned int minimumWidth = 200;
   if (mDesktopWidth < minimumWidth)
   {
      mDesktopWidth = minimumWidth;
      mDesktopHeight = static_cast<unsigned int>(static_cast<float>(mDesktopWidth) / desiredXYRatio + 0.5f);
   }
}
