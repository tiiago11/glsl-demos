#version 420

in vec2 TexCoord; // Coordenada de textura enviada pelo FS
layout (binding = 0) uniform sampler2D Texture1; // binding = 0  equivalente a GL_TEXTURE0
layout (binding = 1) uniform sampler2D Texture2; // binding = 1  equivalente a GL_TEXTURE1
layout (location = 0) out vec4 FragColor;

void main() 
{
	//Amostra as texturas. TexCoord foi enviado pelo VS explicitamente
	// e interpolado para cada fragmento automaticamente.
	vec4 col1 = texture(Texture2, TexCoord);
	vec4 col2 = texture(Texture1, TexCoord);

	//Interpola linearmente as duas cores utilizando a coordenada X da textura
	//ou seja, linearmente da esquerda para a direita.
    FragColor = mix(col1, col2, TexCoord.x);
}
