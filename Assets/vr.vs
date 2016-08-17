



/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 inputPosition;
in vec3 normal;
in vec3 inputColor;
in vec2 uv0;


///////////////////////
// UNIFORM VARIABLES //
///////////////////////

#if __GEAR_VR
	uniform SceneMatrices
	{
		uniform mat4 ViewMatrix[ NUM_VIEWS ];
		uniform mat4 ProjectionMatrix;
	} sm;
	uniform mat4 modelMatrix;
#else
	uniform mat4 projectionMatrix;
	uniform mat4 modelViewMatrix;
#endif

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
#if __GEAR_VR
	gl_Position = sm.ProjectionMatrix * ( sm.ViewMatrix[VIEW_ID] * ( vec4( vertexPosition, 1.0 ) ) );
#else
	gl_Position = modelViewMatrix * vec4(inputPosition, 1.0f);
	gl_Position = projectionMatrix * gl_Position;	
#endif
}
