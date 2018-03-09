#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Tools/fbx.hpp"

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "Engine/Renderer/SkeletonInstance.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"
#include "Engine/IK/IKRig.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/IK/FABRIKSolver.hpp"
#include "Engine/IK/CCDSolver.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/IK/Jacobian5JointSolver.hpp"
#include "Game/DataCollector.hpp"
#include "Engine/General/UniversalFunction.hpp"
//////////////////////////////////////////////////////////////////////////
#include "Engine/IK/IKMotion.hpp"
#include "Engine/IK/CCDConstrainedSolver.hpp"
#include "Engine/IK/IKConstraint.hpp"
#include "Game/Character.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/IK/TwoBoneSolver.hpp"
IKMotion* g_ikMotion = nullptr;
float g_motionTimeMod = 1.f;
DebugEndEffector* g_end2 = nullptr;
//////////////////////////////////////////////////////////////////////////
MeshBuilder* g_mb = nullptr;
float cameraLightIntensity = .25f;
float cameraLightAngle = 30;
Vector3 g_direction = -Vector3::YAXIS;
std::string motionText = "None";
DataCollector* g_dataCollecter = nullptr;
//////////////////////////////////////////////////////////////////////////
COMMAND(FOV, "newFovDegrees; Changes the fov for the game.")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	float newFOVDegrees = std::stof(args[0]);
	g_fovVerticalDegrees = newFOVDegrees;
}

COMMAND(CAMLIGHT, "angle, intesity")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	float newFOVDegrees = std::stof(args[0]);
	cameraLightAngle = newFOVDegrees;

	newFOVDegrees = std::stof(args[1]);
	cameraLightIntensity = newFOVDegrees;
}
COMMAND(wire, "turn wireframe on")
{
	g_renderer->EnableWireframeCulling();
}
COMMAND(solid, "turn wireframe off")
{
	g_renderer->EnableWireframeCulling(false);
}
COMMAND(cam_speed, "Change the speed of the camera")
{
	CAMERA_MOVEMENT_SPEED = stof(arguments);
}
COMMAND(ambient, "Float value for ambient intensity")
{
	g_theGame->m_ambientIntensity = stof(arguments);
	g_renderer->SetAmbientLight(g_theGame->m_ambientIntensity, Rgba::WHITE);
}
//////////////////////////////////////////////////////////////////////////
Game::Game()
	: m_gamePaused(false)
{
	loadConfigTexturesAndShaders();
	loadGameConfigStuff();
	
	g_renderer->timeData.game_frame_time = 0.f;
	g_renderer->timeData.game_time = 0.f;

	g_renderer->CreateOrGetFont("Data/Fonts/Garamound32.fnt");
	g_renderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	g_renderer->SetSampler(RHIEnum::FILTER_POINT, RHIEnum::FILTER_POINT);
	
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);


	g_renderer->SetAmbientLight(m_ambientIntensity, Rgba::WHITE);
	g_renderer->SetSpecularConstants(42.0f, 1.0f);
	g_renderer->EnableDirectionalLight(g_direction, Rgba::WHITE, 0);
	m_solver = new FABRIKSolver();
	m_solverStr = "FABRIK";

	g_end2 = new DebugEndEffector(Rgba::ORANGE);

//	SetUpDataCollector();
	LoadModel();
}

Game::~Game() 
{
	SAFE_DELETE(g_mb);
	SAFE_DELETE(m_solver);
	SAFE_DELETE(g_ikMotion);
	SAFE_DELETE(g_end2);
	SAFE_DELETE(m_character);
}

//////////////////////////////////////////////////////////////////////////
void Game::Update(float deltaSeconds)
{

	UpdateCheats(deltaSeconds);
	HandleInput();
	if (m_debugMode)
	{
		UpdateDebugMode(deltaSeconds);
	}
	else
	{
		UpdateNormalMode(deltaSeconds);
	}
	UpdateCameraControls(deltaSeconds);

	UpdateTimeBuffer(deltaSeconds);

	if (m_gamePaused)
		return;

	if(m_character)
		m_character->Update(deltaSeconds * g_motionTimeMod);
}

void Game::Render() const
{
	g_renderer->SetRenderTarget(nullptr, nullptr);
	g_renderer->ClearScreen(Rgba::GRAY);
	g_renderer->ClearDepth();
	g_renderer->EnableDepth(true);
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	
	g_renderer->SetViewport(0, 0, g_windowWidth, g_windowHeight);
	g_renderer->EnableBackFaceCulling(true);
	SetUpPerspective();
	SetUpCameraView();
	g_renderer->drawAxes(1000.f);


	//*/
	g_renderer->BindTexture("White");
	g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
	g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
	g_renderer->BindTexture("White", 3);
	//*/
	
	g_renderer->EnableSpotLight(m_spectatorCamera.m_position, m_spectatorCamera.getForwardXZY(), cameraLightAngle, Rgba::WHITE, cameraLightIntensity, Vector3::ZAXIS);


	g_renderer->EnableDepthTesting(true);
	g_renderer->EnableBackFaceCulling(true);
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);

	drawRig();

	if(m_debugMode)
		Profiler::GetInstance()->Render(g_renderer);

	drawIKInfo();
}
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////

void Game::drawRig() const
{
	if(m_character)
		m_character->Render();
}

void Game::drawLight(const Vector3& position, const Rgba& color, uint lightNum) const
{
	g_renderer->pushMatrix();
	g_renderer->EnablePointLight(position, color, .75f, Vector3::ZAXIS, Vector3::ZAXIS, lightNum);
 	g_renderer->drawLine(position - Vector3::ZAXIS*.5, position + Vector3::ZAXIS*.5, Rgba::WHITE, Rgba::WHITE);
 	g_renderer->drawLine(position - Vector3::XAXIS*.5, position + Vector3::XAXIS*.5, Rgba::WHITE, Rgba::WHITE);
 	g_renderer->drawLine(position - Vector3::YAXIS*.5, position + Vector3::YAXIS*.5, Rgba::WHITE, Rgba::WHITE);
	g_renderer->SetMatrixMode(RHIEnum::MATRIX_MODEL);
	g_renderer->BindTexture("White");
	g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
	g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
	g_renderer->BindTexture("White", 3);
	g_renderer->translate3D(position);
	g_renderer->scale3DUniform(-.125f);
	g_renderer->popMatrix();
	g_renderer->SetMatrixMode(RHIEnum::MATRIX_PROJECTION);
}
//////////////////////////////////////////////////////////////////////////
void Game::SwitchMotion()
{
	++m_character->m_motionIndex;
	if (!m_character->m_useIKMotion)
	{
		if (m_character->m_motionIndex >= m_character->m_motions.size())
		{
			m_character->m_motionIndex = 0;
		}

		motionText = m_character->m_motions[m_character->m_motionIndex]->m_name;
	}
	else
	{
		if (m_character->m_motionIndex > m_character->m_ikMotions.size())
		{
			m_character->m_motionIndex = 0;
		}
		if (m_character->m_motionIndex == m_character->m_ikMotions.size())
		{
			motionText = "FORCEIK";
		}
		else
		{
			motionText = "IK " + m_character->m_ikMotions[m_character->m_motionIndex]->m_name;
		}
	}
}

void Game::SetUpIKMotionTest()
{
	SAFE_DELETE(g_ikMotion);
	g_ikMotion = new IKMotion(m_character->m_motions[2], m_character->m_ikRig);
	/*/
	g_ikMotion->SetDuration(5.f);
	
	for (uint i = 0; i < 6; ++i)
	{
		IKChainPose poseLeg;
		poseLeg.m_chainName = "r_leg";

		IKPose pose;
		pose.m_chainPoses.push_back(poseLeg);

		g_ikMotion->m_ikPoses[i] = pose;
	}

	g_ikMotion->m_ikPoses[0].m_chainPoses[0].m_goalTransform.position = Vector3(-19.25, 24.5, 10.25);
	g_ikMotion->m_ikPoses[1].m_chainPoses[0].m_goalTransform.position = Vector3(-6, 24.5, -16.25);
	g_ikMotion->m_ikPoses[2].m_chainPoses[0].m_goalTransform.position = Vector3(-10.25, 11, -16.25);
	g_ikMotion->m_ikPoses[3].m_chainPoses[0].m_goalTransform.position = Vector3(-4.75, 36.25, -56.75);
	g_ikMotion->m_ikPoses[4].m_chainPoses[0].m_goalTransform.position = Vector3(8.5, 63.75, -56.75);
	g_ikMotion->m_ikPoses[5].m_chainPoses[0].m_goalTransform.position = Vector3(8.5, 32, 10.75);
	//*/
}



void DataCollectorCall(void* data)
{
	g_dataCollecter->WriteData(data);
}
void Game::SetUpDataCollector()
{
	std::string filename = Stringf("IK_Data_NULL.csv");
	g_dataCollecter = new DataCollector(filename);
	if (g_dataCollecter->myfile.is_open())
		DebuggerPrintf("File is open!");
	else
	{
		DebuggerPrintf("File is NOT open!");

	}
	g_dataCollecter->WriteData(std::string("Solver, Chain, Num Iterations, Max Iterations, Time, Tolerance, Goal, Result, Difference \n"));
	UF->Add("IKData", DataCollectorCall);
}

////////////////////////////////////////////////////////////////////////// PRIVATE //////////////////////////////////////////////////////////////////////////
void Game::HandleInput()
{
#if defined(TOOLS_BUILD) 
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_R))
	{
		//LoadSkeletonMesh_FBX(m_modelPath);
		LoadSkeletonMesh_Asset("Data/Models/FBX/SK_Mannequin");
		LoadMotion_FBX(m_modelPath);

		LoadMotion_FBX("Data/Models/FBX/ThirdPersonIdle.FBX");
		LoadMotion_FBX("Data/Models/FBX/ThirdPersonWalk.FBX");
		LoadMotion_FBX("Data/Models/FBX/ThirdPersonRun.FBX");
		LoadMotion_FBX("Data/Models/FBX/ThirdPersonJump_Start.FBX");
		LoadMotion_FBX("Data/Models/FBX/ThirdPersonJump_Loop.FBX");
		LoadMotion_FBX("Data/Models/FBX/ThirdPersonJump_End.FBX");
	}
#endif // TOOLS_BUILD
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_I))
		SetUpIKMotionTest();
	
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_E))
	{
		LoadModel();
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_Y))
	{
		SwitchMotion();
	}

	/*/ //Find different keys to bind to
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_1))
	{
		m_shaderIndex = 0; //With lighting
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_2))
	{
		m_shaderIndex = 2; //Without lighting
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_3))
	{
		m_shaderIndex = 1; //Shows skin bind values.
	}
	//*/
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_1))
	{
		SetSolverTo(SOLVER_FABRIK);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_2))
	{
		SetSolverTo(SOLVER_CCD);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_3))
	{
		SetSolverTo(SOLVER_JT);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_4))
	{
		SetSolverTo(SOLVER_CCD_CON);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_5))
	{
		SetSolverTo(SOLVER_TWO_BONE);
	}

	if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD1))
	{
		if (m_character->m_useIKMotion && g_inputSystem->isKeyDown(KEYCODE_DECIMAL))
			m_character->m_ikRig->ForceChainsPose("r_arm");
		else
			m_character->m_ikRig->SolveChainToGoal("r_arm", m_debugEndEffector.m_position);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD2))
	{
		if (m_character->m_useIKMotion && g_inputSystem->isKeyDown(KEYCODE_DECIMAL))
			m_character->m_ikRig->ForceChainsPose("l_arm");
		else
			m_character->m_ikRig->SolveChainToGoal("l_arm", m_debugEndEffector.m_position);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD4))
	{
		if (m_character->m_useIKMotion && g_inputSystem->isKeyDown(KEYCODE_DECIMAL))
			m_character->m_ikRig->ForceChainsPose("r_leg");
		else
			m_character->m_ikRig->SolveChainToGoal("r_leg", m_debugEndEffector.m_position);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD5))
	{
		if (m_character->m_useIKMotion && g_inputSystem->isKeyDown(KEYCODE_DECIMAL))
			m_character->m_ikRig->ForceChainsPose("l_leg");
		else
			m_character->m_ikRig->SolveChainToGoal("l_leg", m_debugEndEffector.m_position);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD0))
	{
		if (m_character->m_useIKMotion&& g_inputSystem->isKeyDown(KEYCODE_DECIMAL))
			m_character->m_ikRig->ForceChainsPose("spine");
		else
			m_character->m_ikRig->SolveChainToGoal("spine", m_debugEndEffector.m_position);
		
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_MULTIPLY))
	{
		//m_ikRig->ForcePose();
		m_character->m_useIKMotion = !m_character->m_useIKMotion;
		if (m_character->m_useIKMotion)
		{
			m_character->m_motionIndex = m_character->m_ikMotions.size();
			motionText = "FORCE IK";
		}
		else
		{
			//Reset to motion 0
			m_character->m_motionIndex = m_character->m_motions.size();
			SwitchMotion();
		}
	}
}

void Game::LoadModel()
{
	XMLNode root = XMLNode::openFileHelper("Data/Config/ModelConfig.xml");
	XMLNode childElement = root.getChildNode("Model", m_modelToLoadNumber);
	SAFE_DELETE(m_character);
	m_character = new Character(childElement);
	ResetSolver();
}

void Game::ResetSolver()
{
	m_character->SetSolverForAll(m_solver);
}

void Game::SetSolverTo(ESolvers type)
{
	SAFE_DELETE(m_solver);

	switch (type)
	{
	case SOLVER_FABRIK: m_solver = new FABRIKSolver(); m_solverStr = "FABRIK";
		break;
	case SOLVER_CCD: m_solver = new CCDSolver();  m_solverStr = "CCD";
		break;
	case SOLVER_JT: m_solver = new Jacobian5JointSolver();  m_solverStr = "Jacobian Transpose";
		break;
	case SOLVER_CCD_CON: m_solver = new CCDConstrainedSolver(); m_solverStr = "CCD Constrained";
		break;
	case SOLVER_TWO_BONE: m_solver = new TwoBoneSolver(); m_solverStr = "Two Bone Analytics";
		break;
	case NUM_SOLVERS:
		break;
	default:
		break;
	}
	ResetSolver();
}

void Game::UpdateCheats(float& deltaSeconds)
{
	if (g_inputSystem->isKeyDown('T'))
	{
		deltaSeconds *= 0.1f;
		if (g_inputSystem->isKeyDown(KEYCODE_SHIFT))
			deltaSeconds *= 0.1f;
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_PAUSE))
		m_gamePaused = !m_gamePaused;
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_F1))
		m_debugMode = !m_debugMode;

	if (g_inputSystem->wasKeyJustPressed('O'))
	{
		m_spectatorCamera.m_position = Vector3::ZERO;
		m_spectatorCamera.m_degreesAboutX = 0.f;
		m_spectatorCamera.m_degreesAboutY = 0.f;
		m_spectatorCamera.m_degreesAboutZ = 0.f;
	}

	if (g_inputSystem->wasKeyJustPressed('F'))
	{
		m_spectatorCamera.m_isFlying = !m_spectatorCamera.m_isFlying;
	}
}

void Game::UpdateNormalMode(float deltaSeconds)
{

}

void Game::UpdateDebugMode(float deltaSeconds)
{
	m_debugEndEffector.Update(deltaSeconds);
}

void Game::UpdateTimeBuffer(float deltaSeconds)
{
	g_renderer->timeData.game_frame_time = deltaSeconds;
	g_renderer->timeData.game_time += deltaSeconds;
	g_renderer->time_cb->update(g_renderer->context, &g_renderer->timeData);
}

void Game::drawIKInfo() const
{
	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2(80, 45));
	g_renderer->ClearDepth();
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_renderer->BindTexture("White");
	g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
	g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
	g_renderer->BindTexture("White", 3);
	
	std::string goalPosStr = Stringf("{ %f x, %f y, %f z }", m_debugEndEffector.m_position.x, m_debugEndEffector.m_position.y, m_debugEndEffector.m_position.z);
	uint maxIterStr = m_solver->getNumIterations();
	std::string debugInfo = Stringf("Goal Pos: %s, Max Iter: %i, Solver: %s, Motion: %s", goalPosStr.c_str(), maxIterStr, m_solverStr.c_str(), motionText.c_str());
	g_renderer->DrawText2D(Vector2::ZERO, debugInfo, 1.f, Rgba::LIME, .5);
}

//////////////////////////////////////////////////////////////////////////
void Game::UpdateCameraControls(float deltaSeconds)
{
	UpdateCameraFromKeyboard(deltaSeconds);

	UpdateCameraFromMouse(deltaSeconds);
}

void Game::UpdateCameraFromKeyboard(float deltaSeconds)
{
	bool isSprinting = g_inputSystem->isKeyDown(KEYCODE_SHIFT);

	if (g_inputSystem->isKeyDown('W'))
		m_spectatorCamera.moveForward(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('S'))
		m_spectatorCamera.moveBackward(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('A'))
		m_spectatorCamera.moveLeft(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('D'))
		m_spectatorCamera.moveRight(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown(KEYCODE_SPACE))
		m_spectatorCamera.moveUp(deltaSeconds, isSprinting);//m_position += CAMERA_MOVEMENT_SPEED * deltaSeconds * Vector3::ZAXIS;
	if (g_inputSystem->isKeyDown(KEYCODE_CONTROL))
		m_spectatorCamera.moveDown(deltaSeconds, isSprinting); //m_position -= CAMERA_MOVEMENT_SPEED * deltaSeconds * Vector3::ZAXIS;
	if (g_inputSystem->wasKeyJustPressed('U'))
		m_character->m_motionTime = 0.f;
}

void Game::UpdateCameraFromMouse(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (!g_theApp->m_hasFocus)
		return;
	IntVector2 screenCenter = g_inputSystem->GetScreenCenter();
	IntVector2 mouseScreenPos = g_inputSystem->GetCursorScreenPos();
	IntVector2 mouseDeltaMove = mouseScreenPos - screenCenter;
	g_inputSystem->SetCursorScreenPos(screenCenter);

	float mouseMoveX = (float)mouseDeltaMove.x;
	m_spectatorCamera.rotateLeft(1.f, mouseMoveX*MOUSE_SENSITIVITY_X);
	float mouseMoveY = (float)mouseDeltaMove.y;
	m_spectatorCamera.rotateUp(1.f,mouseMoveY * MOUSE_SENSITIVITY_Y);
}
//////////////////////////////////////////////////////////////////////////
void Game::SetUpPerspective() const
{
	g_renderer->SetPerspectiveProjection(g_fovVerticalDegrees, getAspectRatio(), g_nearClip, g_farClip);
}

void Game::SetUpCameraView() const
{
	g_renderer->rotate3D(-m_spectatorCamera.m_degreesAboutX, Vector3::XAXIS);
	g_renderer->rotate3D(-m_spectatorCamera.m_degreesAboutY, Vector3::YAXIS);
	g_renderer->rotate3D(-m_spectatorCamera.m_degreesAboutZ, Vector3::ZAXIS);

	g_renderer->translate3D(m_spectatorCamera.m_position* -1.f);

	g_renderer->SetEyePosition(m_spectatorCamera.m_position);
}

void Game::drawDebugText() const
{
	g_renderer->EnableBackFaceCulling(false);
	g_renderer->EnableDepthTesting(false);
	g_renderer->SetMatrixMode(RHIEnum::MATRIX_PROJECTION);
	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2(1000 * getAspectRatio(), 1000));

	const Vector3& camPos = m_spectatorCamera.m_position;
	std::string cameraPosText = Stringf("Camera Position: %f, %f, %f", camPos.x, camPos.y, camPos.z);
	std::string cameraRotationText = Stringf("Rotation: %f, %f, %f", m_spectatorCamera.m_degreesAboutX, m_spectatorCamera.m_degreesAboutY, m_spectatorCamera.m_degreesAboutZ);
	Vector3 camForward = m_spectatorCamera.getForwardXZY();
	std::string cameraForwardText = Stringf("Forward: %f, %f, %f", camForward.x, camForward.y, camForward.z);


	float textHeight = 20.f;
	float textAspectRatio = inverse_f(getAspectRatio());
	g_renderer->DrawText2D(Vector2(1.f, 100.f), cameraPosText, textHeight, Rgba::WHITE, textAspectRatio, g_renderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	g_renderer->DrawText2D(Vector2(1.f, 80.f), cameraRotationText, textHeight, Rgba::WHITE, textAspectRatio, g_renderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	g_renderer->DrawText2D(Vector2(1.f, 60.f), cameraForwardText, textHeight, Rgba::WHITE, textAspectRatio, g_renderer->CreateOrGetBitmapFont("SquirrelFixedFont"));


	g_renderer->EnableBackFaceCulling(true);
	g_renderer->EnableDepthTesting(true);
	g_renderer->SetMatrixMode(RHIEnum::MATRIX_PROJECTION);
}
//////////////////////////////////////////////////////////////////////////,
void Game::loadConfigTexturesAndShaders()
{
	// Load in textures
	for (int textureIndex = 0; textureIndex < g_numImages; ++textureIndex)
	{
		std::string textureFilePath;
		std::string image_key = IMAGE_KEY + std::to_string(textureIndex);

		bool success = g_gameConfig->ConfigGetString(image_key.c_str(), textureFilePath);
		ASSERT_RECOVERABLE(success, Stringf("WARNING: Texture #%i was not found!", textureIndex).c_str());
		if (success)
			textureList.push_back(textureFilePath);


		g_renderer->CreateOrGetTexture(textureFilePath);
	}

	// Load in shaders
	for (int shaderIndex = 0; shaderIndex < g_numShaders; ++shaderIndex)
	{
		std::string shaderFilePath;
		std::string shader_key = SHADER_KEY + std::to_string(shaderIndex);

		bool success = g_gameConfig->ConfigGetString(shader_key.c_str(), shaderFilePath);
		ASSERT_RECOVERABLE(success, Stringf("WARNING: Shader #%i was not found!", shaderIndex).c_str());
		if (success)
			shaderList.push_back(shaderFilePath);

		g_renderer->CreateOrGetShader(shaderFilePath);
	}
}

void Game::loadGameConfigStuff()
{
	g_gameConfig->ConfigGetFloat(CAM_SPEED_KEY, CAMERA_MOVEMENT_SPEED);
	g_gameConfig->ConfigGetFloat(DEBUG_END_SPEED_KEY, m_debugEndEffector.m_movementSpeed);
	g_gameConfig->ConfigGetInt(MODEL_KEY, m_modelToLoadNumber);
}
////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
COMMAND(end_effector_speed, "Set's the movement speed for the debug end effector.")
{
	g_theGame->m_debugEndEffector.m_movementSpeed = stof(arguments);
}

COMMAND(ik_num_iterations, "Set the number of iterations. If no argument, defaults to 1.")
{
	if (arguments.empty())
	{
		g_theGame->m_solver->SetNumIterations(1);
	}
	else
	{
		int num = stoi(arguments);
		g_theGame->m_solver->SetNumIterations(num);
	}
}

COMMAND(draw_skeleton, "Toggles drawing the rigs skeleton")
{
	g_theGame->m_drawSkeleton = !g_theGame->m_drawSkeleton;
}

COMMAND(game_ik_anim_mode, "")
{
	if (g_ikMotion->m_mode == IKMotion::PLAY_LOOP)
		g_ikMotion->m_mode = IKMotion::PLAY_ONCE;
	else
	{
		g_ikMotion->m_mode = IKMotion::PLAY_LOOP;
	}
}

COMMAND(game_motion_time_mod, "Float value for modifying animation speed. Good for slowing or speeding anims.!")
{
	if (arguments == "")
		g_motionTimeMod = 1.f;
	else
		g_motionTimeMod = stof(arguments);
}

COMMAND(load_model, "int between 0 and Number - 1 of models in ModelConfig.xml. Defaults to 0 if no number specified.")
{
	if (arguments == "")
		g_theGame->m_modelToLoadNumber = 0;
	else
		g_theGame->m_modelToLoadNumber = stoi(arguments);

	g_theGame->LoadModel();
}

COMMAND(FBXLIST, "Testing my fbx")
{
	FbxListFile(arguments.c_str());
}