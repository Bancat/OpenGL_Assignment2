//***************************************************************************
// DC-101090467 (C) 2018 All Rights Reserved.
//
// Assignment 2 submission.
//
// Description:
//	- Click run to see the results
//  - A & D keys - Move left right
//	- W & S keys - Move forward and backwards
//	- R & F keys - Move up and down
//*****************************************************************************

#include <iostream>
#include <list>
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "DC-101090467-Assignment1.h"

using namespace glm; // Removes the need to write glm:: (Not Recommended).

GLuint NUM_ROWS;
GLuint NUM_COLUMS;

//Buffer objects
GLuint VAO;

GLuint VertexBuffer;
GLuint ColorBuffer;
GLuint ElementBuffer;

//Matrices
std::list<mat4> modelViewStack;
mat4 projectionMatrix;
mat4 modelMatrix;
mat4 viewMatrix;

//Shader pointers
GLuint uMVP;

//Camera Variables
GLfloat cameraSpeed = 0.2f;
vec3 cameraPosition(0.0f, 0.0f, 6.0f);
vec3 initialCameraPosition = cameraPosition;

//Transformation Variables
GLfloat Degree = 0.0f;
GLfloat TransformBy = 0.0f;
GLfloat CurrentDepth = 1.0f;
GLfloat CurrentHight = -0.45f;
GLfloat rotationDelta = 2.0f;

//Keyboard Input
bool keys[256];

//Object data
GLfloat vertices[] =
{
	-0.9f, -0.9f,  0.9f,	//v0
	0.9f, -0.9f,  0.9f,	//v1
	0.9f,  0.9f,  0.9f,	//v2
	-0.9f,  0.9f,  0.9f,	//v3
	-0.9f, -0.9f, -0.9f,	//v4
	0.9f, -0.9f, -0.9f,	//v5
	0.9f,  0.9f, -0.9f,	//v6
	-0.9f,  0.9f, -0.9f,	//v7
};

GLfloat colors[] =
{
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f
};

GLuint indices[] =
{
	0, 1, 2, //front
	0, 2, 3,
	5, 4, 7, //back
	5, 7, 6,
	1, 5, 6, //right
	1, 6, 2,
	4, 0, 3, //left
	4, 3, 7,
	3, 2, 6, //top
	3, 6, 7,
	4, 5, 1, //bottom
	4, 1, 0
};

//Frame Rate Limiter
void RefreshFrame(int id) {
	CameraMovement();

	glutPostRedisplay();

	glutTimerFunc(33.3, RefreshFrame, 0);
}

//Camera Function
void CameraMovement()
{
	if (keys['w'])
	{
		if(cameraPosition.z >= -10)
		cameraPosition.z -= cameraSpeed;
	}
	else if (keys['s'])
	{
		if (cameraPosition.z <= 10)
		cameraPosition.z += cameraSpeed;
	}
	else if (keys['a'])
	{
		if (cameraPosition.x >= -10)
		cameraPosition.x -= cameraSpeed;
	}
	else if (keys['d'])
	{
		if (cameraPosition.x <= 10)
		cameraPosition.x += cameraSpeed;
	}
	else if (keys['r'])
	{
		if (cameraPosition.y <= 10)
		cameraPosition.y += cameraSpeed;
	}
	else if (keys['f'])
	{
		if (cameraPosition.y >= -10)
		cameraPosition.y -= cameraSpeed;
	}
	viewMatrix = lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

//Keyboard Functions
void onKeyDown(unsigned char key, int x, int y) {
	std::cout << key;
	switch (key)
	{
	case 'w':
	case 'W':
		keys['w'] = true;
		break;
	case 'a':
	case 'A':
		keys['a'] = true;
		break;
	case 's':
	case 'S':
		keys['s'] = true;
		break;
	case 'd':
	case 'D':
		keys['d'] = true;
		break;

	case 'r':
	case 'R':
		keys['r'] = true;
		break;

	case 'f':
	case 'F':
		keys['f'] = true;
		break;
	default:
		break;
	}
}

void onKeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
	case 'W':
		keys['w'] = false;
		break;
	case 'a':
	case 'A':
		keys['a'] = false;
		break;
	case 's':
	case 'S':
		keys['s'] = false;
		break;
	case 'd':
	case 'D':
		keys['d'] = false;
		break;
	case 'r':
	case 'R':
		keys['r'] = false;
		break;

	case 'f':
	case 'F':
		keys['f'] = false;
		break;
	default:
		break;
	}
}

//Pushes MVP matrix to shader program.
void uploadMatrixToShader()
{
	mat4 modelViewProjection = mat4(1.0f);
	modelViewProjection = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, value_ptr(modelViewProjection));
}

//Set up shader program, attributes and matrices.
void init()
{
	glEnable(GL_DEPTH_TEST);

	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VertexBuffer);

	glGenBuffers(1, &ColorBuffer);

	glGenBuffers(1, &ElementBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer); //Vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer); //Index list
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer); //Color buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	///uniforms
	uMVP = glGetUniformLocation(program, "uMVP");

	///Initializing matrices
	modelMatrix = mat4(1.0f);

	viewMatrix = lookAt(initialCameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	projectionMatrix = mat4(1.0f);
	projectionMatrix = perspective(radians(80.0f), 1.0f, 0.1f, 100.0f);

	uploadMatrixToShader();
}

//Cube Rendering
void Draw() {
	Degree += rotationDelta;
	int count = 0;

	for (GLuint i = 0; i < NUM_COLUMS; i++)
	{
	for (int x = 0; x <= NUM_ROWS;x++) { // How Many Colums

		modelMatrix = translate(modelMatrix, vec3(-3 + TransformBy, CurrentHight, CurrentDepth));

		if (x % 2 == 0) {
			modelMatrix = rotate(modelMatrix, radians(Degree), vec3(1.0f, 0.0f, 1.0f));
		}
		else {
			modelMatrix = rotate(modelMatrix, radians(-Degree), vec3(1.0f, 0.0f, 1.0f));
		}

		modelMatrix = scale(modelMatrix, vec3(0.3f, 0.3f, 0.3f));

		uploadMatrixToShader();

		modelMatrix = mat4(1.0f);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		CurrentDepth -= 1;

	}
	TransformBy += 3.0f;
	CurrentDepth = 1.0f;
	count++;
	if (count == 3)
	{
		CurrentHight += 1;
		TransformBy = 0;
		count = 0;
	}
	}
	modelMatrix = translate(modelMatrix, glm::vec3(0.0f, -2.0f, 0.0f));
	modelMatrix = scale(modelMatrix, glm::vec3(5.0f, 0.02f, 5.0f));
	uploadMatrixToShader();
	modelMatrix = mat4(1.0f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

//Render function.
void display()
{
	TransformBy = 0.0f;

	CurrentHight = -0.45f;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Draw();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	std::cout << "Please enter a number of Rows:" << std::endl;

	std::cin >> NUM_ROWS;

	std::cout << "Please enter a number of Colums:" << std::endl;

	std::cin >> NUM_COLUMS;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glCullFace(GL_BACK);

	glutInitWindowSize(1028, 1028);

	glutCreateWindow("Camacho_Diego_101090467");

	glewInit();

	init();

	glutDisplayFunc(display);

	glutKeyboardFunc(onKeyDown);

	glutKeyboardUpFunc(onKeyUp);

	glutTimerFunc(33.3, RefreshFrame, 0);

	glutMainLoop();
}