//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tint : TINT;
	float2 uv : UV;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	uint4 bone_indices : BONE_INDICES;
	float4 bone_weights : BONE_WEIGHTS;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float4 tint : TINT;
	float2 uv : UV;
	uint4 bone_indices : BONE_INDICES;
	float4 bone_weights : BONE_WEIGHTS;

	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;

	float4 world_position : WORLD;
	
};

//-------------------------------------------------------------------------
// CONSTANT BUFFERS
cbuffer matrix_cb : register(b0)
{
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1)
{
	float GAME_TIME;
	float SYSTEM_TIME;
	float GAME_FRAME_TIME;
	float SYSTEM_FRAME_TIME;
};

struct light_struct
{
	float4 LIGHT_COLOR; // <r, g, b, intensity>
	float4 LIGHT_DIRECTION;
	float4 LIGHT_POSITION;
	float3 ATTENUATION; // <constant, linear, quadratic>
	float ANGLE;
	float4 SPEC_ATTENUATION; // <constant, linear, quadratic, padding>
};
#define MAX_LIGHTS 8
cbuffer light_cb : register(b2)
{
	// GLOBAL LIGHT DATA
	float4 AMBIENT; // <r, g, b, intensity>

							// PER LIGHT DATA
							// POINT LIGHT, DOT3
	light_struct LIGHTS[MAX_LIGHTS];
	

							  // SURFACE DATA
	float SPEC_FACTOR; // [0, 1] specular scaling factor
	float SPEC_POWER; // [POSITIVE INTEGER]  more speculative power.
	float2 LIGHT_PADDING;

	// Needed for lighting - but I consider part of the matrix buffer
	float4 EYE_POSITION;
}


Texture2D<float4> tDiffuse : register(t0);
Texture2D<float4> tNormal : register(t1);
Texture2D<float4> tSpecular : register(t2);
Texture2D<float4> tAmbientOcclusion : register(t3);
StructuredBuffer<float4x4> tSkinMatrices : register(t5);
SamplerState sSampler : register(s0);

///////////////////////////////////////////////////////////////////////////////
float4 DoDiffuse(light_struct light, float3 light_vector, float3 surface_normal)
{
	float NdotL = saturate(dot(light_vector, surface_normal));
	float4 light_color = float4(light.LIGHT_COLOR.xyz, 1.0f);
	return light_color * NdotL;
}
float4 DoSpecular(light_struct light, float3 view_vector, float3 light_vector, float3 surface_normal)
{
	//Phong
	float3 R = reflect(light_vector, surface_normal);
	float RdotV = saturate(dot(R, view_vector));

	//Blinn-Phong
	//float3 half_angle_vector = normalize(light_vector + view_vector);
	//float NdotH = max(0, dot(surface_normal, half_angle_vector));

	return float4((light.LIGHT_COLOR.xyz * pow(RdotV, SPEC_POWER) * SPEC_FACTOR), 1.0f);
}
float DoAttenuation(light_struct light, float distance, float4 attenuationType)
{
	float attenuation = light.LIGHT_COLOR.w / (attenuationType.x + attenuationType.y * distance + attenuationType.z * distance * distance);
	return saturate(attenuation);
}
struct LightingResult
{
	float4 diffuse;
	float4 specular;
};
LightingResult DoPointLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = light.LIGHT_POSITION.xyz - point_being_shaded.xyz;
	float distance = length(light_vector);
	light_vector = light_vector / distance;

	float attenuation = DoAttenuation(light, distance, float4(light.ATTENUATION, 1.0f));
	float specAttenuation = DoAttenuation(light, distance, light.SPEC_ATTENUATION);

	result.diffuse = DoDiffuse(light, light_vector, surface_normal) * attenuation;
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal) * specAttenuation;

	return result;
}

LightingResult DoDirectionalLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = -light.LIGHT_DIRECTION.xyz;

	result.diffuse = DoDiffuse(light, light_vector, surface_normal);
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal);

	return result;
}
float DoSpotCone(light_struct light, float3 light_vector)
{
	float minCos = cos(light.ANGLE);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(light.LIGHT_DIRECTION.xyz, -light_vector);
	cosAngle = clamp(cosAngle, minCos, maxCos);
	return smoothstep(minCos, maxCos, cosAngle);
}
LightingResult DoSpotLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = light.LIGHT_POSITION.xyz - point_being_shaded.xyz;
	float distance = length(light_vector);
	light_vector = light_vector / distance;

	float attenuation = DoAttenuation(light, distance, float4(light.ATTENUATION, 1.0f));
	float specAttenuation = DoAttenuation(light, distance, light.SPEC_ATTENUATION);
	float spotIntensity = DoSpotCone(light, light_vector);

	result.diffuse = DoDiffuse(light, light_vector, surface_normal) * attenuation * spotIntensity;
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal) * specAttenuation * spotIntensity;

	return result;
}
LightingResult ComputeLighting(float4 point_being_shaded, float3 surface_normal)
{
	float3 view_vector = -normalize(EYE_POSITION.xyz - point_being_shaded.xyz);
	
	LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

		result = DoSpotLight(LIGHTS[i], view_vector, point_being_shaded, surface_normal);

		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}
//-------------------------------------------------------------------------
// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{
	vertex_to_fragment_t out_data = (vertex_to_fragment_t) 0;

   // so skin transforms should be in model space, so apply them before we go through
	float4 local_pos = float4(vertex.position, 1.0f);
   // so position is going to be transformed by 
	uint numMatrices;
	uint sizeOfBuffer;
	tSkinMatrices.GetDimensions(numMatrices, sizeOfBuffer);
	float4x4 skin_model;
	if (vertex.bone_indices.x < numMatrices && vertex.bone_indices.y < numMatrices && vertex.bone_indices.z < numMatrices && vertex.bone_indices.w < numMatrices)
	{
        // Really we're taking a weighted blend of the final transform, but
        // math works out that we can blend the matrices themselves.
        // This happens before model transform;
		float4x4 m0 = mul(vertex.bone_weights.x, tSkinMatrices[vertex.bone_indices.x]);
		float4x4 m1 = mul(vertex.bone_weights.y, tSkinMatrices[vertex.bone_indices.y]);
		float4x4 m2 = mul(vertex.bone_weights.z, tSkinMatrices[vertex.bone_indices.z]);
		float4x4 m3 = mul(vertex.bone_weights.w, tSkinMatrices[vertex.bone_indices.w]);
		float4x4 skin_transform = m0 + m1 + m2 + m3;
        // so instead of using our model matrix before, we will use 
        // the skin/model matrix (that is, apply skin transform and then model)
        // this is uesd for position/tangent/normal/etc...
		skin_model = mul(skin_transform, MODEL);
	}
	else
	{
		skin_model = MODEL;
	}
	float4 world_position = mul(local_pos, skin_model); // no model matrix
	float4 view_position = mul(world_position, VIEW);
	float4 clip_position = mul(view_position, PROJECTION);
   // . . .

	out_data.position = clip_position;
	out_data.normal = mul(float4(vertex.normal, 0.0f), skin_model).xyz; // W get sa 0, so as not to take on the translation of the model matrix
	out_data.tint = vertex.tint;
	out_data.uv = vertex.uv;
	out_data.world_position = world_position;
	out_data.tangent = mul(float4(vertex.tangent, 0.f), skin_model).xyz;
	out_data.bitangent = mul(float4(vertex.bitangent, 0.f), skin_model).xyz;
	out_data.bone_weights = vertex.bone_weights;
	out_data.bone_indices = vertex.bone_indices;

	return out_data;
}

float4 NormalAsColor(float3 n)
{
	return float4((n + 1.0f) * .5f, 1.0f);
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction(vertex_to_fragment_t data) : SV_Target0
{
	  //*/
	float3 specularMap = tSpecular.Sample(sSampler, data.uv).xyz;
	float3 occlusionMap = tAmbientOcclusion.Sample(sSampler, data.uv).xyz;
	float4 color = tDiffuse.Sample(sSampler, data.uv);
	float4 diffuse = color * data.tint;

	float3 normal_color = tNormal.Sample(sSampler, data.uv).xyz;
	float3 faceNormal = normalize(data.normal);

	float3 tangent = data.tangent;
	float3 bitangent = normalize(cross(tangent, faceNormal));
	tangent = normalize(cross(faceNormal, bitangent));

	float3x3 tbn = { tangent, bitangent, faceNormal };

	float3 surface_normal = normal_color * float3(2, 2, 1) - float3(1, 1, 0);
	surface_normal = mul(surface_normal, tbn);

	float3 normal = surface_normal;
	normal = normalize(normal);

	LightingResult lit = ComputeLighting(data.world_position, normal);

	float4 ambient = float4(AMBIENT.xyz * AMBIENT.w, 1.0f) * float4(occlusionMap, 1.0f);
	

	float4 material_specular = tSpecular.Sample(sSampler, data.uv);
	float4 specular = material_specular * lit.specular;

	
	float4 diffuse_factor = saturate(ambient + lit.diffuse);
	float4 final_color = diffuse_factor * diffuse + specular;

	return final_color;
	/*/
	return float4(data.bone_weights.xyz, 1.0f);
	//*/
}
