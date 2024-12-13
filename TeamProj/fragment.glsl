#version 330 core

in vec3 passColorAttribute;
in vec2 TexCoord;

uniform sampler2D portalTexture;
uniform bool useTexture;

out vec4 FragColor;

void main()
{
	if (useTexture)
		FragColor = texture(portalTexture, TexCoord);
	else
		FragColor = vec4(passColorAttribute, 1.0);
};