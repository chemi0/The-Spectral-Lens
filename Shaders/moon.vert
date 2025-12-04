#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoord;

out vec2 fragTexCoord;

uniform mat4 uProjection;

void main() {
	fragTexCoord = inTexCoord;
	gl_Position = uProjection * vec4(inPos, 0.0, 1.0);
}