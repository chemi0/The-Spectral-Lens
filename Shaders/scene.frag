#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
	FragColor = texture(uTexture, fragTexCoord); // Just a simple texture for now, lens reveal logic will be here later
}