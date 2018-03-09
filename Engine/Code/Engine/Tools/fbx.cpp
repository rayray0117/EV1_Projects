#include "Engine/Tools/fbx.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#if defined(TOOLS_BUILD) 
// If we have FBX IMPORTING ENABLED, make sure to include the library and header files.
//#define __PLACEMENT_NEW_INLINE 
#include <fbxsdk.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/Motion.hpp"
#include <algorithm>

#pragma comment(lib, "libfbxsdk-md.lib")

//------------------------------------------------------------------------
// helper struct for loading
struct fbx_skin_weight_t
{
	fbx_skin_weight_t() :
		indices(0,0,0,0),
		weights(0.f,0.f,0.f,0.f)
	{}

	void reset()
	{
		indices = UIntVector4::ZERO;
		weights = Vector4::ZERO;
	}

	UIntVector4 indices;
	Vector4 weights;
};

// Helpers for FbxListFile
FbxScene* FbxLoadScene(char const *filename, bool import_embedded);
void FbxUnloadScene(FbxScene* scene);
void FbxPrintNode(FbxNode* node, int depth);
void FbxListScene(FbxScene* scene);
void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth);
char const* GetAttributeTypeName(FbxNodeAttribute::EType type);
// Helpers for FbxLoadMesh
void ConvertSceneToEngineBasis(FbxScene *scene);
void TriangulateScene(FbxScene *scene);
void FlipX(MeshBuilder *mb);
void ImportMeshes(MeshBuilder *out, FbxNode *node, MatrixStack &mat_stack, const Skeleton* skel);
Matrix4 GetNodeWorldTransform(FbxNode *node);
Matrix4 ToEngineMatrix(FbxMatrix const &fbx_mat);
void ImportMesh(MeshBuilder *out, MatrixStack &mat_stack, FbxMesh *mesh, const Skeleton* skeleton);
Matrix4 GetGeometricTransform(FbxNode *node);
void FbxImportVertex(MeshBuilder *out, Matrix4 const &transform, FbxMesh *mesh, int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t> skin_weights);
bool FbxGetPosition(Vector3 *out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
bool GetNormal(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
bool GetUV(Vector2 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
bool GetTangent(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
bool GetBitangent(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
// Skeleton Functions
void ImportSkeleton(Skeleton *out, FbxNode *node, FbxSkeleton *root_bone, FbxSkeleton *parent_bone, FbxPose *pose);
FbxPose* GetBindPose(FbxScene *scene);
char const* GetBoneName(FbxSkeleton const *skel);
// Motion Functions
float GetNativeFramefrate(FbxScene const *scene);
bool FbxImportMotion(Motion *motion, Skeleton const *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate);
Matrix4 GetNodeWorldTransformAtTime(FbxNode *node, FbxTime time);
// Skinning Functions
void AddHighestWeight(fbx_skin_weight_t& skin_weight, uint32_t bone_idx, float weight);
void CalculateSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights, FbxMesh const *mesh, Skeleton const *skeleton);
bool HasSkinWeights(FbxMesh const *mesh);
void normSkinWeights(Vector4& weights);

void normSkinWeights(Vector4& weights)
{
	float weightTotal = weights.x + weights.y + weights.z + weights.w;
	
	weights.x /= weightTotal;
	weights.y /= weightTotal;
	weights.z /= weightTotal;
	weights.w /= weightTotal;
}

////////////////////////////////////////////////////////////////////////// Public Functions //////////////////////////////////////////////////////////////////////////
void FbxListFile(char const *filename)
{
	FbxScene* scene = FbxLoadScene(filename, false);
	if (nullptr == scene) {
		//WARNING_MSG("Failed to load scene: %s", filename);
		return;
	}

	DebuggerPrintf("Listing File: %s", filename);
	g_theConsole->addMessage(Stringf("Listing File: %s", filename));
	FbxListScene(scene);

	FbxUnloadScene(scene);
}


bool FbxLoadMesh(MeshBuilder* mb, const char* filename,const Skeleton* skel)
{
	// First - load the scene - code is identical to when you 
	// listed the file
	FbxScene *scene = FbxLoadScene(filename, false);
	if (nullptr == scene) {
		return false;
	}

	// Second, we want to convert the basis for consitancy
	ConvertSceneToEngineBasis(scene);

	// Third, our engine only deals with triangles, 
	// but authoring programs tend to deal with a lot of 
	// different surface patch types.  For we triangulate
	// (the process of converting every non-triangle polygon to triangles )
	TriangulateScene(scene);


	// Alright, now we can start importing data, for this we'll need a matrix stack 
	// [well, it helps, you can do this without since we'll mostly be deailing with 
	// global transforms outside of what is called the "geometric transform" which is
	// always local]

	// clear all old data before importing
	mb->clear();

	// My matrix stack treats the top as being IDENTITY
	MatrixStack mat_stack;

	// Next, get the root node, and start importing
	FbxNode *root = scene->GetRootNode();

	// import data into our builder
	ImportMeshes(mb, root, mat_stack, skel);

		// Clean up after ourself
	FbxUnloadScene(scene);


	// [HACK] After loading, I flip everything across the X axis
	// for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
	// but no mattter the transform I sent, it was always flipped on X [at best]
	//
	// Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
	FlipX(mb);
	return true;
}

bool FbxLoadSkeleton(Skeleton *skel, char const *filename)
{
    // Same as you did for Meshes.
    FbxScene *fbx_scene = FbxLoadScene(filename, false);
    if (nullptr == fbx_scene) {
        return false;
    }

    ConvertSceneToEngineBasis(fbx_scene);

    // First, get the bind post for the scene
    skel->clear();


    // Now, traverse the scene, and build the skeleton out with 
    // the bind pose positions
    FbxNode *root = fbx_scene->GetRootNode();

    // Bind pose - the pose which assets are authored for.
    FbxPose *pose = GetBindPose(fbx_scene);

    // Import the skeleton by traversing the scene.
    ImportSkeleton(skel, root, nullptr, nullptr, pose);

    FbxUnloadScene(fbx_scene);

    // Success if I've loaded in at least ONE bone/joint.
    return (skel->getJointCount() > 0U);
}
////////////////////////////////////////////////////////////////////////// Helper Functions //////////////////////////////////////////////////////////////////////////
// Reference:
//    $(FBXSDK_DIR)\samples\Common\Common.cxx
FbxScene* FbxLoadScene(char const *filename, bool import_embedded)
{
	// Manager - think of it as a running process of the FbxSdk - you could only ever load one of these
	// and call it good - though I usually just keep one around per import.
	FbxManager *fbx_manager = FbxManager::Create();
	if (nullptr == fbx_manager) {
		g_theConsole->addMessage("Could not create FBX Manager.", 500.f, Rgba::RED);
		return nullptr;
	}

	// Next, set the IO settings - this is how we configure what we're importing
	// By default, we import everything, but if you wanted to explicitly not import some parts
	// you could do that here)
	FbxIOSettings *io_settings = FbxIOSettings::Create(fbx_manager, IOSROOT);
	io_settings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, import_embedded);

	fbx_manager->SetIOSettings(io_settings);


	// Next, create our importer (since we're loading an FBX, not saving one)
	// Second argument is the name - we don't need to name it.
	FbxImporter *importer = FbxImporter::Create(fbx_manager, "");

	bool result = importer->Initialize(filename,
		-1, // File format, -1 will let the program figure it out
		io_settings);

	if (result) {
		// First, create a scene to be imported into
		// Doesn't need a name, we're not going to do anything with it
		FbxScene *scene = FbxScene::Create(fbx_manager, "");

		// Import into the scene
		result = importer->Import(scene);
		importer->Destroy();
		importer = nullptr;

		if (result) {
			return scene;
		}
	}

	// Failed somewhere, so clean up after ourselves.
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(importer);
	FBX_SAFE_DESTROY(fbx_manager);

	return nullptr;
}

void FbxUnloadScene(FbxScene *scene)
{
	if (nullptr == scene) {
		return;
	}

	FbxManager *manager = scene->GetFbxManager();
	FbxIOSettings *io_settings = manager->GetIOSettings();

	FBX_SAFE_DESTROY(scene);
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(manager);
}

void FbxPrintNode(FbxNode *node, int depth)
{
	// Print the node's attributes.
	DebuggerPrintf("%*sNode [%s]", depth, " ", node->GetName());
	g_theConsole->addMessage(Stringf("%*sNode [%s]", depth, " ", node->GetName()));
	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxPrintAttribute(node->GetNodeAttributeByIndex(i), depth);
	}

	// Print the nodes children
	for (int32_t i = 0; i < node->GetChildCount(); ++i) {
		FbxPrintNode(node->GetChild(i), depth + 1);
	}
}

void FbxListScene(FbxScene *scene)
{
	// Print the node
	FbxNode *root = scene->GetRootNode();
	FbxPrintNode(root, 0);
}

void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth)
{
	if (nullptr == pAttribute) {
		return;
	}

	FbxNodeAttribute::EType type = pAttribute->GetAttributeType();

	char const* typeName = GetAttributeTypeName(type);
	char const* attrName = pAttribute->GetName();

	DebuggerPrintf("%*s- type='%s' name='%s'", depth, " ", typeName, attrName);
	g_theConsole->addMessage(Stringf("%*s- type='%s' name='%s'", depth, " ", typeName, attrName));
}

char const* GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}
//------------------------------------------------------------------------ Helpers for FbxLoadMesh
// So, new functions and the Fbx Code that makes them up

// Conver scene to a consistant basis.
// The name is misleading, as this actually converts to a 
// right-handed system (Y-UP, X-RIGHT, Z-BACK)
// but any attempt to convert to a left-handed
// system caused it to flip across Y, which is not what I 
// want - so I handle the final basis flip as a final step after
// loading
void ConvertSceneToEngineBasis(FbxScene *scene)
{
	fbxsdk::FbxAxisSystem local_system(
		(fbxsdk::FbxAxisSystem::EUpVector) fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
		(fbxsdk::FbxAxisSystem::EFrontVector) fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd,  // by default points toward me.
		(fbxsdk::FbxAxisSystem::ECoordSystem) fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded);

	FbxAxisSystem scene_system = scene->GetGlobalSettings().GetAxisSystem();
	if (scene_system != local_system) {
		local_system.ConvertScene(scene);
	}
}

// Converts the scene to only use triangles for the meshes.
void TriangulateScene(FbxScene *scene)
{
	FbxGeometryConverter converter(scene->GetFbxManager());

	// Replace being true means it will destroy the old 
	// representation of the mesh after conversion.  Setting this to 
	// false would leave the old representations in.
	converter.Triangulate(scene, /*replace=*/true);
}

// Flips all geoemetry across the X access
void FlipX(MeshBuilder *mb)
{
	// LEFT AS AN EXCERCISE (mostly as it depends on how your implementation of MeshBuilder/Mesh)

	// You just want to flip the X coordinate of all vertex
	// data (positions,normals/tangents/bitangents)
	//IMPLEMENT_ME;
	UNUSED(mb);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
// The meat of the problem - Importing a mesh
// This walking of a list, loading specific attributes will be a pretty common task
// in this class.
void ImportMeshes(MeshBuilder *out, FbxNode *node, MatrixStack &mat_stack,const Skeleton* skel)
{
	// first, we're traversing the graph, keep track of our current world transform
	// (I will do this by by pushing directly the world transform of this node to my stack)
	// (that is, this transform becomes the new top exactly)
	Matrix4 transform = GetNodeWorldTransform(node);
    mat_stack.push();
	mat_stack.make(transform);

	// First, traverse the scene graph, looking for FbxMesh node attributes.
	int attrib_count = node->GetNodeAttributeCount();
	for (int ai = 0; ai < attrib_count; ++ai) 
	{
		FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex(ai);
		std::string name = node->GetName();
		if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh) 
		{
			DebuggerPrintf("%s \n",name.c_str());
			ImportMesh(out, mat_stack, (FbxMesh*)attrib, skel);
		}
	}

	// Try to load any meshes that are children of this node
	int child_count = node->GetChildCount();
	for (int ci = 0; ci < child_count; ++ci) 
	{
		FbxNode *child = node->GetChild(ci);
		ImportMeshes(out, child, mat_stack, skel);
	}

	// we're done with this node, so its transform off the stack
	mat_stack.pop();

}

// This gets the world transform of a specific node
// and converts it to OUR engine matrix format
Matrix4 GetNodeWorldTransform(FbxNode *node)
{
	FbxAMatrix fbx_mat = node->EvaluateGlobalTransform();
	return ToEngineMatrix(fbx_mat);
}

Matrix4 ToEngineMatrix(FbxMatrix const &fbx_mat)
{
	// For reference, fbx_mat is stored Row Major, and can be accessed...
	FbxDouble4 row0 = fbx_mat.mData[0];
	FbxDouble4 row1 = fbx_mat.mData[1];
	FbxDouble4 row2 = fbx_mat.mData[2];
	FbxDouble4 row3 = fbx_mat.mData[3];

	Matrix4 result;
	result.values[0] = (float)row0.mData[0];
	result.values[1] = (float)row0.mData[1];
	result.values[2] = (float)row0.mData[2];
	result.values[3] = (float)row0.mData[3];
	result.values[4] = (float)row1.mData[0];
	result.values[5] = (float)row1.mData[1];
	result.values[6] = (float)row1.mData[2];
	result.values[7] = (float)row1.mData[3];
	result.values[8] = (float)row2.mData[0];
	result.values[9] = (float)row2.mData[1];
	result.values[10] = (float)row2.mData[2];
	result.values[11] = (float)row2.mData[3];
	result.values[12] = (float)row3.mData[0];
	result.values[13] = (float)row3.mData[1];
	result.values[14] = (float)row3.mData[2];
	result.values[15] = (float)row3.mData[3];
	return result;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

// Import a single mesh attribute
void ImportMesh(MeshBuilder *out, MatrixStack &mat_stack, FbxMesh *mesh, const Skeleton* skeleton)
{
	// Should have been triangulated before this - sort of a late check 
	// [treat this as an ASSERT_OR_DIE]
	ASSERT_OR_DIE(mesh->IsTriangleMesh(), "Was not a triangle mesh!");

	int32_t poly_count = mesh->GetPolygonCount();
	if (poly_count <= 0) 
	{
		return;
	}


	std::vector<fbx_skin_weight_t> skin_weights;
	if (nullptr != skeleton) {
		CalculateSkinWeights(skin_weights, mesh, skeleton);
	}


	// Geometric Transformations only apply to the current node
	// http://download.autodesk.com/us/fbx/20112/FBX_SDK_HELP/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
	Matrix4 geo_trans = GetGeometricTransform(mesh->GetNode());
	mat_stack.pushThenTransform(geo_trans);

	// import the mesh data.
	Matrix4 transform = mat_stack.top();

	


	// Starting a draw call.
	out->begin(RHIEnum::PRIMATIVE_TRIANGLE, false);
	// Load in the mesh - first, figure out how many polygons there are
	for (int32_t poly_idx = 0; poly_idx < poly_count; ++poly_idx) 
	{

		// For each polygon - get the number of vertices that make it up (should always be 3 for a triangulated mesh)
		int32_t vert_count = mesh->GetPolygonSize(poly_idx);
		ASSERT_OR_DIE(vert_count == 3, "Had more than 3 verts so not a trianle!"); // Triangle Meshes should ALWAYS have 3 verts per poly

		// Finally, import all the data for this vertex (for now, just position)
		for (int32_t vert_idx = 0; vert_idx < vert_count; ++vert_idx) 
		{
			FbxImportVertex(out, transform, mesh, poly_idx, vert_idx, skin_weights);
		}
	}

	out->end();

	// pop the geometric transform
	mat_stack.pop();
}

// This gets a local transform that is applied ONLY to data stored at this node,
// and is always considered local to the node.
Matrix4 GetGeometricTransform(FbxNode *node)
{
	Matrix4 ret = Matrix4();

	if ((node != nullptr) && (node->GetNodeAttribute() != nullptr)) {
		FbxEuler::EOrder order;

		node->GetRotationOrder(FbxNode::eSourcePivot, order);

		FbxVector4 const geo_trans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 const geo_rot = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 const geo_scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geo_mat;
		FbxAMatrix mat_rot;
		mat_rot.SetR(geo_rot, order);
		geo_mat.SetTRS(geo_trans, mat_rot, geo_scale);

		ret = ToEngineMatrix(geo_mat);
	}

	return ret;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// This loads in all the information for a specific vertex as defined by a 
// poly_idx and vert_idx.  For now, will only worry about position.
void FbxImportVertex(MeshBuilder *out, Matrix4 const &transform, FbxMesh *mesh,	int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t> skin_weights)
{
    /** NEW BITS **/

    // Will demonstarate with normal, but this process is repeated for
    // uvs, colors, tangents, bitangents (called binormals in the SDK)
    Vector3 normal;
    if (GetNormal(&normal, transform, mesh, poly_idx, vert_idx)) 
    {
        out->setNormal(normal);
    }

    // same process for UVs, Tangents, Bitangents
    Vector2 uv;
    if (GetUV(&uv, transform, mesh, poly_idx, vert_idx)) 
    {
        out->setUV(uv);
    }
    Vector3 tangent;
    if (GetTangent(&tangent, transform, mesh, poly_idx, vert_idx)) 
    {
        out->setTangent(tangent);
    }
    Vector3 bitangent;
    if (GetBitangent(&bitangent, transform, mesh, poly_idx, vert_idx)) 
    {
        out->setBitangent(bitangent);
    }

	if (HasSkinWeights(mesh))
	{
		// First, get the control point index for this poly/vert pair.
		int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
		if (ctrl_idx < 0) 
		{
			
		}
		else
		{
			out->setBones(skin_weights[ctrl_idx].indices);
			normSkinWeights(skin_weights[ctrl_idx].weights);
			out->setWeights(skin_weights[ctrl_idx].weights);
		}
	}

    /** END NEW BITS **/

	// If we get a position- push it.
	// This actually should always succeed.
	Vector3 position;
	if (FbxGetPosition(&position, transform, mesh, poly_idx, vert_idx)) 
	{
 		out->addVertex(position);
	}


}

//------------------------------------------------------------------------
bool GetNormal(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common exmaple of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
    if (element == nullptr) 
    {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch (element->GetMappingMode()) 
    {
        case FbxGeometryElement::eByControlPoint: 
        {
            // Get the normal by control point - most direct way.
            elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        } break; // case: eByControlPoint

        case FbxGeometryElement::eByPolygonVertex: 
        {
            // array is packed by polygon vertex - so each polygon is unique
            // (think sharp edges in the case of normals)
            elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
        } break; // case: eByPolygonVertex

        default:
            //ASSERT(0); // Unknown mapping mode
            return false;
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch (element->GetReferenceMode()) 
    {
        case FbxGeometryElement::eDirect: 
        {
            // this is just an array - use elem index as an index into this array
            if (elem_idx < element->GetDirectArray().GetCount()) {
                FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(elem_idx);
				Vector3 n = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
                *out = transform.TransformDirection(n);
                return true;
            }
            return false;
        } break;

        case FbxGeometryElement::eIndexToDirect: 
        {
            // This is an indexed array, so elem_idx is our offset into the 
            // index buffer.  We use that to get our index into the direct array.
            if (elem_idx < element->GetIndexArray().GetCount()) 
            {
                int index = element->GetIndexArray().GetAt(elem_idx);
                FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(index);
				Vector3 n = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
				*out = transform.TransformDirection(n);
                return true;
            }
            return false;
        } break;

        default:
            //ASSERT(0); // Unknown reference type
            return false;
    }
}
//------------------------------------------------------------------------
bool GetUV(Vector2 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common exmaple of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementUV *element = mesh->GetElementUV(0);
    if (element == nullptr)
    {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch (element->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
    {
        // Get the normal by control point - most direct way.
        elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
    } break; // case: eByControlPoint

    case FbxGeometryElement::eByPolygonVertex:
    {
        // array is packed by polygon vertex - so each polygon is unique
        // (think sharp edges in the case of normals)
        elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
    } break; // case: eByPolygonVertex

    default:
        //ASSERT(0); // Unknown mapping mode
        return false;
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch (element->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    {
        // this is just an array - use elem index as an index into this array
        if (elem_idx < element->GetDirectArray().GetCount()) {
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(elem_idx);
            *out = Vector2((float)fbxVector4.mData[0], (float)fbxVector4.mData[1]);
            return true;
        }
        return false;
    } break;

    case FbxGeometryElement::eIndexToDirect:
    {
        // This is an indexed array, so elem_idx is our offset into the 
        // index buffer.  We use that to get our index into the direct array.
        if (elem_idx < element->GetIndexArray().GetCount())
        {
            int index = element->GetIndexArray().GetAt(elem_idx);
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(index);
            *out = Vector2((float)fbxVector4.mData[0], (float)fbxVector4.mData[1]);
            return true;
        }
        return false;
    } break;

    default:
        //ASSERT(0); // Unknown reference type
        return false;
    }
}
//------------------------------------------------------------------------
bool GetTangent(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common exmaple of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementTangent *element = mesh->GetElementTangent(0);
    if (element == nullptr)
    {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch (element->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
    {
        // Get the normal by control point - most direct way.
        elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
    } break; // case: eByControlPoint

    case FbxGeometryElement::eByPolygonVertex:
    {
        // array is packed by polygon vertex - so each polygon is unique
        // (think sharp edges in the case of normals)
        elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
    } break; // case: eByPolygonVertex

    default:
        //ASSERT(0); // Unknown mapping mode
        return false;
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch (element->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    {
        // this is just an array - use elem index as an index into this array
        if (elem_idx < element->GetDirectArray().GetCount()) {
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(elem_idx);
            *out = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
            return true;
        }
        return false;
    } break;

    case FbxGeometryElement::eIndexToDirect:
    {
        // This is an indexed array, so elem_idx is our offset into the 
        // index buffer.  We use that to get our index into the direct array.
        if (elem_idx < element->GetIndexArray().GetCount())
        {
            int index = element->GetIndexArray().GetAt(elem_idx);
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(index);
            *out = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
            return true;
        }
        return false;
    } break;

    default:
        //ASSERT(0); // Unknown reference type
        return false;
    }
}
//------------------------------------------------------------------------
bool GetBitangent(Vector3 *out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common exmaple of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementBinormal *element = mesh->GetElementBinormal(0);
    if (element == nullptr)
    {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch (element->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
    {
        // Get the normal by control point - most direct way.
        elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
    } break; // case: eByControlPoint

    case FbxGeometryElement::eByPolygonVertex:
    {
        // array is packed by polygon vertex - so each polygon is unique
        // (think sharp edges in the case of normals)
        elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
    } break; // case: eByPolygonVertex

    default:
        //ASSERT(0); // Unknown mapping mode
        return false;
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch (element->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    {
        // this is just an array - use elem index as an index into this array
        if (elem_idx < element->GetDirectArray().GetCount()) {
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(elem_idx);
            *out = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
            return true;
        }
        return false;
    } break;

    case FbxGeometryElement::eIndexToDirect:
    {
        // This is an indexed array, so elem_idx is our offset into the 
        // index buffer.  We use that to get our index into the direct array.
        if (elem_idx < element->GetIndexArray().GetCount())
        {
            int index = element->GetIndexArray().GetAt(elem_idx);
            FbxVector4 fbxVector4 = element->GetDirectArray().GetAt(index);
            *out = Vector3((float)fbxVector4.mData[0], (float)fbxVector4.mData[1], (float)fbxVector4.mData[2]);
            return true;
        }
        return false;
    } break;

    default:
        //ASSERT(0); // Unknown reference type
        return false;
    }
}


// Gets the position from a FbxMesh value.
// Every <poly_idx, vert_idx> pair will evaluate to a unique
// "control point" - that is, a position, or vertex that can be 
// manipulate by the program - which makes importing positions pretty easy

bool FbxGetPosition(Vector3 *out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	// First, get the control point index for this poly/vert pair.
	int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	if (ctrl_idx < 0) {
		return false;
	}

	// Get the position for the control point.
	FbxVector4 fbx_pos = mesh->GetControlPointAt(ctrl_idx);

	// convert to an engine vertex, and save it to the out variable
	Vector3 pos((float)fbx_pos.mData[0], (float)fbx_pos.mData[1], (float)fbx_pos.mData[2]);
	*out_pos = transform.TransformPosition(pos);

	// return success.
	return true;
}

////////////////////////////////////////////////////////////////////////// Load Skeleton Functions //////////////////////////////////////////////////////////////////////////
// Get the bind pose - that is, the pose that the resources are authored
// for. If none exists - we'll assume ONLY the bind pose exists, and use
// the global transformations.
FbxPose* GetBindPose(FbxScene *scene)
{
    int pose_count = scene->GetPoseCount();
    for (int i = 0; i < pose_count; ++i) {
        FbxPose *pose = scene->GetPose(i);
        if (pose->IsBindPose()) {
            // DEBUG - Print all nodes part of this pose
            /*
            int count = pose->GetCount();
            for (int j = 0; j < count; ++j) {
            FbxNode *node = pose->GetNode(j);
            Trace( "fbx", "Pose Node: %s", node->GetName() );
            }
            */

            return pose;
        }
    }

    return nullptr;
}

// Imports the skeleton.
void ImportSkeleton(Skeleton *out,  // skeleton I'm loading into
    FbxNode *node,               // Current node I'm looking at.
    FbxSkeleton *root_bone,      // The head bone in this skeleton (keeping track purely for future support for nested skeletons in an FBX)
    FbxSkeleton *parent_bone,    // The bone to consider the parent of any subsequently loaded bone.
    FbxPose *pose)              // Pose we're loading.
{
    for (int i = 0; i < node->GetNodeAttributeCount(); ++i) 
    {

        // If this node is a skeleton node (a joint), 
        // then it should have a skeleton attribute. 
        FbxNodeAttribute *na = node->GetNodeAttributeByIndex(i);
        if (na->GetAttributeType() == FbxNodeAttribute::eSkeleton) 
        {

            // Cast it, and figure out the type of bone. 
            FbxSkeleton *skel = (FbxSkeleton*)na;
            FbxSkeleton::EType type = skel->GetSkeletonType();

            // From FBXSDK Documentation.
            // eRoot,			/*!< First element of a chain. */
            // eLimb,			/*!< Chain element. */
            // eLimbNode,		/*!< Chain element. */
            // eEffector		/*!< Last element of a chain. */

            // If this is a root - we better have not have another root higher 
            // up (for now).  This is just here to catch this happening so
            // I know if I should support it in the future - you could cut this.
            if ((type == FbxSkeleton::eRoot) || (type == FbxSkeleton::eEffector)) 
            {
                // this is a root bone - so can treat it as the root
                // of a skeleton.
                // TODO: no support for nested skeletons yet.
                ASSERT_OR_DIE(root_bone == nullptr, "");
                root_bone = skel;
            } // else, it is a limb or limb node, which we treat the same

              // See if this bone exists int he pose, and if so, 
              // get its index in the pose.
            int pose_node_idx = -1;
            if (pose != nullptr) 
            {
                pose_node_idx = pose->Find(node);
            }

            // Get the global transform for this bone
            Matrix4 bone_transform;
            if (pose_node_idx != -1) 
            {
                // If it exists in the pose - use the poses transform.
                // Since we're only doing the bind pose now, the pose should ONLY
                // be storing global transforms (as per the documentation).
                //
                // When we update this code to load in animated poses, this willc hange.
                bool is_local = pose->IsLocalMatrix(pose_node_idx);
                ASSERT_OR_DIE(false == is_local, ""); // for now, should always be global (pose transforms always are)

                FbxMatrix fbx_matrix = pose->GetMatrix(pose_node_idx);
                bone_transform = ToEngineMatrix(fbx_matrix);
            }
            else 
            {
                // Not in the pose - so we have two options
                if (parent_bone == nullptr) 
                {
                    // We have no parent [first in the skeleton], so just use 
                    // the global transform of the node.
                    bone_transform = ToEngineMatrix(node->EvaluateGlobalTransform());
                }
                else 
                {
                    // Otherwise, calculate my world based on my parent transform
                    // and my nodes local.
                    //
                    // The reason behind this is EvaluateGlobalTransform actually
                    // gets the animated transform, while getting the Pose transform
                    // will get an unanimated transform.  So if we're a child of a transform
                    // that was gotten from a pose, I should be evaluating off that.
                    //
                    // TODO: Evaluate Local from Parent - just in case their is siblings between this
                    Matrix4 local = ToEngineMatrix(node->EvaluateLocalTransform());
                    Matrix4 ptrans = out->getJointTransform(GetBoneName(parent_bone));
                    bone_transform = local.getTransformed(ptrans);
                }
            }

            // Add a joint.
            out->addJoint(GetBoneName(skel), (parent_bone != nullptr) ? GetBoneName(parent_bone) : nullptr, bone_transform);

            // set this as the next nodes parent, and continue down the chain
            parent_bone = skel;
            break;
        }
    }

    // import the rest
    for (int i = 0; i < node->GetChildCount(); ++i) 
    {
        ImportSkeleton(out, node->GetChild(i), root_bone, parent_bone, pose);
    }
}

//------------------------------------------------------------------------
// Just a helper - checks for null - but otherwise gets the node name
static char const* GetNodeName(FbxNode const *node)
{
    if (nullptr == node) {
        return "";
    }
    else {
        return node->GetName();
    }
}


//------------------------------------------------------------------------
// Get the name to use for this bone (ideally, use the node, but if the bone
// doesn't exist, use the skeleton attributes name)
char const* GetBoneName(FbxSkeleton const *skel)
{
    if (nullptr == skel) {
        return nullptr;
    }

    std::string node_name = GetNodeName(skel->GetNode());
    if (!node_name.empty())
    {
        return skel->GetNode()->GetName();
    }
    else 
    {
        return skel->GetName();
    }
}
////////////////////////////////////////////////////////////////////////// Load Motion //////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
// Just as an extra - if you want to use the FBX files frame rate, this
// is how you can fetch it.
float GetNativeFramefrate(FbxScene const *scene)
{
	// Get the scenes authored framerate
	const FbxGlobalSettings& settings = scene->GetGlobalSettings();
	FbxTime::EMode time_mode = settings.GetTimeMode();
	double framerate;
	if (time_mode == FbxTime::eCustom) {
		framerate = settings.GetCustomFrameRate();
	}
	else {
		framerate = FbxTime::GetFrameRate(time_mode);
	}
	return (float)framerate;
 }

//------------------------------------------------------------------------
bool FbxImportMotion(Motion *motion, Skeleton const *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate)
{
	// First, set the scene to use this animation - helps all the evaluation functions
	scene->SetCurrentAnimationStack(anim);

	// how far do we move through this
	FbxTime advance;
	advance.SetSecondDouble((double)(1.0f / framerate));

	// Get the start, end, and duration of this animation.
	FbxTime local_start = anim->LocalStart;
	FbxTime local_end = anim->LocalStop;
	float local_start_fl = (float)local_start.GetSecondDouble();
	float local_end_fl = (float)local_end.GetSecondDouble();

	// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
	// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
	// Whatever, we'll always start at least 0.

	local_start_fl = (std::max)(0.0f, local_start_fl);
	float duration_fl = local_end_fl - local_start_fl;

	// no duration, no animation!
	if (duration_fl <= 0.0f) {
		return false;
	}

	// Alright, we have a motion, we start getting data from it
	char const *motion_name = anim->GetName();
	//float time_span = duration_fl;

	// set some identifying information
	motion->m_name = motion_name;
	motion->m_framerate = framerate;
	motion->setDuration(duration_fl);

	// number of frames should encompasses the duration, so say we are at 10hz,
	// but have an animation that is 0.21f seconds.  We need at least...
	//    3 intervals (Ceiling(10 * 0.21) = Ceiling(2.1) = 3)
	//    4 frames (0.0, 0.1, 0.2, 0.3)
	uint frame_count = (uint)std::ceil(duration_fl * framerate) + 1;


	// Now, for each joint in our skeleton
	// not the most efficient way to go about this, but whatever - tool step
	// and it is not slow enough to matter.
	uint joint_count = skeleton->getJointCount();
	for (uint joint_idx = 0; joint_idx < joint_count; ++joint_idx) 
	{
		std::string name = skeleton->getJointName(joint_idx);

		// get the node associated with this bone
		// I rely on the names to find this - so author of animations
		// should be sure to be using the same rig/rig names.
		// (this is only important for the import process, after that
		// everything is assuming matching indices)
		FbxNode *node = scene->FindNodeByName(name.c_str());
		FbxNode *parent = nullptr;
		uint parent_idx = skeleton->getJointParentIndex(joint_idx);
		ASSERT_OR_DIE(node != nullptr, "Null pointer to a scene!");

		// get the parent node
		if (parent_idx != INVALID_JOINT_INDEX) 
		{
			std::string parent_name = skeleton->getJointName(parent_idx);
			parent = scene->FindNodeByName(parent_name.c_str());
			ASSERT_OR_DIE(parent != nullptr, "Null pointer to a parent joint!");
		}

		// Now, for this entire animation, evaluate the local transform for this bone at every interval
		// number of frames is 
		FbxTime eval_time = FbxTime(0);
		for (uint frame_idx = 0; frame_idx < frame_count; ++frame_idx) 
		{
			// Okay, get the pose we want
			Pose& pose = motion->m_poses[frame_idx];
			pose.m_transforms.resize(joint_count);
			Matrix4 joint_world = GetNodeWorldTransformAtTime(node, eval_time);
			Matrix4 joint_local = joint_world;
			if (nullptr != parent) 
			{
				Matrix4 parent_world = GetNodeWorldTransformAtTime(parent, eval_time);
				//joint_local =  parent_world.getInverse().getTransformed(joint_world); //Original way
				joint_local = joint_world.getTransformed(parent_world.getInverse());
			}

			pose.m_transforms[joint_idx].setFromMatrix(joint_local);

			eval_time += advance;
		}
	}

	return true;
}

//------------------------------------------------------------------------
Matrix4 GetNodeWorldTransformAtTime(FbxNode *node, FbxTime time)
{
	if (nullptr == node) {
		return Matrix4();
	}

	FbxMatrix fbx_mat = node->EvaluateGlobalTransform(time);
	return ToEngineMatrix(fbx_mat);
}
//
bool FbxLoadMotion(Motion *motion, Skeleton const *skeleton, char const *filename, uint framerate /*= 10*/)
{

	// Normal things
	FbxScene *scene = FbxLoadScene(filename, false);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Framerate - how often are we sampling this
	float fr = (float)framerate;


	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		FbxImportMotion(motion, skeleton, scene, anim, fr);
	}

	FbxUnloadScene(scene);

	// Flip the Axis [basically flip the x axis [all locals]
// 	{
// 		PROFILE_LOG_SCOPE("FlipAxis");
// 		mat44 transform = mat44::IDENTITY;
// 		transform.set_right(-transform.get_right());
// 
// 		uint frame_count = motion->get_frame_count();
// 		for (uint frame_idx = 0U; frame_idx < frame_count; ++frame_idx) {
// 			Pose *pose = motion->get_pose(frame_idx);
// 			for (uint i = 0; i < pose->local_transforms.size(); ++i) {
// 				pose->local_transforms[i] = pose->local_transforms[i] * transform;
// 			}
// 		}
// 	}

	return (motion->get_duration() > 0.0f);
}
////////////////////////////////////////////////////////////////////////// Skinning //////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------
// So skin weights are by control point - so we'll start off
// by collecting them all up front before we even start building our mesh
void CalculateSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights, FbxMesh const *mesh,	Skeleton const *skeleton)
{
	// default them all to zero
	uint ctrl_count = (uint)mesh->GetControlPointsCount();
	skin_weights.resize(ctrl_count);

	for (uint i = 0; i < ctrl_count; ++i) 
	{
		skin_weights[i].reset();
	}

	if (skeleton == nullptr) 
	{
		skin_weights.clear();
		return;
	}

	// for each deformer [probably will only be one] - figure out how it affects
	// my vertices
	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int didx = 0; didx < deformer_count; ++didx) 
	{
		FbxSkin *skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);
		if (nullptr == skin) 
		{
			continue;
		}

		// cluster is how a single joint affects the mesh, 
		// so go over the vertices it affects and keep track of how much!
		int cluster_count = skin->GetClusterCount();
		for (int cidx = 0; cidx < cluster_count; ++cidx) 
		{
			FbxCluster *cluster = skin->GetCluster(cidx);
			FbxNode const *link_node = cluster->GetLink();

			// Can't use it without a link node [shouldn't happen!]
			if (nullptr == link_node) 
			{
				continue;
			}

			// Find the joint - if this skeleton doesn't have a joint
			// then we skip this cluster!
			uint32_t joint_idx = skeleton->getJointIndex(link_node->GetName());
			if (joint_idx == INVALID_JOINT_INDEX) 
			{
				continue;
			}

			// cluster stores things in an index buffer again,
			// it will store indices to the control points it affects, 
			// as well as how much weight we apply
			int *indices = cluster->GetControlPointIndices();
			int index_count = cluster->GetControlPointIndicesCount();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < index_count; ++i) 
			{
				int control_idx = indices[i];
				double weight = weights[i];

				fbx_skin_weight_t& skin_weight = skin_weights[control_idx];
				AddHighestWeight(skin_weight, (uint32_t)joint_idx, (float)weight);
			}
			
		}
	}
}

//------------------------------------------------------------------------
// Keep track of this influencer if it influences more than an already existing
// bone. [all bones influence at 0 at the start]
void AddHighestWeight(fbx_skin_weight_t& skin_weight, uint32_t bone_idx, float weight)
{
	uint* bones;
	float* weights;
	bones = skin_weight.indices.getAsArray();
	weights = skin_weight.weights.getAsFloatArray();

	//Check if any of them are empty and assign there, else keep track of which is the lowest
	uint lowestWeightIndex = 0;
	bool isHigherThanAWeight = false;
	for (uint i = 0; i < 4; i++)
	{
		if (weights[i] == 0.f)
		{
			weights[i] = weight;
			bones[i] = bone_idx;
			skin_weight.indices = UIntVector4(bones[0], bones[1], bones[2], bones[3]);
			skin_weight.weights = Vector4(weights[0], weights[1], weights[2], weights[3]);
			return;
		}

		if (weights[i] > weight)
			continue;

		isHigherThanAWeight = true;

		if (weights[lowestWeightIndex] > weights[i])
		{
			lowestWeightIndex = i;
		}
	}

	//swap out the lowest if it is lower than my weight
	if (isHigherThanAWeight)
	{
		weights[lowestWeightIndex] = weight;
		bones[lowestWeightIndex] = bone_idx;
		skin_weight.indices = UIntVector4(bones[0], bones[1], bones[2], bones[3]);
		skin_weight.weights = Vector4(weights[0], weights[1], weights[2], weights[3]);
	}
}


//------------------------------------------------------------------------
bool HasSkinWeights(FbxMesh const *mesh)
{
	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	return deformer_count > 0;
}
#else 
////////////////////////////////////////////////////////////////////////// If FBX not available //////////////////////////////////////////////////////////////////////////
// Stubs so that if we don't have the FBX SDK Available, our program does not crash
void FbxListFile(char const*) {}
bool FbxLoadMesh(MeshBuilder*, const char*, const Skeleton* skel) { return false; }
bool FbxLoadSkeleton(Skeleton*, char const*) { return false; }
bool FbxLoadMotion(Motion* motion, const Skeleton* skeleton, const char*filename, uint framerate) { return false; }
#endif

