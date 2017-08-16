#pragma once
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "common\Texture.h"
#include "common\Controls.hpp"
#include "common\objloader.h"

static GLuint vertexbuffer;
static GLuint uvbuffer;
static GLuint programID;
static GLuint MatrixID;
static GLuint Texture;
static GLuint TextureID;
static int noOfVertices;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

int GlewCheck()
{
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GlEW\n");
		return -1;
	}
	return 0;
}

void Start()
{
	if (GlewCheck() < 0)
	{
		return;
	}

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// draw our static object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("Objects\\chest_final.obj", vertices, uvs, normals);

	glm::vec2 pointuv1(uvs[0].x * 512, uvs[0].y * 512);
	glm::vec2 pointuv2(uvs[1].x * 512, uvs[1].y * 512);
	glm::vec2 pointuv3(uvs[2].x * 512, uvs[2].y * 512);

	// Load it into a VBO
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	noOfVertices = vertices.size();
	glBufferData(GL_ARRAY_BUFFER, noOfVertices * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);


	// load the uv coordinates for every face
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, noOfVertices * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// create and compile our GLSL program from the shaders
	programID = LoadShaders("shaders\\SimpleVertexShader5.vertexshader", "shaders\\SimpleFragmentShader5.fragmentshader");

	Texture = loadDDS("textures/chest4.dds");

	// get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
}

void Update(GLFWwindow* window)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs(window);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	// send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
	// vertex bufffer configration
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	// color buffer configration
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1, // attribute. No particular reason for 1, but must match the layout in the shader
		2, // size
		GL_FLOAT,// type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, noOfVertices); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
	glDisableVertexAttribArray(0);
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(VertexShaderStream, Line))
		{
			VertexShaderCode += "\n" + Line;
		}
		VertexShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. are you in the right directory? don't forget to read the FAQ!\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(FragmentShaderStream, Line))
		{
			FragmentShaderCode += "\n" + Line;
		}
		FragmentShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. are you in the right directory? don't forget to read the FAQ!\n", fragment_file_path);
		getchar();
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);

	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}