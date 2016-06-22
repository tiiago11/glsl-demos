#version 420

in vec2 TexCoord;

layout (location=0) out vec4 FragColor;

uniform sampler2D MainTex;

void main() 
{
	FragColor = vec4(0,1,0,1);
}
