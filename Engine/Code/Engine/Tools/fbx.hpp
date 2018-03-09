#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class MeshBuilder;
class Skeleton;
class Motion;
//////////////////////////////////////////////////////////////////////////
#if !defined( __TOOLS_FBXLOAD__ )
#define __TOOLS_FBXLOAD__

void FbxListFile(char const* filename);
bool FbxLoadMesh(MeshBuilder* mb, const char* filename,const Skeleton* skel = nullptr);
bool FbxLoadSkeleton(Skeleton* skel, char const *filename);
bool FbxLoadMotion(Motion* motion, const Skeleton* skeleton, const char*filename, uint framerate = 10);
#endif
