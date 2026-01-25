#version 330 core
out vec4 FragColor;

in vec3 VertexColor;
in vec3 FragPos; 
in vec3 ViewPos; // Camera position

uniform vec3 uColor; // Object's base color

void main() {

	// Distance from Camera to Fragment
	float distance = length(ViewPos - FragPos);

	// Fog Settings
	vec3 fogColor = vec3(0.1f, 0.15f, 0.12f); 
	float fogStart = 10.0f;
	float fogEnd = 40.0f; // Fully opaque at 40 units

	// Fog Factor calculation
	float fogFactor = (distance - fogStart) / (fogEnd - fogStart);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);

	// Object Color + Fog Color
	vec3 finalColor = mix(uColor, fogColor, fogFactor);

	FragColor = vec4(finalColor, 1.0);
}