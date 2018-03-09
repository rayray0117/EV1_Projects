#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnim::SpriteAnim(const SpriteSheet& spriteSheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimMode animMode /*= SPRITE_ANIM_MODE_PLAY_TO_END*/)
	: m_elapsedSeconds(0.f)
	, m_durationSeconds(durationSeconds)
	, m_isPlaying(false)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_framesPerSecond((float) (1 + endSpriteIndex - startSpriteIndex) / durationSeconds)
	, m_animMode(animMode)
	, m_spriteSheet(spriteSheet)
{
}

SpriteAnim::~SpriteAnim()
{
}

void SpriteAnim::Update(float deltaSeconds)
{
	if (!m_isPlaying)
		return;
	m_elapsedSeconds += deltaSeconds;
	if (m_animMode == SPRITE_ANIM_MODE_LOOPING)
	{
		while (m_elapsedSeconds >= m_durationSeconds)
		{
			m_elapsedSeconds -= m_durationSeconds;
		}
	}
	else if (m_animMode == SPRITE_ANIM_MODE_PLAY_TO_END)
	{
		if (m_elapsedSeconds >= m_durationSeconds)
		{
			m_elapsedSeconds = m_durationSeconds;
			m_isPlaying = false;
			m_isFinished = true;
		}
	}
}

int SpriteAnim::getCurrentSpriteIndex() const
{
	return m_startSpriteIndex + getCurrentFrameNumber();
}

int SpriteAnim::getCurrentFrameNumber() const
{
	return (int)(m_elapsedSeconds * m_framesPerSecond);
}

int SpriteAnim::getNumFrames() const
{
	return m_endSpriteIndex - m_startSpriteIndex;
}

AABB2 SpriteAnim::GetCurrentTexCoords() const
{
	return m_spriteSheet.getTexCoordsForSpriteIndex(getCurrentSpriteIndex());
}

const Texture& SpriteAnim::GetTexture() const
{
	return m_spriteSheet.getTexture();
}

const Texture2D& SpriteAnim::GetTexture2D() const
{
	return m_spriteSheet.getTextureDX();
}

void SpriteAnim::Pause()
{
	m_isPlaying = false;
}

void SpriteAnim::Resume()
{
	m_isPlaying = true;
}

void SpriteAnim::Reset()
{
	m_isPlaying = true;
	m_isFinished = false;
	m_elapsedSeconds = 0.f;
}

float SpriteAnim::GetSecondsRemaining() const
{
	float remainingSeconds = m_durationSeconds - m_elapsedSeconds;
	return remainingSeconds;
}

float SpriteAnim::GetFractionElapsed() const
{
	float fractionElapsed = m_elapsedSeconds / m_durationSeconds;
	return fractionElapsed;
}

float SpriteAnim::GetFractionRemaining() const
{
	return 1.f - GetFractionElapsed();
}

void SpriteAnim::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
}

void SpriteAnim::SetFractionElapsed(float fractionElapsed)
{
	m_elapsedSeconds = fractionElapsed * m_durationSeconds;
}

void SpriteAnim::advanceToStartOfNextFrame()
{
	m_elapsedSeconds = (float)(getCurrentFrameNumber() + 1) * m_framesPerSecond;
}

void SpriteAnim::setToStartOfFrame(int frameNumber)
{
	m_elapsedSeconds = (float)frameNumber * m_framesPerSecond;
}
