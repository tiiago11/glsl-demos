#version 400

in vec3 Color;

layout (location=0) out vec4 FragColor;
uniform bool useSingleColor;

void main() 
{
	if(useSingleColor)
		FragColor = vec4(0.2,0.4,0.4, 1.0);
	else
		FragColor = vec4(Color, 1.0);
}
