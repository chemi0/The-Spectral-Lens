#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoord;

out vec2 fragTexCoord;

uniform vec2 uEntityPos;
uniform vec2 uEntitySize;

void main() {
	vec2 pos = inPos * uEntitySize + uEntityPos;

	pos = pos * 2.0 - 1.0;

	gl_Position = vec4(pos, 0.0, 1.0);
	fragTexCoord = inTexCoord;
}