#version 430 core
layout(location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

uniform mat4 uMVP;

out vec3 Color;

void main()
{
	Color = vColor;
	gl_Position = uMVP * vec4(vPosition, 1.0f);
}
