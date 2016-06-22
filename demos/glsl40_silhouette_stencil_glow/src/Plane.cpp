#include "Plane.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <TextureManager.h>

using namespace std;
/*
	TODO:
		-multipass gauss blur
			-pingpong texture to improve current filter.

*/
Plane::Plane(GLFWwindow* window, int size){
	this->size = size;
	this->window = window;
	planePos = vec3(0.0f, 0.0f, 2.0f);
}

void Plane::init(){
	genPlane();
	genBuffers();

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(
		vec3(0.0f, 0.0f, -1.0f), //eye
		vec3(0.0f, 0.0f, 0.0f), //center
		vec3(0.0f, 1.0f, 0.0f)); //up
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

	// load shaders
	try {
		shader.compileShader("shader/glsl40_silhouette_stencil_glow.vert");
		shader.compileShader("shader/glsl40_silhouette_stencil_glow.frag");
		
		outlineShader.compileShader("shader/glsl40_silhouette_stencil_glow.vert");
		outlineShader.compileShader("shader/glsl40_outline.frag");

		blurShader.compileShader("shader/glsl40_silhouette_stencil_glow.vert");
		blurShader.compileShader("shader/blur.frag");

		blurShader.link();
		outlineShader.link();
		shader.link();

		shader.use();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	// Setup some stencil options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	fboIDs[0] = setupFBO(GL_TEXTURE0);
	fboIDs[1] = setupFBO(GL_TEXTURE1);

	glActiveTexture(GL_TEXTURE2);
	if (!TextureManager::Inst()->LoadTexture("..\\..\\resources\\lena.bmp", 1))
		std::cout << "Failed to load texture." << std::endl;

	renderOutline();
	applyFilter();
}

GLuint Plane::setupFBO(GLenum texSlot) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	GLuint handle;
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &handle);
	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	// Create the texture object
	GLuint renderTex;
	glGenTextures(1, &renderTex);
	glActiveTexture(texSlot);  // Use texture unit 0
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	//// Create the depth buffer
	//GLuint depthBuf;
	//glGenRenderbuffers(1, &depthBuf);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf); // Bind the depth buffer to the FBO

	GLuint stencilBuf;
	glGenRenderbuffers(1, &stencilBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, stencilBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuf); // Bind the stencil buffer to the FBO

	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		cout << "Framebuffer is complete" << endl;
	}
	else {
		cout << "Framebuffer error: " << result << endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return handle;
}

void Plane::renderOutline() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[0]);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	////////////////////////
	//////////////////////// first draw: enable stencil write
	////////////////////////
	shader.use();

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjections

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);

	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);

	////////////////////////
	//////////////////////// second draw - scale the same object and draw again where the stencil mask is different than 1
	////////////////////////
	outlineShader.use();

	glClear(GL_COLOR_BUFFER_BIT); // we only want the stencil
	
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	//// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1, 1.1, 1));
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	outlineShader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);

	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Plane::applyFilter() {
	GLuint amount = 1;
	
	blurShader.use();

	for (GLuint i = 0; i < amount; i++)
	{
		int fbo = i % 2 == 0;
		glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[fbo]);

		// matrices setup
		modelMatrix = mat4(1.0f); // identity
		modelMatrix = glm::translate(modelMatrix, planePos); // translate back
		//modelMatrix = glm::scale(modelMatrix, vec3(5, 5, 1));
		modelViewMatrix = viewMatrix * modelMatrix;
		modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

		// set var MVP on the shader
		blurShader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjections
		blurShader.setUniform("MainTex", 0);

		glBindVertexArray(vaoID);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Plane::update(double deltaTime){
	//// matrices setup
	//modelMatrix = mat4(1.0f); // identity
	//modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	//modelViewMatrix = viewMatrix * modelMatrix;
	//modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	//// set var MVP on the shader
	//shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	//shader.setUniform("useSingleColor", false);
}

void Plane::render(){
	shader.use();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//shader.setUniform("OutlineTex", 0);

	//////////////////////////
	////////////////////////// first draw: enable stencil write
	//////////////////////////

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	shader.setUniform("isOutline", false);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);

	//////////////////////////
	////////////////////////// second draw - scale the same object and draw again where the stencil mask is different than 1
	//////////////////////////

	//// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1, 1.1, 1));
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	shader.setUniform("isOutline", true);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);

}

void Plane::genBuffers() {

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(0);  // Vertex position -> layout 0 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, texUV.size() * sizeof(vec2), (GLvoid*)&texUV[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
	glEnableVertexAttribArray(1);  // texture coords -> layout 1 in the VS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Plane::resize(int x, int y) {

}

void Plane::genPlane() {

	// v0 -- top left
	vertices.push_back(vec3(-size, size, 0.0f));
	texUV.push_back(vec2(0.0f, 1.0f));

	//v1 -- bottom left
	vertices.push_back(vec3(-size, -size, 0.0f));
	texUV.push_back(vec2(0.0f, 0.0f));

	// v2 -- bottom right
	vertices.push_back(vec3(size, -size, 0.0f));
	texUV.push_back(vec2(1.0f, 0.0f));

	//v3 -- top right
	vertices.push_back(vec3(size, size, 0.0f));
	texUV.push_back(vec2(1.0f, 1.0f));

	// we'll have two triangles, one being v0,v1,v2 and the other v2,v3,v0
	// triangle 1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	//// triangle 2
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);


}
