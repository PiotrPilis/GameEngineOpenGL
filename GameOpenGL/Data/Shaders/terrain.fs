#version 420

const uint NUM_TEXTURES = 16;

in vec2 esLocalCoord;
in vec2 esGlobalCoord;

in flat uint esIndexMap;
in vec3 esNormal;
in vec3 esTangent;

in vec3 esVert;
in vec4 esLightSpaceVert[3];
in float esDepth;

uniform sampler2DArray heightMap;
uniform sampler2DArray textures;
uniform sampler2DArray pathMap;							// num pathMap = num heightMap
uniform sampler2DArray normalMap;

uniform sampler2D shadowMap[3];


uniform uvec2[NUM_TEXTURES] pathMapInfo;				// x -> ile tekstur ma obs³u¿yæ - y -> od jakiej tekstury ID
uniform float[NUM_TEXTURES] pathMapFactor;				// Opisuje przedzia³y path map'y - co ile jest 100% dana tekstura	  1: 0%; 2: 33%; 3: 66%; 4: 100%,        dla num = 4
uniform vec4 terrainInfo;								// x -> gêstoœæ texCoord - y -> numHeightMaps.x - z -> numHeightMaps.y - w -> maxHeight
uniform uint[16] texturesID;							// ID'ki tekstur bior¹cych udzia³ na danej height map'ie (na danym obszarze) 
uniform int[16] normalID;
uniform float cascadeEndClipSpace[3];

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

out vec4 frag_colour;

vec3 CalcBumpedNormal(int NormalID, vec3 fragNormal)
{
    vec3 tangent = normalize(esTangent);
    tangent = normalize(tangent - dot(tangent, fragNormal) * fragNormal);
    vec3 bitangent = cross(tangent, fragNormal);
    vec3 bumpMapNormal = texture(normalMap, vec3(esLocalCoord, NormalID)).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 newNormal;
    mat3 TBN = mat3(tangent, bitangent, fragNormal);
    newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

float CalcShadowFactor(int cascadeIndex, vec4 vertLightSpacePos)                                                  
{                                                                                           
    vec3 ProjCoords = vertLightSpacePos.xyz / vertLightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;                                                  
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;                                                  
    float z = 0.5 * ProjCoords.z + 0.5;                                                     
    float Depth = texture(shadowMap[cascadeIndex], UVCoords).x;                                          
    if (Depth < z + 0.00001)                                                                 
        return 0.5;                                                                         
    else                                                                                    
        return 1.0;                                                                         
} 

void CalcLight(uint idNormalID, vec3 fragColor)
{
	float shadowFactor = 1.0;
    vec3 cascadeColor = vec3(0.0, 0.0, 0.0);

    for (int i = 0 ; i < 3; i++) {
        if (esDepth <= cascadeEndClipSpace[i]) {
            shadowFactor = CalcShadowFactor(i, esLightSpaceVert[i]);

           /* if (i == 0) 
                cascadeColor = vec3(0.1, 0.0, 0.0);
            else if (i == 1)
                cascadeColor = vec3(0.0, 0.1, 0.0);
            else if (i == 2)
                cascadeColor = vec3(0.0, 0.0, 0.2);*/

				vec3 projco = 0.5 * (esLightSpaceVert[i].xyz / esLightSpaceVert[i].w) + 0.5;
			if(projco.x > 1.0 || projco.x < 0.0)
				cascadeColor.r = 0.7;
			if(projco.y > 1.0 || projco.y < 0.0)
				cascadeColor.b = 0.7;
            break;
        }
   }

	vec3 newNormal = normalize(esNormal);
	if(normalID[idNormalID] != -1)
	{
		newNormal = CalcBumpedNormal(normalID[idNormalID], newNormal);
	}
    
	vec3 fragToLight = normalize(-light.position.xyz);
	//vec3 fragToLight = -light.position.xyz - esVert;						// point light

	float brightness = clamp(dot(newNormal, fragToLight) / (length(fragToLight) * length(newNormal)), light.brightnessRange.x, light.brightnessRange.y);
	frag_colour = vec4(shadowFactor * brightness * light.color.rgb * (fragColor.rgb + cascadeColor), 1.0);
}

void main () {
	if(esIndexMap == 4294967295)
		discard;

	// -> Computing textures color
		float Factor = texture(pathMap, vec3(esGlobalCoord, esIndexMap)).r;
		if(Factor == 0.0)
			CalcLight(0, texture(textures, vec3(esLocalCoord, texturesID[pathMapInfo[esIndexMap].y])).rgb);
		else
			for(uint i = 1; i <= pathMapInfo[esIndexMap].x; ++i)
			{
				if((pathMapFactor[esIndexMap] * i) >= Factor)
				{
					float normalizedFactor = (Factor - pathMapFactor[esIndexMap] * (i - 1)) / (pathMapFactor[esIndexMap] * i - pathMapFactor[esIndexMap] * (i - 1));

					// -> Directonal light
					vec3 fragColor = texture(textures, vec3(esLocalCoord, texturesID[pathMapInfo[esIndexMap].y + i - 1])).rgb * normalizedFactor + texture(textures, vec3(esLocalCoord, texturesID[pathMapInfo[esIndexMap].y + i])).rgb * (1.0 - normalizedFactor);
					CalcLight(i, fragColor);
					// <-
					break;
				}
			}
			//frag_colour = vec4(1.0, 1.0, 1.0, 1.0);
	// <-
}