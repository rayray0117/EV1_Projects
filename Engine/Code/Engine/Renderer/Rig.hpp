#pragma once
#include "Engine/Core/EngineBase.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
class Mesh;
class Skeleton;
class SkeletonInstance;
class Motion;
//////////////////////////////////////////////////////////////////////////
class Rig
{
public:
	Rig();
	~Rig();
	Mesh* m_mesh = nullptr;
	Skeleton* m_skeleton = nullptr;
	Mesh* m_debug_skeleton = nullptr;
	SkeletonInstance* m_skelInst = nullptr;
	std::string m_skinShader = "Data/Shaders/skinning1.hlsl";

	void LoadSkeletonMesh(const std::string& path);

	void LoadMesh_FBX(const std::string& fbxPath); //Must have .fbx at the end of it!
	void LoadMesh_Asset(const std::string& path);

	void LoadSkeleton_FBX(const std::string& fbxPath); //Must have .fbx at the end of it!
	void LoadSkeleton_Asset(const std::string& path);
	
	Motion* LoadMotion_FBX(const std::string& fbxPath, uint framerate = 10); //Must have .fbx at the end of it!
	Motion* LoadMotion_Asset(const std::string& fbxPath, uint framerate = 10); //Framerate variable is only for the case where the motion file was never made.
	
	
	const bool isValid() const
	{
		return m_mesh != nullptr && m_skeleton != nullptr && m_debug_skeleton != nullptr && m_skelInst != nullptr;
	}
};