#version 450

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColor;

//out vec3 Color;

/*void main()
{
	//VColor = vec3(1.0, 0.0, 0.5);
	//VNormal = normalize( NormalMatrix * vec3(0.0, 1.0, 0.0));
	//VPosition = vec3( ModelViewMatrix * vec4(Ver=texPosition, 1.0) ); // vertex pos on eye 

	
	vec2 vTexCoords = vec2 (VertexPosition.x, VertexPosition.z) / uSize;
	float height = texture2D(uSampler, vTexCoords).z * 10.0;

	//if (uSize == 512)
	//	VColor = vec3(1.0, 0.0, 0.0);
	//else
	//	VColor = vec3(0.0, 1.0, 0.0);

	vec4 newpos = vec4 (VertexPosition.x, height, VertexPosition.z, 1.0);
	//vec4 newpos = vec4(VertexPosition, 1.0);
	//vec4 newpos = vec4 (VertexPosition.x, cos((VertexPosition.z + VertexPosition.x) * 100), VertexPosition.z, 1.0);

	//inPosition = vec3( newpos );
	//VPosition = vec3( ModelViewMatrix * newpos );

    //gl_Position = MVP * newpos;
	gl_Position = newpos;
}/**/

/**/
void main()
{
	//Color = VertexColor;
	gl_Position = vec4(VertexPosition, 1.0);
}/**/
