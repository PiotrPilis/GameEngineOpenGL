#version 420
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texcoord;
layout( location = 2 ) in vec3 normal;

layout(std140) uniform Transform
{
	mat4 vpMatrix;
	mat4 vMatrix;
	vec3 eyeWorldPosition;
};
layout(std140) uniform Light
{
	mat4 projectionMatrix[3];

	vec4 position;
	vec4 color;

	vec2 brightnessRange;
} light;
uniform mat4 modelMatrix;

out vec2 vsTexCoord;
out vec3 vsNormal;
out float vsDepth;

out vec4 vsLightSpaceVert[3];

void main(void)
{
	vsTexCoord = texcoord;
	vsNormal = normal;

	for (int i = 0 ; i < 3; i++) {
        vsLightSpaceVert[i] = light.projectionMatrix[i] * modelMatrix * vec4(position, 1.0);
    }

	gl_Position  = vpMatrix * modelMatrix * vec4(position, 1.0);

	vsDepth = gl_Position.z;
}