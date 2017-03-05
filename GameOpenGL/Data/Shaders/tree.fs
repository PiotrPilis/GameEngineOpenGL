#version 420

in vec2 vsTexCoord;
in vec3 vsNormal;
in vec3 vsVert;
in float vsDepth;

in vec4 vsLightSpaceVert[3];

uniform sampler2D textures;
uniform sampler2D shadowMap[3];
uniform mat4 modelMatrix;
uniform float cascadeEndClipSpace[3];

const float Epsilon = 0.0001;

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

	vec2 brightnessRange;					// obserwuj to, nie wiem czy nie powinno byæ wyrównane do vec 4 (std140)
} light;

out vec4 frag_colour;

float CalcShadowFactor(int cascadeIndex, vec4 vertLightSpacePos)                                                  
{                                                                                           
    vec3 ProjCoords = vertLightSpacePos.xyz / vertLightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;                                                  
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;                                                  
    float z = 0.5 * ProjCoords.z + 0.5;                                                     
    float Depth = texture(shadowMap[cascadeIndex], UVCoords).x;                                          
    if (Depth < z + 0.0001)                                                                 
        return 0.5;                                                                         
    else                                                                                    
        return 1.0;                                                                         
} 

void main () {
	float ShadowFactor = 0.0;
    vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 1.0);

    for (int i = 0 ; i < 3; i++) {
        if (vsDepth <= cascadeEndClipSpace[i]) {
            ShadowFactor = CalcShadowFactor(i, vsLightSpaceVert[i]);

            break;
        }
   }


	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 normal = normalize(normalMatrix * vsNormal);
    
    vec3 fragToLight = -light.position.xyz;

    float brightness = clamp(dot(normal, fragToLight) / (length(fragToLight) * length(normal)), light.brightnessRange.x, light.brightnessRange.y);
    vec4 fragColor = texture(textures, vsTexCoord);
	//vec4 fragColor = texture(shadowMap[0], vsTexCoord);
	//ShadowFactor = 1.0;
    frag_colour = vec4(ShadowFactor * brightness * light.color.rgb * (fragColor.rgb + CascadeIndicator.rgb), fragColor.a);
	//frag_colour = vec4(texture(shadowMap[1], vsTexCoord).x, texture(shadowMap[1], vsTexCoord).x, texture(shadowMap[1], vsTexCoord).x, 1.0);
}