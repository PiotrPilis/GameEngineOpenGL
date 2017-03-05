#version 420
layout( vertices = 4 ) out;

const float loddist0 = 100.0;
const float lodlevel0 = 25.0;

const float loddist1 = 300.0;
const float lodlevel1 = 20.0;

const float loddist2 = 500.0;
const float lodlevel2 = 15.0;

const float loddist3 = 800.0;
const float lodlevel3 = 13.0;

layout(std140) uniform Transform
{
	mat4 vpMatrix;
	mat4 vMatrix;
	vec3 eyeWorldPosition;
};

/*in Vertex
{
    vec2 worldPosition;
};*/

float level(vec2 v)
{
	float dist = distance(v, eyeWorldPosition.xy);
    if (dist < loddist0)
        return lodlevel0;
    else if (dist < loddist1)
        return lodlevel1;
    else if (dist < loddist2)
        return lodlevel2;
    else if (dist < loddist3)
        return lodlevel3;
    else
		return 1.0;
}

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    if (gl_InvocationID == 0)
    {
        float level0 = level(gl_in[0].gl_Position.xy);
        float level1 = level(gl_in[1].gl_Position.xy);
        float level2 = level(gl_in[2].gl_Position.xy);
        float level3 = level(gl_in[3].gl_Position.xy);

        gl_TessLevelOuter[0] = mix(level1, level2, 0.5);
        gl_TessLevelOuter[1] = mix(level1, level0, 0.5);
        gl_TessLevelOuter[2] = mix(level3, level0, 0.5);
        gl_TessLevelOuter[3] = mix(level2, level3, 0.5);
        gl_TessLevelInner[0] = mix(gl_TessLevelOuter[1], gl_TessLevelOuter[2], 0.5);
        gl_TessLevelInner[1] = mix(gl_TessLevelOuter[0], gl_TessLevelOuter[3], 0.5);

        /* glTessLevelOuter[0] = 1.0;
         glTessLevelOuter[1] = 1.0;
         glTessLevelOuter[2] = 1.0;
         glTessLevelOuter[3] = 1.0;
         glTessLevelInner[0] = 1.0;
         glTessLevelInner[1] = 1.0;
     */

    }
}
