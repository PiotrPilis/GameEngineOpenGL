#version 420
layout( location = 1 ) in vec2 pos;

uniform vec2 offset[9];					// without multitask
/*layout(std140) uniform Offset			// within multitask
{
	vec2 offset[361];
};*/

void main(void)
{
	//gl_Position = vec4(pos + offset[gl_InstanceID], 1.0, 1.0);
	gl_Position = vec4(pos + offset[gl_InstanceID+1], 1.0, 1.0);
}