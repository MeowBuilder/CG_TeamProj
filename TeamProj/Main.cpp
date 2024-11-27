#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>

#include "Shader.h"
#include "Object.h"
#include "Player.h"

using namespace std;

const int WIN_X = 0, WIN_Y = 0;
const int WIN_W = 1920, WIN_H = 1080;

bool isCulling = true;

GLfloat mx = 0.0f;
GLfloat my = 0.0f;

Shader shader1;
Object Cube;
Player player(glm::vec3(0.0f, 0.0f, 0.0f));
bool firstMouse = true;
float lastX = WIN_W / 2.0f;
float lastY = WIN_H / 2.0f;

glm::vec3 cameraPos = { 0.0f,0.0f,1.0f };

glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// 키 입력 상태를 저장할 변수들 추가 (전역 변수로)
bool keys[256] = { false };

GLvoid drawScene()
{
	glClearColor(0,0,0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(shader1.shaderProgramID);

	// 투영 행렬 설정
	glm::mat4 projection = glm::mat4(1.0f);
	float aspectRatio = (float)WIN_W / (float)WIN_H;
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	unsigned int projectionLocation = glGetUniformLocation(shader1.shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	// 뷰 행렬 설정
	glm::mat4 view = player.GetCamera().GetViewMatrix();
	unsigned int viewLocation = glGetUniformLocation(shader1.shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// 큐브 그리기
	glm::vec3 cubePositions[] = {
		glm::vec3(3.0f, 0.0f, 3.0f),   // 오른쪽 앞
		glm::vec3(-3.0f, 0.0f, 3.0f),  // 왼쪽 앞
		glm::vec3(3.0f, 0.0f, -3.0f),  // 오른쪽 뒤
		glm::vec3(-3.0f, 0.0f, -3.0f)  // 왼쪽 뒤
	};

	for(int i = 0; i < 4; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		
		unsigned int modelLocation = glGetUniformLocation(shader1.shaderProgramID, "transform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

		unsigned int colorLocation = glGetUniformLocation(shader1.shaderProgramID, "colorAttribute");
		glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);  // 빨간색으로 변경

		Cube.Draw(shader1.shaderProgramID);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, WIN_W, WIN_H);
}

GLvoid TimerFunction(int value)
{
	float deltaTime = 0.016f;

	// WASD 키 입력에 따른 이동 처리
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

	if (glm::length(moveDir) > 0.0f) {
		moveDir = glm::normalize(moveDir);
		player.Move(moveDir, deltaTime);
	}

	player.Update(deltaTime);

	// 플레이어 위치 출력
	glm::vec3 pos = player.GetPosition();
	std::cout << "\rPosition - X: " << std::fixed << std::setprecision(2) << pos.x 
			  << " Y: " << pos.y 
			  << " Z: " << pos.z << std::flush;

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

// 키보드 떼는 것을 처리하는 함수 추가
GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	keys[key] = false;
	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	switch (key)
	{
	default:
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		 firstMouse = true;
	}
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	// 현재 마우스 위치와 화면 중앙의 차이를 계산
	float xoffset = x - (WIN_W / 2);
	float yoffset = (WIN_H / 2) - y;  // 반전된 y 좌표

	// 마우스 감도 적용 및 카메라 회전
	player.SetMouseLook(xoffset, yoffset);

	// 마우스를 화면 중앙으로 강제 이동
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

	Cube.Set_Obj(shader1.shaderProgramID, "cube.obj");

	// 초기 플레이어 위치 설정
	player.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(WIN_W/2, WIN_H/2);

	// 타이머 함수 최초 호출 추가
	glutTimerFunc(16, TimerFunction, 1);

	glutMainLoop();
}