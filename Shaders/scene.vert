#version 330 core

layout(location = 0) in vec2 inPos; // Vertex position
layout (location = 1) in vec2 inTexCoord; // Texture coordinates

out vec2 fragTexCoord; // Pass texture coordinates to fragment shader

uniform mat4 uProjection; // Projection matrix

void main() {
	fragTexCoord = inTexCoord; // Pass through texture coordinates
	gl_Position = uProjection * vec4(inPos, 0.0, 1.0); // Transform vertex position
}