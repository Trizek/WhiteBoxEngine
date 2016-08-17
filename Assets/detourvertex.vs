
/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 inputPosition;
in vec3 normal;

//////////////////////
// OUTPUT VARIABLES //
/////////////////////

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
	vec3 outNormal = mat3(modelViewMatrix) * normal;
	outNormal = normalize(outNormal);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = modelViewMatrix * vec4(inputPosition, 1.0f);

	float c = 0.003f * (-gl_Position.z);

	gl_Position = gl_Position + vec4(c * outNormal.x, c * outNormal.y, c * outNormal.z, 0.0f);


	gl_Position = projectionMatrix * gl_Position;

	
}

 