//---------------------------------------------------------------------------
#include "Engine/Audio/Audio.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


//---------------------------------------------------------------------------
// FMOD startup code based on "GETTING STARTED With FMOD Ex Programmer�s API for Windows" document
//	from the FMOD programming API at http://www.fmod.org/download/
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
	, m_musicChannel(nullptr)
{
	InitializeFMOD();
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	ShutDownFMOD();
}


//---------------------------------------------------------------------------
void AudioSystem::InitializeFMOD()
{
	const int MAX_AUDIO_DEVICE_NAME_LEN = 256;
	FMOD_RESULT result;
	unsigned int fmodVersion;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS deviceCapabilities;
	char audioDeviceName[ MAX_AUDIO_DEVICE_NAME_LEN ];

	// Create a System object and initialize.
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->getVersion( &fmodVersion );
	ValidateResult( result );

	if( fmodVersion < FMOD_VERSION )
	{
		DebuggerPrintf( "Engine/Audio SYSTEM ERROR!  Your FMOD .dll is of an older version (0x%08x == %d) than that the .lib used to compile this code (0x%08x == %d).\n", fmodVersion, fmodVersion, FMOD_VERSION, FMOD_VERSION );
	}

	result = m_fmodSystem->getNumDrivers( &numDrivers );
	ValidateResult( result );

	if( numDrivers == 0 )
	{
		result = m_fmodSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND );
		ValidateResult( result );
	}
	else
	{
		result = m_fmodSystem->getDriverCaps( 0, &deviceCapabilities, 0, &speakerMode );
		ValidateResult( result );

		// Set the user selected speaker mode.
		result = m_fmodSystem->setSpeakerMode( speakerMode );
		ValidateResult( result );

		if( deviceCapabilities & FMOD_CAPS_HARDWARE_EMULATED )
		{
			// The user has the 'Acceleration' slider set to off! This is really bad
			// for latency! You might want to warn the user about this.
			result = m_fmodSystem->setDSPBufferSize( 1024, 10 );
			ValidateResult( result );
		}

		result = m_fmodSystem->getDriverInfo( 0, audioDeviceName, MAX_AUDIO_DEVICE_NAME_LEN, 0 );
		ValidateResult( result );

		if( strstr( audioDeviceName, "SigmaTel" ) )
		{
			// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			// PCM floating point output seems to solve it.
			result = m_fmodSystem->setSoftwareFormat( 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR );
			ValidateResult( result );
		}
	}

	result = m_fmodSystem->init( 100, FMOD_INIT_NORMAL, 0 );
	if( result == FMOD_ERR_OUTPUT_CREATEBUFFER )
	{
		// Ok, the speaker mode selected isn't supported by this sound card. Switch it
		// back to stereo...
		result = m_fmodSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO );
		ValidateResult( result );

		// ... and re-init.
		result = m_fmodSystem->init( 100, FMOD_INIT_NORMAL, 0 );
		ValidateResult( result );
	}
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ShutDownFMOD()
{
//	FMOD_RESULT result = FMOD_OK;
//	result = FMOD_System_Close( m_fmodSystem );
//	result = FMOD_System_Release( m_fmodSystem );
	m_fmodSystem = nullptr;
}


//---------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFileName, SoundMode mode)
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFileName );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFileName.c_str(), mode, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFileName ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//---------------------------------------------------------------------------
void AudioSystem::StopChannel( AudioChannelHandle channel )
{
	if( channel != nullptr )
	{
		FMOD::Channel* fmodChannel = (FMOD::Channel*) channel;
		fmodChannel->stop();
	}
}


//---------------------------------------------------------------------------
AudioChannelHandle AudioSystem::PlaySound( SoundID soundID, float volumeLevel, AudioChannelHandle channel)
{
	unsigned int numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return nullptr;



	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*)channel;
	if (channelAssignedToSound)
	{
		m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, sound, false, &channelAssignedToSound);
		channelAssignedToSound->setVolume(volumeLevel);
	}
	else
	{
		m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound);
		if (channelAssignedToSound)
		{
			channelAssignedToSound->setVolume(volumeLevel);
		}
		channel = (AudioChannelHandle)channelAssignedToSound;
	}


	return (AudioChannelHandle)channelAssignedToSound;
}


AudioChannelHandle AudioSystem::PlaySound(const std::string& soundFileName, float volumeLevel, AudioChannelHandle channel)
{
	return PlaySound(CreateOrGetSound(soundFileName), volumeLevel, channel);
}

//---------------------------------------------------------------------------
void AudioSystem::Update()
{
	FMOD_RESULT result = m_fmodSystem->update();
	ValidateResult( result );
}
//////////////////////////////////////////////////////////////////////////
void AudioSystem::PlayMusic(SoundID soundID, float volumeLevel /*= 1.f*/)
{
	m_musicChannel = PlaySound(soundID, volumeLevel, m_musicChannel);
}

void AudioSystem::PlayMusic(const std::string& soundFileName, float volumeLevel /*= 1.f*/, SoundMode mode)
{
	SoundID soundID = CreateOrGetSound(soundFileName, mode);
	if (m_currentMusicSoundId == soundID)
		return;
	else
		m_currentMusicSoundId = soundID;
	PlayMusic(m_currentMusicSoundId, volumeLevel);
}

void AudioSystem::PlaySoundFX(const std::string& soundFileName, float volumeLevel /*= 1.f*/)
{
	PlaySound(soundFileName, volumeLevel);
}

void AudioSystem::StopMusic()
{
	StopChannel(m_musicChannel);
}

//---------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	GUARANTEE_OR_DIE( result == FMOD_OK, Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %d.\n", result ) );
}


