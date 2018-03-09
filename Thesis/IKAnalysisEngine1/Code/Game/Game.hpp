#pragma once
#include <vector>
#include "Game/Camera3D.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/BinaryStream.hpp"

#include "Game/DebugEndEffector.hpp"
//////////////////////////////////////////////////////////////////////////
class Sampler;
class VertexBuffer;
class Mesh;
class MeshBuilder;
class Skeleton;
class SkeletonInstance;
class Motion;
class IKRig;
class IKSolver;
class Character;
//////////////////////////////////////////////////////////////////////////
extern DebugEndEffector* g_end2;
//////////////////////////////////////////////////////////////////////////
enum ESolvers
{
	SOLVER_FABRIK,
	SOLVER_CCD,
	SOLVER_JT,
	SOLVER_CCD_CON,
	SOLVER_TWO_BONE,
	NUM_SOLVERS
};

//////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	~Game();
	void Update(float deltaSeconds);

	const Vector3 lightPos(float t) const
	{
		return Vector3(2.0f * cosf(t), 0.0f, -2.f * sinf(t));
	}

	void Render() const;

	void drawRig() const;

	void drawLight(const Vector3& position, const Rgba& color, uint lightNum) const;

	//void LoadSkeletonMesh_FBX(const std::string& fbxPath);
	//void LoadSkeleton_FBX(const std::string& fbxPath);
	//void LoadMesh_FBX(const std::string& fbxPath);
	//void LoadMotion_FBX(const std::string& fbxPath);
	//
	//void LoadSkeletonMesh_Asset(const std::string& fbxPath);
	//void LoadSkeleton_Asset(const std::string& path);
	//void LoadMesh_Asset(const std::string& path);
	//
	//void LoadMotion_Asset(const std::string& path);

	void SwitchMotion();

	void SetUpIKMotionTest();
	void SetUpDataCollector();
	void LoadModel();
public:
	bool m_debugMode = false;
	bool m_drawSkeleton = false;
	int m_modelToLoadNumber = 0;

	Camera3D m_spectatorCamera;
	std::vector<std::string> shaderList;
	FileBinaryStream m_stream;

	Character* m_character = nullptr;

	int m_shaderIndex = 0;

	float m_ambientIntensity = .2f;

	DebugEndEffector m_debugEndEffector;
	IKSolver* m_solver = nullptr;
	std::string m_solverStr = "NONE";
private:
	void HandleInput();

	
	void ResetSolver();

	void SetSolverTo(ESolvers type);

	void UpdateCheats(float& deltaSeconds);
	void UpdateNormalMode(float deltaSeconds);
	void UpdateDebugMode(float deltaSeconds);
	void UpdateTimeBuffer(float deltaSeconds);

	void drawIKInfo() const;

	//Spectator Camera
	void UpdateCameraControls(float deltaSeconds);
	void UpdateCameraFromKeyboard(float deltaSeconds);
	void UpdateCameraFromMouse(float deltaSeconds);
	
	void SetUpPerspective() const;
	void SetUpCameraView() const;

	void drawDebugText() const;

	void loadConfigTexturesAndShaders();
	void loadGameConfigStuff();

	void loadMeshs();
private:
	bool m_gamePaused;

	std::vector<std::string> textureList;
	

	int currentTexture;
	int currentShader;

	Vector3 m_lightPos;
	
};