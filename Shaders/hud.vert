#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform vec2 uCharPos; 
uniform vec2 uCharSize;

void main() {
	vec2 pos = aPos * uCharSize + uCharPos;
	gl_Position = vec4(pos, 0.0, 1.0);
	vTexCoord = aTexCoord;
}