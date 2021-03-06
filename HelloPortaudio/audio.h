#pragma once
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const int FRAMES_PER_BUFFER = 512;
static int paCallback(const void* inputBuffer, void* outputBuffer,
  unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void* userData);
void initializePA(int fs);
void closePA();


#endif
