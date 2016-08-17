
/////////////////////
// INPUT VARIABLES //
/////////////////////
in lowp vec3 inputPosition;
in lowp vec3 normal;
in lowp vec2 uv0;

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

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
#if __GEAR_VR
	gl_Position = sm.ProjectionMatrix * (sm.ViewMatrix[VIEW_ID] * (modelMatrix * (vec4(inputPosition, 1.0))));
	outNormal = mat3(sm.ViewMatrix[VIEW_ID]) * (mat3(modelMatrix) * normal);
#else
	gl_Position = sm.projectionMatrix * (modelViewMatrix * vec4(inputPosition, 1.0f));
	outNormal = mat3(modelViewMatrix) * normal;
#endif

	texCoord = uv0;
	outNormal = normalize(outNormal);
}
