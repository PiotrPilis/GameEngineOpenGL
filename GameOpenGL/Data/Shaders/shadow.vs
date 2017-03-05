#version 330                                                                      
                                                                                    
layout (location = 0) in vec3 position;                                             
layout (location = 1) in vec2 texcoord;                                             
layout (location = 2) in vec3 normal;                                               
                                                                               
uniform mat4 vpMatrix; 
uniform mat4 wMatrix;                                                               
                                                                                                                                                                       
void main()                                                                         
{                                                                                   
    gl_Position = vpMatrix * wMatrix * vec4(position, 1.0);
}