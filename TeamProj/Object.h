#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma once

class Object
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	bool Load_Object(const char* path);
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 velocity;
	glm::vec3 RGB;
	bool isMovable;
	bool isGrounded;
	float mass;
	float friction;

public:
	GLuint VAO, VBO, EBO;
	
	Object() : 
		position(0.0f), 
		size(1.0f), 
		velocity(0.0f),
		isMovable(false),
		isGrounded(false),
		mass(1.0f),
		friction(0.5f) {}
	
	bool Set_Obj(GLuint shaderProgramID, const char* path);
	void Draw(GLuint shaderProgramID);
	void Update(float deltaTime, const std::vector<Object*>& staticObjects);
	void HandleCollision(Object* other, const glm::vec3& normal, float penetration);
	bool IsOnFloor(const std::vector<Object*>& staticObjects) const;
	bool CheckCollisionWithBox(const glm::vec3& otherPos, const glm::vec3& otherSize, glm::vec3& normal, float& penetration) const;

	glm::vec3 GetPosition() const { return position; }
	void SetPosition(const glm::vec3& pos) { position = pos; }
	
	glm::vec3 GetSize() const { return size; }
	void SetSize(const glm::vec3& s) { size = s; }

	glm::vec3 GetVelocity() const { return velocity; }
	void SetVelocity(const glm::vec3& vel) { velocity = vel; }

	bool IsMovable() const { return isMovable; }
	void SetMovable(bool movable) { isMovable = movable; }

	void SetRGB(glm::vec3 nRGB) { RGB = nRGB; }

	bool IsGrounded() const { return isGrounded; }
	void SetGrounded(bool grounded) { isGrounded = grounded; }

	float GetMass() const { return mass; }
	void SetMass(float m) { mass = m; }

	float GetFriction() const { return friction; }
	void SetFriction(float f) { friction = f; }
};