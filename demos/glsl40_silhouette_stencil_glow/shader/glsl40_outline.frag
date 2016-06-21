#version 420

in vec2 TexCoord;

layout (location=0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D OutlineTex; 
layout (binding = 1) uniform sampler2D MainTex; 

uniform bool useSingleColor;

void main() 
{
	FragColor = vec4(1,0,0,1);
}
