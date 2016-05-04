// Vertex Shader
//
// Como as tranformções serão aplicadas somente no estágio Tessellation Evaluation Shader, este
// vertex shader apenas passa os vértices de entrada adiante para o Tessellation Control Shader.
//
// Abril 2016 - Alex Frasson - afrasson@inf.ufsm.br

#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 ModelViewMatrix;
out vec3 vColor;

void main()
{	
	vColor = VertexPosition;
	gl_Position = ModelViewMatrix * vec4(VertexPosition,1.0);
}