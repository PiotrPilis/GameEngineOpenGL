#version 420
layout(quads, fractional_odd_spacing, ccw) in;

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

uniform sampler2DArray heightMap;

uniform vec2 offset;
uniform vec4 terrainInfo;



out vec2 esLocalCoord;
out vec2 esGlobalCoord;

out flat uint esIndexMap;
out vec3 esNormal;
out vec3 esTangent;

out vec3 esVert;
out float esDepth;
out vec4 esLightSpaceVert[3];



vec2 interpolate2D()                                                   
{                                                                                               
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
	vec2 a = mix(gl_in[1].gl_Position.xy, gl_in[0].gl_Position.xy, u);
    vec2 b = mix(gl_in[2].gl_Position.xy, gl_in[3].gl_Position.xy, u);
    return mix(a, b, v);
} 

float GetHeight(vec2 position)
{
	vec2 texCoord = abs(position.xy / terrainInfo.x);
	vec2 intTexCoord = vec2(floor(texCoord.x), floor(texCoord.y));

	return texture(heightMap, vec3(texCoord - intTexCoord, floor(texCoord.x) + terrainInfo.y * floor(texCoord.y))).r*terrainInfo.w;
}

void main()
{
	vec2 worldPos2D = interpolate2D();
	float height = 0.0;

	vec2 texCoord = abs(worldPos2D / terrainInfo.x);
	vec2 intTexCoord = vec2(floor(texCoord.x), floor(texCoord.y));
	esGlobalCoord = texCoord - intTexCoord;
	esIndexMap = uint(floor(texCoord.x) + terrainInfo.y * floor(texCoord.y));

	if(floor(texCoord.x) >= terrainInfo.y || floor(texCoord.y) >= terrainInfo.z || worldPos2D.x < 0.0 || worldPos2D.y < 0.0)
	{
		esIndexMap = 4294967295;
	} else
	{
		height = texture(heightMap, vec3(esGlobalCoord, esIndexMap)).r * terrainInfo.w;

		esLocalCoord = esGlobalCoord * 50.0;

		vec3 off = vec3(1.0, 1.0, 0.0);
		float hLeft = GetHeight(worldPos2D - off.xz);
		float hRight = GetHeight(worldPos2D + off.xz);
		float hFar = GetHeight(worldPos2D + off.zy);
		float hNear = GetHeight(worldPos2D - off.zy);

		esNormal.x = hLeft - hRight;
		esNormal.y = hNear - hFar;
		esNormal.z = 2.0;
		//esNormal = vec3(0.0, 0.0, 1.0);

		esTangent = vec3(1.0, 0.0, (hRight - hLeft) / 2.0);
	}
	esVert = vec3(worldPos2D.x, worldPos2D.y, height);
	gl_Position  = vpMatrix * vec4(esVert, 1.0);

	for (int i = 0 ; i < 3; i++) {
		esLightSpaceVert[i] = light.projectionMatrix[i] * vec4(esVert, 1.0);
	}
	esDepth = gl_Position.z;
}