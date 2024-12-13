#version 330 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec2 texCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

uniform vec3 colorAttribute;
out vec3 passColorAttribute;
out vec2 TexCoord;

void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(positionAttribute, 1.0);
	passColorAttribute = colorAttribute;
	TexCoord = texCoord;
};