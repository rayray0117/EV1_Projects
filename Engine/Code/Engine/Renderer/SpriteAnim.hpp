#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"

enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	//  SPRITE_ANIM_MODE_PINGPONG, 	// optional, play forwards, backwards, forwards...
	NUM_SPRITE_ANIM_MODES
};


//////////////////////////////////////////////////////////////////////////
class SpriteAnim
{
public:
	SpriteAnim(const SpriteSheet& spriteSheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimMode animMode = SPRITE_ANIM_MODE_PLAY_TO_END);
	~SpriteAnim();


	void Update(float deltaSeconds);
	int getCurrentSpriteIndex() const;
	int getCurrentFrameNumber() const;
	int getNumFrames() const;
	AABB2 GetCurrentTexCoords() const;	// Based on the current elapsed time
	const Texture& GetTexture() const;
	const Texture2D& GetTexture2D() const;
	void Pause();					// Starts unpaused (playing) by default
	void Resume();				// Resume after pausing
	void Reset();					// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const { return m_isFinished; }
	bool IsPlaying() const { return m_isPlaying; }
	float GetDurationSeconds() const { return m_durationSeconds; }
	float GetSecondsElapsed() const { return m_elapsedSeconds; }
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	void SetSecondsElapsed(float secondsElapsed);	    // Jump to specific time
	void SetFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
	void advanceToStartOfNextFrame();
	void setToStartOfFrame(int frameNumber);
private:
	float m_elapsedSeconds;
	float m_durationSeconds;
	bool m_isPlaying;
	bool m_isFinished;
	int m_startSpriteIndex;
	int m_endSpriteIndex;
	float m_framesPerSecond;
	SpriteAnimMode m_animMode;
	SpriteSheet m_spriteSheet;
};



