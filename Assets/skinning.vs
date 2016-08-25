
/////////////////////
// INPUT VARIABLES //
/////////////////////
in lowp vec3 inputPosition;
in lowp vec3 normal;
in lowp vec4 boneIndices;
in lowp vec4 boneWeights;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
smooth out vec2 texCoord;
smooth out vec3 outNormal;

#if __GEAR_VR

uniform SceneMatrices
{
	uniform mat4 ViewMatrix[NUM_VIEWS];
	uniform mat4 ProjectionMatrix;
} sm;

uniform mat4 modelMatrix;

#else

uniform SceneMatrices
{
	uniform mat4 projectionMatrix;
} sm;

uniform mat4 modelViewMatrix;

#endif

uniform SkinningMatrices
{
	uniform mat4 mat[64];
} skin;



////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	vec4 pos = vec4(inputPosition, 1.0f);
	vec4 pos2 = vec4(0, 0, 0, 1);
	pos2 += boneWeights[0] * (skin.mat[int(boneIndices[0])] * pos);
	pos2 += boneWeights[1] * (skin.mat[int(boneIndices[1])] * pos);
	pos2 += boneWeights[2] * (skin.mat[int(boneIndices[2])] * pos);
	pos2 += boneWeights[3] * (skin.mat[int(boneIndices[3])] * pos);

	vec3 norm = normal;
	vec3 n2 = vec3(0, 0, 0);
	n2 += boneWeights[0] * (mat3(skin.mat[int(boneIndices[0])]) * norm);
	n2 += boneWeights[1] * (mat3(skin.mat[int(boneIndices[1])]) * norm);
	n2 += boneWeights[2] * (mat3(skin.mat[int(boneIndices[2])]) * norm);
	n2 += boneWeights[3] * (mat3(skin.mat[int(boneIndices[3])]) * norm);


#if __GEAR_VR
	gl_Position = sm.ProjectionMatrix * (sm.ViewMatrix[VIEW_ID] * (modelMatrix * (vec4(pos2.xyz, 1.0))));
	outNormal = mat3(sm.ViewMatrix[VIEW_ID]) * (mat3(modelMatrix) * n2);
#else



	gl_Position = sm.projectionMatrix * (modelViewMatrix * vec4(pos2.xyz, 1.0f));
	outNormal = mat3(modelViewMatrix) * n2;
#endif

	outNormal = normalize(outNormal);
}
