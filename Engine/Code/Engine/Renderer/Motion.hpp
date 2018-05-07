#pragma once
#include "Engine/Renderer/Pose.hpp"
#include <vector>
#include <string>
//////////////////////////////////////////////////////////////////////////
class BinaryStream;
//////////////////////////////////////////////////////////////////////////
class Motion
{
public:
	enum EPlaybackMode
	{
		PLAY_ONCE,
		PLAY_LOOP,
		NUM_PLAY_MODES
	};
public:
	Motion(): m_framerate(30.f) {};
	~Motion() {};
	
	// Make sure poses is exactly long enough to encompass this time.
	float setDuration(float time);

	// framerate is 10hz, that means the time between two frames takes 1/10th of a second.
	// therefore if we have 3 frames in this motion, the total duration would be (3 - 1) * 0.1,
	// or 0.2 seconds long.  
	float get_duration() const { return (m_poses.size() - 1) / m_framerate; }
	uint get_frame_count() const { return m_poses.size(); }

	// This will fill out a pose with the evaluated pose at time
	// left as an excercise...
	void evaluate(Pose *out, float time) const;
	void evaluate(Pose *out, uint frame) const;
	Pose get_pose(uint frame_index) const;
	Pose* get_pose(uint frame_index);


	void write(BinaryStream &stream);
	void read(BinaryStream &stream);
public:
	std::string m_name;
	float m_framerate;
	std::vector<Pose> m_poses;
	EPlaybackMode m_mode = PLAY_LOOP;
};