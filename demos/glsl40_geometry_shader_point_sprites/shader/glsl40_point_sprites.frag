// Fragment Shader
//
// Simples Fragment Shader que seta a cor de todos os fragmentos para vermelho.
// Nenhum cálculo de iluminação é feito.
//
// Abril 2016 - Alex Frasson - afrasson@inf.ufsm.br

#version 400

in vec2 TexCoord; // From the geometry shader
in vec3 gColor;
//uniform sampler2D SpriteTex;

layout( location = 0 ) out vec4 FragColor;

void main()
{
	//FragColor = texture(SpriteTex, TexCoord);
	FragColor = vec4(gColor, 1.0);
}