#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class SoundManager
{
public:
   enum Sound
   {
      bulletNoise,
      explosion,
      bulletContact
   };

   SoundManager()
   {
     AddSound(bulletNoise, "881.wav", 15.0f);
     AddSound(explosion, "Explosion.wav", 10.0f, false, 0.8f);
     AddSound(bulletContact, "Explosion.wav", 5.0f, false, 1.5f);
   }

   void AddSound(Sound sound,
                 std::string fileName, 
                 float defaultVolume = 100.0f, 
                 bool loop = false,
                 float pitch = 1.0f);
   void Play(Sound sound);
   void Play(Sound sound, float volume);
   void Stop(Sound sound);

private:
   static const unsigned int MAX_SOUNDS = 20;
   static const unsigned int MAX_SIMULTANEOUS_SOUNDS = 10;

   struct SoundSetting
   {
      sf::SoundBuffer mSoundBuffer;
      float mDefaultVolume;
      bool mLoop;
      float mPitch;
   };

   SoundSetting mSoundSettings[MAX_SOUNDS];

   struct InProgressSound
   {
      Sound mSound;
      sf::Sound mSFSound;
   };

   InProgressSound mInProgressSound[MAX_SIMULTANEOUS_SOUNDS];
};
