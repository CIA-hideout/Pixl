/* Manages the audio in the game
 * 
 */
#pragma once

#include <iostream>
#include <xact3.h>

#include "constants.h"

class Audio {
private:
	IXACT3Engine* xactEngine;	// pointer to the XACT sound engine
	IXACT3WaveBank *waveBank;   // pointer to XACT wave bank
	IXACT3SoundBank *soundBank; // pointer to XACT sound bank
	IXACT3Wave *ppWave;         // pointer to XACT wave object
	XACTINDEX cueI;             // XACT sound index
	void *mapWaveBank;          // call UnmapViewOfFile() to release file
	void *soundBankData;
	bool coInitialized;         // set true if coInitialize is successful


public:
	// constructor
	Audio();

	bool mute;

	// destructor 
	virtual ~Audio();

	// Initialise Audio
	HRESULT initialise();

	// Perform periodic sound engine tasks
	void run();

	// Play sound specified by cue from sound bank
	// If cue does not exist no error occurs, there is simply no sound played
	void playCue(const char[]);

	// Stop a playing sound specified by cue from sound bank
	// If cue does not exist no error occurs
	void stopCue(const char[]);



};