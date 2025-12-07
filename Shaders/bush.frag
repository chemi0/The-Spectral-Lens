#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

uniform sampler2D uBushTexture;

// Lens properties
uniform vec2 uMousePos;
uniform float uLensRadius;
uniform float uLensRevealProgress;

uniform vec2 uWindowSize;

void main() {
	vec4 bushColor = texture(uBushTexture, fragTexCoord);

	if (bushColor.a < 0.01) {
		discard; // Fully discard transparent pixels
	}

	vec2 fragScreenPos = fragTexCoord * uWindowSize;
	fragScreenPos.y = uWindowSize.y - fragScreenPos.y; 

	float dist = distance(fragScreenPos, uMousePos);
	float featherSize = 30.0f;
	float mask = smoothstep(uLensRadius + featherSize, uLensRadius - featherSize, dist);
	mask *= uLensRevealProgress;

	if (mask <= 0.0) {
		discard; // Bush is only present on the hidden scenery
	}

	bushColor.a *= mask;

	FragColor = bushColor;

}