#version 330 core

in vec3 passColorAttribute;
in vec2 TexCoord;

uniform sampler2D portalTexture;
uniform sampler2D mainTexture;
uniform bool useTexture;
uniform bool isPortal;

out vec4 FragColor;

void main()
{
	if (isPortal && useTexture)
		FragColor = texture(portalTexture, TexCoord);
	else if (useTexture)
		FragColor = texture(mainTexture, TexCoord);
	else
		FragColor = vec4(passColorAttribute, 1.0);
};