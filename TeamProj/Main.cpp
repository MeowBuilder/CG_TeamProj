#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>

#include "Shader.h"
#include "Player.h"
#include "Stage.h"

using namespace std;

const int WIN_X = 0, WIN_Y = 0;
const int WIN_W = 1920, WIN_H = 1080;

bool isCulling = true;

GLfloat mx = 0.0f;
GLfloat my = 0.0f;

Shader shader1;
Player player(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = WIN_W / 2.0f;
float lastY = WIN_H / 2.0f;

bool keys[256] = { false };

Stage* stage;

void InitStage() {
	stage = new Stage(&shader1, &player);
	stage->Initialize();
}

GLvoid drawScene(){
	glUseProgram(shader1.shaderProgramID);

	stage->RenderPortal();

	glViewport(0, 0, WIN_W, WIN_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	float aspectRatio = (float)WIN_W / (float)WIN_H;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 50.0f);
	unsigned int projectionLocation = glGetUniformLocation(shader1.shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glm::mat4 view = player.GetCamera().GetViewMatrix();
	unsigned int viewLocation = glGetUniformLocation(shader1.shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	stage->Render();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, WIN_W, WIN_H);
}

GLvoid TimerFunction(int value)
{
	float deltaTime = 0.016f;

	glm::vec3 moveDir(0.0f);
	
	glm::vec3 front = player.GetCamera().GetFront();
	front.y = 0.0f;
	front = glm::normalize(front);
	
	glm::vec3 right = glm::normalize(glm::cross(front, player.GetCamera().GetUp()));

	if (keys['w'] || keys['W'])
		moveDir += front;
	if (keys['s'] || keys['S'])
		moveDir -= front;
	if (keys['a'] || keys['A'])
		moveDir -= right;
	if (keys['d'] || keys['D'])
		moveDir += right;

	if (keys[' '] && player.IsGrounded()) {
		glm::vec3 vel = player.GetVelocity();
		vel.y = 8.0f;
		player.SetVelocity(vel);
		player.SetGrounded(false);
	}

	player.Move(moveDir, deltaTime);

	stage->Update(deltaTime);

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	keys[key] = true;

	switch (key) {
	case 'q': case 'Q':
		glutLeaveMainLoop();
		break;
	case ' ':
		player.Jump();
		break;
	}
	glutPostRedisplay();
}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	keys[key] = false;
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	float xoffset = x - (WIN_W / 2);
	float yoffset = (WIN_H / 2) - y;

	player.SetMouseLook(xoffset, yoffset);

	glutWarpPointer(WIN_W / 2, WIN_H / 2);

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutInitWindowSize(WIN_W, WIN_H);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	if (!shader1.Make_Shader_Program()) {
		cerr << "Error: Shader Program  " << endl;
		std::exit(EXIT_FAILURE);
	}

	InitStage();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(WIN_W/2, WIN_H/2);

	glutTimerFunc(16, TimerFunction, 1);

	glutMainLoop();
}