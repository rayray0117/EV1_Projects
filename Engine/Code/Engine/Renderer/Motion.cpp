#include "Engine/Renderer/Motion.hpp"
#include <xtgmath.h>
#include "Engine/Core/BinaryStream.hpp"
#include "../Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
float Motion::setDuration(float time)
{
	uint numPoses = (uint)std::ceil(m_framerate * time) + 1;

	m_poses.resize(numPoses);

	return (float)numPoses;
}

void Motion::evaluate(Pose* out, float time) const
{
	float frame = m_framerate * time;
	/*/
	if (frame >= get_frame_count() - 1)
	{
		uint frameIndex = get_frame_count() - 1;
		*out = m_poses[frameIndex];
		return;
	}
	//*/
	int beginFrame = (int)floor(frame);
	int endFrame = beginFrame + 1;
	float fracToEnd = frame - (float)beginFrame;

	*out = interpolate(get_pose(beginFrame), get_pose(endFrame), fracToEnd);
}

Pose Motion::get_pose(uint frame_index) const
{
	if (frame_index >= get_frame_count() -1)
	{
		switch (m_mode)
		{
		case Motion::PLAY_ONCE: frame_index = get_frame_count() - 1;
			break;
		case Motion::PLAY_LOOP: frame_index = frame_index % (get_frame_count() - 1); //The -1 prevents doubled up frames for the end to beginning.
			break;
		case Motion::NUM_PLAY_MODES:
		default: ERROR_AND_DIE(Stringf("Playback Mode not set for motion %s", m_name.c_str()).c_str());
			break;
		}
	}

	return m_poses[frame_index];
}

Pose* Motion::get_pose(uint frame_index)
{
	if (frame_index >= get_frame_count() - 1)
	{
		switch (m_mode)
		{
		case Motion::PLAY_ONCE: frame_index = get_frame_count() - 1;
			break;
		case Motion::PLAY_LOOP: frame_index = frame_index % (get_frame_count() - 1); //The -1 prevents doubled up frames for the end to beginning.
			break;
		case Motion::NUM_PLAY_MODES:
		default: ERROR_AND_DIE(Stringf("Playback Mode not set for motion %s", m_name.c_str()).c_str());
			break;
		}
	}

	return &m_poses[frame_index];
	
}
//////////////////////////////////////////////////////////////////////////
void Motion::write(BinaryStream &stream)
{
	stream.write(m_name);
	stream.write(m_framerate);
	stream.write(m_poses.size());
	for each (Pose var in m_poses)
	{
		stream.write(var);
	}
}

void Motion::read(BinaryStream &stream)
{
	m_poses.clear();
	stream.read(m_name);
	stream.read(m_framerate);
	uint poseSize;
	stream.read(poseSize);
	m_poses.reserve(poseSize);
	for(uint i = 0; i < poseSize; i++)
	{
		Pose var;
		stream.read(var);
		m_poses.push_back(var);
	}
}
