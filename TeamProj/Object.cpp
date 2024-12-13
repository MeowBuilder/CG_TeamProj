#include "Object.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool Object::Load_Object(const char* path) {
	vertexIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	vertices.clear();
	uvs.clear();
	normals.clear();

	ifstream in(path);
	if (!in) {
		cerr << path << "  ã";
		exit(1);
	}

	while (in) {
		string lineHeader;
		in >> lineHeader;
		if (lineHeader == "v") {
			glm::vec3 vertex;
			in >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}
		else if (lineHeader == "vt") {
			glm::vec2 uv;
			in >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (lineHeader == "vn") {
			glm::vec3 normal;
			in >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (lineHeader == "f") {
			char a;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			for (int i = 0; i < 3; i++)
			{
				in >> vertexIndex[i] >> a >> uvIndex[i] >> a >> normalIndex[i];
				vertexIndices.push_back(vertexIndex[i] - 1);
				uvIndices.push_back(uvIndex[i] - 1);
				normalIndices.push_back(normalIndex[i] - 1);
			}
		}
	}

	return true;
}

bool Object::Set_Obj(GLuint shaderProgramID, const char* path) {
	Load_Object(path);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

	GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
	if (positionAttribute == -1) {
		cerr << "position Ӽ  ";
		return false;
	}
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);

	return true;
}

void Object::Draw(GLuint shaderProgramID) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, size);
	
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

	unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
	
	glUniform3f(colorLocation, RGB.x, RGB.y, RGB.z);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Object::Update(float deltaTime)
{
	if (!isMovable) return;

	glm::vec3 prevPosition = position;

	const float GRAVITY = -9.8f;
	velocity.y += GRAVITY * deltaTime;

	const float MAX_VELOCITY = 8.0f;
	float currentSpeed = glm::length(velocity);
	if (currentSpeed > MAX_VELOCITY) {
		velocity = glm::normalize(velocity) * MAX_VELOCITY;
	}

	position += velocity * deltaTime;

	if (IsOnFloor()) {
		if (position.y - size.y/2 <= 0.0f) {
			position.y = size.y/2;
			velocity.y = 0.0f;
			isGrounded = true;
		}
	}

	if (isGrounded) {
		glm::vec2 horizontalVel(velocity.x, velocity.z);
		if (glm::length(horizontalVel) > 0.01f) {
			float frictionFactor = std::max(0.0f, 1.0f - friction * 2.0f * deltaTime);
			velocity.x *= frictionFactor;
			velocity.z *= frictionFactor;

			if (abs(velocity.x) < 0.1f) velocity.x = 0.0f;
			if (abs(velocity.z) < 0.1f) velocity.z = 0.0f;
		}
		else {
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
}

bool Object::CheckCollisionWithBox(const glm::vec3& otherPos, const glm::vec3& otherSize, glm::vec3& normal, float& penetration) const
{
	glm::vec3 diff = otherPos - position;
	
	glm::vec3 minDist = (size + otherSize) * 0.5f;
	glm::vec3 distances = glm::abs(diff) - minDist;

	if (distances.x < 0 && distances.y < 0 && distances.z < 0)
	{
		if (distances.x > distances.y && distances.x > distances.z)
		{
			normal = glm::vec3(diff.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
			penetration = -distances.x;
		}
		else if (distances.y > distances.x && distances.y > distances.z)
		{
			normal = glm::vec3(0.0f, diff.y > 0 ? 1.0f : -1.0f, 0.0f);
			penetration = -distances.y;
		}
		else
		{
			normal = glm::vec3(0.0f, 0.0f, diff.z > 0 ? 1.0f : -1.0f);
			penetration = -distances.z;
		}
		return true;
	}
	return false;
}

void Object::HandleCollision(Object* other, const glm::vec3& normal, float penetration)
{
	if (!isMovable) return;

	if (other && !other->IsMovable()) {
		position += normal * (penetration + 0.01f);

		float velDotNormal = glm::dot(velocity, normal);
		if (velDotNormal < 0) {
			velocity = velocity - (normal * velDotNormal * 1.5f);
				
			if (abs(normal.y) < 0.7f) {
				velocity.x *= 0.1f;
				velocity.z *= 0.1f;
			}
		}
	}
	else if (other && other->IsMovable()) {
		float totalMass = mass + other->GetMass();
		float massRatio = mass / totalMass;

		position += normal * penetration * (1.0f - massRatio);
		other->SetPosition(other->GetPosition() - normal * penetration * massRatio);

		glm::vec3 relativeVel = velocity - other->GetVelocity();
		float velAlongNormal = glm::dot(relativeVel, normal);

		if (velAlongNormal < 0) {
			float restitution = 0.3f;
			float j = -(1.0f + restitution) * velAlongNormal;
			j /= 1.0f / mass + 1.0f / other->GetMass();

			glm::vec3 impulse = normal * j;
			velocity += impulse / mass;
			other->SetVelocity(other->GetVelocity() - impulse / other->GetMass());
		}
	}

	float maxSpeed = 8.0f;
	float currentSpeed = glm::length(velocity);
	if (currentSpeed > maxSpeed) {
		velocity = glm::normalize(velocity) * maxSpeed;
	}
}

bool Object::IsOnFloor() const {
	const float FLOOR_SIZE = 10.0f;
	return (position.x >= -FLOOR_SIZE && position.x <= FLOOR_SIZE &&
			position.z >= -FLOOR_SIZE && position.z <= FLOOR_SIZE);
}