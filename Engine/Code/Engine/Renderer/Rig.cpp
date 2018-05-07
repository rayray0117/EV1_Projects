#include "Engine/Renderer/Rig.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/SkeletonInstance.hpp"
#include "Engine/Tools/fbx.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/BinaryStream.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Motion.hpp"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Rig::Rig()
{
	m_skeleton = new Skeleton();
	m_skelInst = new SkeletonInstance();
	m_skelInst->mo_skeleton = m_skeleton;
}

Rig::~Rig()
{
	SAFE_DELETE(m_mesh);
	SAFE_DELETE(m_skeleton);
	SAFE_DELETE(m_debug_skeleton);
	SAFE_DELETE(m_skelInst);
}

void Rig::LoadSkeletonMesh(const std::string& path)
{
	LoadSkeleton_Asset(path);
	LoadMesh_Asset(path);
}

//////////////////////////////////////////////////////////////////////////
void Rig::LoadMesh_FBX(const std::string& fbxPath)
{
#if defined(TOOLS_BUILD) 
	FileBinaryStream g_stream;
	MeshBuilder* mb = new MeshBuilder();
	FbxLoadMesh(mb, fbxPath.c_str(), m_skeleton);
	SAFE_DELETE(m_mesh);

	m_mesh = g_mainEngineRenderer->CreateAndGetMeshStatic(mb);

	std::vector<Matrix4> matr;
	for (uint i = 0; i < m_skeleton->m_joints.size(); i++)
		matr.push_back(m_skeleton->m_joints[i].global_transform);

	m_skelInst->SetStructedBuffer(g_mainEngineRenderer->CreateAndGetStructedBuffer(matr));
	g_mainEngineRenderer->BindShader(m_skinShader);
	g_mainEngineRenderer->SetStructuredBuffer(m_skelInst->m_skinTransfroms);
	//*/
	std::string filePath = fbxPath;
	filePath.pop_back(); //x
	filePath.pop_back(); //b
	filePath.pop_back(); //f

	g_stream.open_for_write(std::string(filePath + "mesh").c_str());
	mb->write(g_stream);
	g_stream.close();
	//*/
	SAFE_DELETE(mb);
#else
	UNUSED(fbxPath);
	ERROR_RECOVERABLE(C_STRF("NOT IN TOOLS_BUILD ERROR! %s was not loaded. Check file path or build a .mesh in a TOOLS_BUILD!"));
#endif
}

void Rig::LoadMesh_Asset(const std::string& path)
{
	FileBinaryStream g_stream;
	if (!g_stream.open_for_read(std::string(path + ".mesh").c_str()))
	{
		LoadMesh_FBX(path + ".fbx");
		return;
	}

	MeshBuilder* mb = new MeshBuilder();
	mb->read(g_stream);
	g_stream.close();
	SAFE_DELETE(m_mesh);
	m_mesh = g_mainEngineRenderer->CreateAndGetMeshStatic(mb);

	
	std::vector<Matrix4> matr;
	for (uint i = 0; i < m_skeleton->m_joints.size(); i++)
		matr.push_back(m_skeleton->m_joints[i].global_transform);

	m_skelInst->SetStructedBuffer(g_mainEngineRenderer->CreateAndGetStructedBuffer(matr));
	g_mainEngineRenderer->BindShader(m_skinShader);
	g_mainEngineRenderer->SetStructuredBuffer(m_skelInst->m_skinTransfroms);

	SAFE_DELETE(mb);
}

void Rig::LoadSkeleton_FBX(const std::string& fbxPath)
{
#if defined(TOOLS_BUILD)
	FileBinaryStream g_stream;
	m_skeleton->clear();
	FbxLoadSkeleton(m_skeleton, fbxPath.c_str());

	SAFE_DELETE(m_debug_skeleton);
	m_debug_skeleton = g_mainEngineRenderer->CreateAndGetSkeletonMesh(m_skeleton);
	//*/
	std::string filePath = fbxPath;
	filePath.pop_back(); //x
	filePath.pop_back(); //b
	filePath.pop_back(); //f

	g_stream.open_for_write(std::string(filePath + "skel").c_str());
	m_skeleton->write(g_stream);
	g_stream.close();
#else
	UNUSED(fbxPath);
	ERROR_RECOVERABLE(C_STRF("NOT IN TOOLS_BUILD ERROR! %s was not loaded. Check file path or build a .skel in a TOOLS_BUILD!"));
#endif
}

void Rig::LoadSkeleton_Asset(const std::string& path)
{
	FileBinaryStream g_stream;
	if (!g_stream.open_for_read(std::string(path + ".skel").c_str()))
	{
		LoadSkeleton_FBX(path + ".fbx");
		return;
	}

	m_skeleton->read(g_stream);
	g_stream.close();

	SAFE_DELETE(m_debug_skeleton);
	m_debug_skeleton = g_mainEngineRenderer->CreateAndGetSkeletonMesh(m_skeleton);
}

Motion* Rig::LoadMotion_FBX(const std::string& fbxPath, uint framerate)
{
#if defined(TOOLS_BUILD)
	FileBinaryStream g_stream;
	Motion* motion = new Motion();
	FbxLoadMotion(motion, m_skeleton, fbxPath.c_str(), framerate);


	std::string filePath = fbxPath;
	filePath.pop_back(); //x
	filePath.pop_back(); //b
	filePath.pop_back(); //f
	g_stream.open_for_write(std::string(filePath + "motion").c_str());
	motion->write(g_stream);
	g_stream.close();

	return motion;
#else
	UNUSED(fbxPath);
	UNUSED(framerate);
	ERROR_RECOVERABLE(C_STRF("NOT IN TOOLS_BUILD ERROR! %s was not loaded. Check file path or build a .motion in a TOOLS_BUILD!"));
	return nullptr;
#endif
}

Motion* Rig::LoadMotion_Asset(const std::string& path, uint framerate)
{
	FileBinaryStream g_stream;
	if (!g_stream.open_for_read(std::string(path + ".motion").c_str()))
	{
		return LoadMotion_FBX(path + ".fbx", framerate);
	}

	Motion* motion = new Motion();
	motion->read(g_stream);
	g_stream.close();

	return motion;
}

