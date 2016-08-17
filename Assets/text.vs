

/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 inputPosition;
in vec3 normal;
in vec3 inputColor;
in vec2 uv0;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
smooth out vec3 outColor;
smooth out vec2 texCoord;
smooth out vec3 outNormal;

///////////////////////
// UNIFORM VARIABLES //
///////////////////////
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = modelViewMatrix * vec4(inputPosition, 1.0f);
	gl_Position = projectionMatrix * gl_Position;


	// Store the input color for the pixel shader to use.
	outColor = inputColor;
	texCoord = uv0;
	outNormal = mat3(modelViewMatrix) * normal;
	outNormal = normalize(outNormal);
	
}
