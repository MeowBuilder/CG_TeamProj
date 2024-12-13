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
#include "Portal.h"

using namespace std;

const int WIN_X = 0, WIN_Y = 0;
const int WIN_W = 1920, WIN_H = 1080;

bool isCulling = true;
const int NUM_CUBES = 5;

GLfloat mx = 0.0f;
GLfloat my = 0.0f;

Shader shader1;
Object cubes[NUM_CUBES];
Player player(glm::vec3(0.0f, 0.0f, 0.0f));
bool firstMouse = true;
float lastX = WIN_W / 2.0f;
float lastY = WIN_H / 2.0f;

bool keys[256] = { false };

void RenderScene(GLuint shaderProgramID, bool skipPortals = false)
{
	for(int i = 0; i < NUM_CUBES; i++) {
		cubes[i].Draw(shaderProgramID);
	}
	player.Render(shaderProgramID);
}

GLvoid drawScene()
{
	glUseProgram(shader1.shaderProgramID);

	glViewport(0, 0, WIN_W, WIN_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	float aspectRatio = (float)WIN_W / (float)WIN_H;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	unsigned int projectionLocation = glGetUniformLocation(shader1.shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glm::mat4 view = player.GetCamera().GetViewMatrix();
	unsigned int viewLocation = glGetUniformLocation(shader1.shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	RenderScene(shader1.shaderProgramID, false);

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

	glm::vec3 prevPosition = player.GetPosition();
	player.SetGrounded(false);

	player.Move(moveDir, deltaTime);
	player.Update(deltaTime);

	for(int i = 0; i < NUM_CUBES; i++) {
		cubes[i].Update(deltaTime);
	}

	glm::vec3 pos = player.GetPosition();
	const float FLOOR_SIZE = 2.0f;
	bool isOnFloor = (pos.x >= -FLOOR_SIZE && pos.x <= FLOOR_SIZE &&
					 pos.z >= -FLOOR_SIZE && pos.z <= FLOOR_SIZE);

	if (isOnFloor && pos.y - player.GetColliderSize().y/2 <= 0.0f) {
		pos.y = player.GetColliderSize().y/2;
		player.SetPosition(pos);
		if (player.GetVelocity().y < 0) {
			player.SetGrounded(true);
			glm::vec3 vel = player.GetVelocity();
			vel.y = 0.0f;
			player.SetVelocity(vel);
		}
	}

	for(int i = 0; i < NUM_CUBES; i++) {

		for(int j = i + 1; j < NUM_CUBES; j++) {

			glm::vec3 normal;
			float penetration;
			
			if (cubes[i].CheckCollisionWithBox(cubes[j].GetPosition(), cubes[j].GetSize(), normal, penetration)) {
				if (cubes[i].IsMovable() && !cubes[j].IsMovable()) {
					cubes[i].HandleCollision(&cubes[j], -normal, penetration);
				}
				else if (!cubes[i].IsMovable() && cubes[j].IsMovable()) {
					cubes[j].HandleCollision(&cubes[i], normal, penetration);
				}
				else if (cubes[i].IsMovable() && cubes[j].IsMovable()) {
					cubes[i].HandleCollision(&cubes[j], -normal, penetration);
					cubes[j].HandleCollision(&cubes[i], normal, penetration);
				}
			}
		}
	}

	for(int i = 0; i < NUM_CUBES; i++) {
		glm::vec3 normal;
		float penetration;
		
		if (cubes[i].CheckCollisionWithBox(player.GetPosition(), player.GetColliderSize(), normal, penetration)) {
			if (cubes[i].IsMovable()) {
				glm::vec3 pushVelocity = player.GetVelocity();
				if (glm::length(pushVelocity) > 0.1f) {
					pushVelocity.y = 0.0f;
					cubes[i].SetVelocity(cubes[i].GetVelocity() + pushVelocity);
				}
			}

			glm::vec3 correction;
			if (normal.y > 0.7f) {
				correction = glm::vec3(0.0f, penetration, 0.0f);
				player.SetPosition(player.GetPosition() + correction);
				
				if (player.GetVelocity().y < 0) {
					player.SetGrounded(true);
					glm::vec3 vel = player.GetVelocity();
						vel.y = 0.0f;
					player.SetVelocity(vel);
				}
			}
			else {
				player.SetPosition(prevPosition);
				glm::vec3 vel = player.GetVelocity();
				float velDotNormal = glm::dot(vel, normal);
				if (velDotNormal < 0) {
					vel = vel - (normal * velDotNormal);
					if (abs(normal.y) < 0.7f) {
						vel.x *= 0.1f;
						vel.z *= 0.1f;
					}
				}
				player.SetVelocity(vel);
			}
		}
	}

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
	if (state == GLUT_DOWN)
	{
		 firstMouse = true;
	}
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

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, -0.5f, 0.0f),  // ù ��° ����
	glm::vec3(50.0f, -0.5f, 0.0f)  // �� ��° ���� (�ָ� ������ ��ġ)
	};

	glm::vec3 cubeSizes[] = {
		glm::vec3(10.0f, 1.0f, 10.0f),   // ù ��° ���� ũ��
		glm::vec3(10.0f, 1.0f, 10.0f)    // �� ��° ���� ũ��
	};

	

	for(int i = 0; i < NUM_CUBES; i++) {
		cubes[i].Set_Obj(shader1.shaderProgramID, "cube.obj");
		cubes[i].SetPosition(cubePositions[i]);
		cubes[i].SetSize(cubeSizes[i]);
		cubes[i].SetRGB(glm::vec3(1.0f));
		
		if (i <= 1) {
			cubes[i].SetMovable(false);
		}
		else {
			cubes[i].SetMovable(true);
			cubes[i].SetMass(1.0f);
		}
	}

	player.SetPosition(glm::vec3(0.0f, 0.6f, 0.0f));  // ù ��° ���� �� ����
	
	if (!player.InitializeBuffers()) {
		cerr << "Error: Player Buffer Initialization Failed" << endl;
		std::exit(EXIT_FAILURE);
	}

	// ��Ż ��ġ
	portal1.SetPosition(glm::vec3(0.0f, 1.0f, -2.0f));  // ù ��° ���� �� ��Ż
	portal2.SetPosition(glm::vec3(50.0f, 1.0f, 2.0f)); // �� ��° ���� �� ��Ż

	// ��Ż ũ�� �� ���� ����
	portal1.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	portal2.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	portal1.SetSize(glm::vec3(2.0f, 3.2f, 0.2f));
	portal2.SetSize(glm::vec3(2.0f, 3.2f, 0.2f));
	portal1.LinkPortal(&portal2);
	portal2.LinkPortal(&portal1);

	if (!portal1.InitializeBuffers() || !portal2.InitializeBuffers()) {
		cerr << "Error: Portal Buffer Initialization Failed" << endl;
		std::exit(EXIT_FAILURE);
	}

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