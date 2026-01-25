#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // Just using vertex colors for now

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 uColor; 

out vec3 VertexColor;
out vec3 FragPos; // World Position
out vec3 ViewPos; // Camera Position

uniform vec3 cameraPos;

void main() {

	vec4 worldPosition = model * vec4(aPos, 1.0);
	FragPos = vec3(worldPosition);
	ViewPos = cameraPos;

	gl_Position = projection * view * worldPosition;
	VertexColor = uColor;
}