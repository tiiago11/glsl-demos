#version 420

in vec2 TexCoord;

layout (location=0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D OutlineTex; 
layout (binding = 2) uniform sampler2D MainTex;

uniform bool isOutline;
/*
*	Simple gaussian blur
*	Based on http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
*/ 
vec4 gaussianBlurSampling(sampler2D image, vec2 texCoord){
	int maxOffset = 4;
	vec2 size = textureSize(OutlineTex, 0);
	float[7] offset = float[](-5.0f/size.x,-4.0f/size.x, -2.0f/size.x, 0.0f, 2.0f/size.x, 4.0f/size.x, 5.0f/size.x);
	float[7] weightInverse = float[](0.1270270270,0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162, 0.1270270270);

	vec4 colorH = vec4(0.0);
	vec4 colorV = vec4(0.0);

    for (int i = 0; i < 7; i++) {
        colorH += texture2D( image, texCoord + vec2(offset[i], 0.0f )) * weightInverse[i];
		colorV += texture2D( image, texCoord + vec2(0.0f, offset[i] )) * weightInverse[i];
		//colorH += texture2D( image, texCoord - vec2(offset[i], 0.0f )) * weightInverse[i];
		//colorV += texture2D( image, texCoord - vec2(0.0f, offset[i] )) * weightInverse[i];
    }
	return (colorH + colorV) * 0.5f;
}

void main() 
{
	vec4 mainColor = texture(MainTex, TexCoord);		
	vec4 outlineColor = gaussianBlurSampling(OutlineTex, TexCoord);
	//if(isOutline)
	//	FragColor = mix(mainColor, outlineColor, outlineColor.a);
	//else
	//	FragColor = mainColor;

	FragColor = outlineColor;

	//if(outlineColor.a == 1.0)
		//FragColor = vec4(0,1,0,1);
	//else 
	//	FragColor = vec4(0,0,1,1);
	
}
