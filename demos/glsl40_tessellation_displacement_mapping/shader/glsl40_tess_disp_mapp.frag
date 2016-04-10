// Fragment Shader
//
// Simples Fragment Shader que seta a cor de todos os fragmentos para vermelho.
// Nenhum cálculo de iluminação é feito.
//
// Abril 2016 - Alex Frasson - afrasson@inf.ufsm.br

#version 450

layout(location = 0) out vec4 FragColor;

//in vec2 teTexCoord;

void main()
{
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}