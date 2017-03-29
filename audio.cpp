// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// audio.cpp v1.2

#include "audio.h"

//==================================================
// default constructor
//=================================================
Audio::Audio()
{
	mute = false;
	// remove this line in release (debug purposes)
	// mute = true;
}

//==================================================
// default destructor
//=================================================
Audio::~Audio()
{
	// Shutdown XACT
	if (xactEngine) {
		xactEngine->ShutDown(); // Shut down XACT engine and free resources
		xactEngine->Release();
	}

	if (soundBankData)
		delete[] soundBankData;

	soundBankData = NULL;

	// After xactEngine->ShutDown() returns, release memory mapped files
	if (mapWaveBank)
		UnmapViewOfFile(mapWaveBank);
	mapWaveBank = NULL;

	if (coInitialized) // If CoInitializeEx succeeded
		CoUninitialize();
}

//==================================================
// initalise audio
// 
//=================================================
HRESULT Audio::initialise()
{
	HRESULT result = E_FAIL;
	HANDLE hFile;
	DWORD fileSize;
	DWORD bytesRead;
	HANDLE hMapFile;

	result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(result))
		result = XACT3CreateEngine(0, &xactEngine);

	if (FAILED(result) || xactEngine == NULL)
		return E_FAIL;

	result = XACT3CreateEngine(0, &xactEngine);
	if (FAILED(result) || xactEngine == NULL)
		return E_FAIL;

	// Load the global settings file and pass it into XACTInitialize
	VOID* pGlobalSettingsData = NULL;
	DWORD dwGlobalSettingsFileSize = 0;
	bool bSuccess = false;

	hFile = CreateFile(XGS_FILE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile) {
		dwGlobalSettingsFileSize = GetFileSize(hFile, NULL);
		if (dwGlobalSettingsFileSize != INVALID_FILE_SIZE) {
			pGlobalSettingsData = CoTaskMemAlloc(dwGlobalSettingsFileSize);
			if (pGlobalSettingsData) {
				if (0 != ReadFile(hFile, pGlobalSettingsData, dwGlobalSettingsFileSize, &bytesRead, NULL)) {
					bSuccess = true;
				}
			}
		}
		CloseHandle(hFile);
	}
	if (!bSuccess) {
		if (pGlobalSettingsData)
			CoTaskMemFree(pGlobalSettingsData);
		pGlobalSettingsData = NULL;
		dwGlobalSettingsFileSize = 0;
	}

	// Initialize & create the XACT runtime
	XACT_RUNTIME_PARAMETERS xactParams = { 0 }; xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT; result = xactEngine->Initialize(&xactParams);
	if (FAILED(result))
		return result;

	// Create an "in memory" XACT wave bank file using memory mapped file IO result = E_FAIL; // Default to failure code, replaced on success
	hFile = CreateFile(WAVE_BANK, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		fileSize = GetFileSize(hFile, NULL);

		if (fileSize != -1) {
			hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,
				fileSize, NULL);

			if (hMapFile) {
				mapWaveBank = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0,
					0);
				if (mapWaveBank)
					result = xactEngine->CreateInMemoryWaveBank(
					mapWaveBank, fileSize, 0, 0,
					&waveBank);
				// mapWaveBank maintains a handle on the file
				// Close this unneeded handle
				CloseHandle(hMapFile);
			}
		}
		// mapWaveBank maintains a handle on the file so close this unneeded // handle
		CloseHandle(hFile);
	}
	if (FAILED(result))
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	// Read and register the sound bank file with XACT
	result = E_FAIL;    // Default to failure code, replaced on success
	hFile = CreateFile(SOUND_BANK, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		fileSize = GetFileSize(hFile, NULL);

		if (fileSize != -1) {

			soundBankData = new BYTE[fileSize];

			if (soundBankData)
				// Reserve memory for
				// sound bank
			{
				if (0 != ReadFile(hFile, soundBankData, fileSize, &bytesRead, NULL))
					result = xactEngine->CreateSoundBank(soundBankData, fileSize,
						0, 0, &soundBank);
			}
		}
		CloseHandle(hFile);
	}


	if (FAILED(result))
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	return S_OK;
}

//======================================================================== 
// Perform periodic sound engine tasks
//======================================================================== 
void Audio::run()
{
	if (xactEngine == NULL)
		return;
	xactEngine->DoWork();
}

//======================================================================== 
// Play sound specified by cue from sound bank
// If cue does not exist no error occurs, there is simply no sound played 
//========================================================================
void Audio::playCue(const char cue[])
{
	if (soundBank == NULL)
		return;

	cueI = soundBank->GetCueIndex(cue); // Get cue index from sound bank 
	if (!mute)
		soundBank->Play(cueI, 0, 0, NULL);
}

//======================================================================== 
// Stop a playing sound specified by cue from sound bank
// If cue does not exist no error occurs 
//========================================================================
void Audio::stopCue(const char cue[])
{
	if (soundBank == NULL)
		return;

	cueI = soundBank->GetCueIndex(cue); // Get cue index from sound bank 
	soundBank->Stop(cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}

