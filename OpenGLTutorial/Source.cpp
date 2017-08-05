#include <stdio.h>
#include <stdlib.h>

#include "Tut5.h"
#include <GLFW\glfw3.h>


int MainProgram()
{
	if (!glfwInit())
	{

		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Hey Nader", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create a window try different version of openGL.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	Start();

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



	do
	{
		// Draw nothing, see you in tutorial 2 !
		Update();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;

}

int main()
{
	return MainProgram();
}