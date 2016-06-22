#version 420

in vec2 TexCoord;
layout (binding = 0) uniform sampler2D MainTex;
layout (location = 0) out vec4 FragColor;

uniform vec2 TexSize;

// based on this paper http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
void mainImage(out vec4 result, in vec2 uv)
{
	vec4 baseColor = texture2D(MainTex, uv);
	float distAlphaMask = baseColor.a;
	bool outline = true;
	float outlineMin0 = 0.1;
	float outlineMin1 = 0.2;
	float outlineMax0 = 0.5;
	float outlineMax1 = 0.9;

	vec4 outlineColor = vec4(1,0,0,1);
	if(outline && distAlphaMask >= outlineMin0 && distAlphaMask <= outlineMax1){
		float oFactor = 1.0;
		if(distAlphaMask <= outlineMin1){
			oFactor = smoothstep(outlineMin0, outlineMin1, distAlphaMask);
		}else{
			oFactor = smoothstep(outlineMax1, outlineMax0, distAlphaMask);
		}
		baseColor = mix(baseColor, outlineColor, oFactor);
	}

	bool softEdges = true;
	float softEdgeMin = 0.4;
	float softEdgeMax = 0.8;
	if(softEdges){
		baseColor.a *= smoothstep(softEdgeMin, softEdgeMax, distAlphaMask);
	}
    
	//TODO fix glow
	bool outerGlow = false;
	vec4 outerGlowColor = vec4(0,1,0,1);
	float outerGlowMin = 0.2;
	float outerGlowMax = 0.5;
	if(outerGlow){
		float hfK = 3.0;
		vec4 sum;
		for(float x = -hfK; x < hfK; x++){
			for(float y = -hfK; y < hfK; y++){
				vec4 glowTexel = texture( MainTex, TexCoord + (vec2(x,y) / TexSize));
				vec4 glowc = outerGlowColor * smoothstep (outerGlowMin, outerGlowMax, glowTexel.a);
				baseColor = mix(glowc, baseColor, baseColor.a);
			}
		}
	}
    result = baseColor;
}

void main() 
{
	vec4 c;
	mainImage(c, TexCoord);
	FragColor = c;
}
