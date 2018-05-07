#pragma once
#include "Engine/IK/IKPose.hpp"
//////////////////////////////////////////////////////////////////////////
class IKRig;
class Motion;
//////////////////////////////////////////////////////////////////////////
class IKMotion
{
public:
	enum EPlaybackMode
	{
		PLAY_ONCE,
		PLAY_LOOP,
		NUM_PLAY_MODES
	};
public:
	IKMotion() {}
	IKMotion(Motion* keyFrameMotion, IKRig* rig); //FirstPass may remove!
	~IKMotion() {}
	

	float SetDuration(float time); 
	float getDuration() const { return (m_ikPoses.size() - 1) / m_framerate; }
	uint getFrameCount() const { return m_ikPoses.size(); }

	void Evaluate(IKRig* rig, float time) const;
	void Evaluate(IKRig* rig, uint frame) const;

	void Eval(const IKPose &currentPose, IKRig* rig) const;

	IKPose getIKPose(float time) const;
	IKPose getIKPose(uint frame_index) const;

	uint getFrameIndexNumber(uint frame_index) const;

public:
	std::string m_name;
	float m_framerate = 1.f;
	std::vector<IKPose> m_ikPoses;
	EPlaybackMode m_mode = PLAY_LOOP;
	float m_duration;
private:
	mutable float timeSinceReset = 0.f;
};