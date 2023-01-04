#include "stdafx.h"
#include "SoundManager.h"
#include <assert.h>

void SoundManager::AddSound(Sound sound,
                            std::string fileName, 
                            float defaultVolume,
                            bool loop,
                            float pitch)
{
   assert(sound < static_cast<signed int>(MAX_SOUNDS));

   if(!mSoundSettings[sound].mSoundBuffer.loadFromFile(fileName))
      assert(false);

   mSoundSettings[sound].mDefaultVolume = defaultVolume;
   mSoundSettings[sound].mLoop = loop;
   mSoundSettings[sound].mPitch = pitch;
}

void SoundManager::Play(Sound sound)
{
   assert(sound < static_cast<signed int>(MAX_SOUNDS));
   Play(sound, mSoundSettings[sound].mDefaultVolume);
}

void SoundManager::Play(Sound sound, float volume)
{
   assert(sound < static_cast<signed int>(MAX_SOUNDS));

   bool done = false;

   for (unsigned int i = 0; !done && (i < MAX_SIMULTANEOUS_SOUNDS); i++)
   {
      InProgressSound* pInProgressSound = &mInProgressSound[i];

      if (pInProgressSound->mSFSound.getStatus() == sf::SoundSource::Status::Stopped)
      {
         SoundSetting* pSoundSetting = &mSoundSettings[sound];

         pInProgressSound->mSFSound.setBuffer(pSoundSetting->mSoundBuffer);
         pInProgressSound->mSFSound.setVolume(volume);
         pInProgressSound->mSFSound.setLoop(pSoundSetting->mLoop);
         pInProgressSound->mSFSound.setPitch(pSoundSetting->mPitch);

         pInProgressSound->mSFSound.play();
         pInProgressSound->mSound = sound;
         done = true;
      }
   }
}

void SoundManager::Stop(Sound sound)
{
   for (unsigned int i = 0; i < MAX_SIMULTANEOUS_SOUNDS; i++)
   {
      InProgressSound* pInProgressSound = &mInProgressSound[i];

      if((pInProgressSound->mSound == sound) &&
         (pInProgressSound->mSFSound.getStatus() == sf::SoundSource::Playing))
         pInProgressSound->mSFSound.stop();
   }
}
