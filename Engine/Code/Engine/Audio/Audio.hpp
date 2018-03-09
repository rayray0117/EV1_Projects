#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library


//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;
//////////////////////////////////////////////////////////////////////////
enum SoundMode
{
	SOUNDMODE_DEFAULT = FMOD_DEFAULT,
	SOUNDMODE_LOOP_OFF = FMOD_LOOP_OFF,
	SOUNDMODE_LOOP_NORMAL = FMOD_LOOP_NORMAL,
	SOUNDMODE_LOOP_BIDI = FMOD_LOOP_BIDI,
	NUM_SOUNDMODES
};
/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();
	
	// Necessary function to run!
	void Update();

	//Easy to use functions!
	void PlayMusic(SoundID soundID, float volumeLevel = 1.f);
	void PlayMusic(const std::string& soundFileName, float volumeLevel = 1.f, SoundMode mode = SOUNDMODE_LOOP_NORMAL);
	void PlaySoundFX(const std::string& soundFileName, float volumeLevel = 1.f);
	void StopMusic();


	SoundID CreateOrGetSound( const std::string& soundFileName, SoundMode mode = SOUNDMODE_DEFAULT );

	AudioChannelHandle PlaySound( SoundID soundID, float volumeLevel=1.f, AudioChannelHandle channel = nullptr );
	AudioChannelHandle PlaySound( const std::string& soundFileName, float volumeLevel = 1.f,  AudioChannelHandle channel = nullptr);

	void StopChannel( AudioChannelHandle channel );

protected:
	void InitializeFMOD();
	void ShutDownFMOD();
	void ValidateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
	AudioChannelHandle					m_musicChannel;
	SoundID								m_currentMusicSoundId;
};

