// Basic Tessellation Demo
//
// Este programa é uma demonstrção da utilização de Tessellation em OpenGL.
// Um quadrado é enviado para GPU e subdividido em tempo real. O usuário pode
// controlar o número de subdivisões utilizando as teclas 'Q', 'A', 'W', 'S', 
//'E', 'D', 'R', 'F', 'T' e 'G'.
//
// Abril 2016 - Alex Frasson - afrasson@inf.ufsm.br

//Include GLEW - always first 
#include "GL/glew.h"
#include <GLFW/glfw3.h>

//Include the standard C++ headers 
#include "Scene.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include "TessellatedQuad.h"


#define WINDOW_WIDTH	1000
#define WINDOW_HEIGHT	1000


using namespace std;

Scene *tessellatedQuad;
GLFWwindow* window;


void mainLoop()
{
	double thisTime;
	double lastTime = glfwGetTime();
	do
	{
		// set deltatime and call update
		thisTime = glfwGetTime();
		tessellatedQuad->update(thisTime - lastTime);
		lastTime = thisTime;

		tessellatedQuad->render();

		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));
}

// Define an error callback  
void errorCallback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}
// Key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Initialize GLFW  
void initGLFW()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLSL4.3 + GLM + VBO + VAO", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		system("pause");
		exit(EXIT_FAILURE);
	}
}
void initCallbacks()
{
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetErrorCallback(errorCallback);
}
void initGLEW()
{
	// Initialize GLEW
	glewExperimental = GL_TRUE; //ensures that all extensions with valid entry points will be exposed.
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		system("pause");
		exit(EXIT_FAILURE);
	}
}
void initializeGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
// Close OpenGL window and terminate GLFW  
void closeApplication()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(void)
{
	initGLFW();
	initCallbacks();
	initGLEW();
	initializeGL();

	tessellatedQuad = new TessellatedQuad(window, 1);
	tessellatedQuad->init();

	cout << endl << "Q: increase inner" << endl;
	cout << "A: decrease inner" << endl;
	cout << "W: increase outer 0" << endl;
	cout << "S: decrease outer 0" << endl;
	cout << "E: increase outer 1" << endl;
	cout << "D: decrease outer 1" << endl;
	cout << "R: increase outer 2" << endl;
	cout << "F: decrease outer 2" << endl;
	cout << "T: increase outer 3" << endl;
	cout << "G: decrease outer 3" << endl;

	mainLoop();

	exit(EXIT_SUCCESS);
}