#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

uniform sampler2D uEntityTexture;
uniform float uLensRevealProgress;
uniform vec2 uMousePos;
uniform float uLensRadius;
uniform vec2 uWindowSize;
uniform vec2 uEntityPos;
uniform vec2 uEntitySize;
uniform float uIsFound; // 0.0 = not found, 1.0 = found

void main() {
	vec4 texColor = texture(uEntityTexture, fragTexCoord);

	if (texColor.a < 0.01) {
		discard; // Fully discard transparent pixels
	}

	// Calculate fragment's screen position
	vec2 worldUV = uEntityPos + fragTexCoord * uEntitySize;
	vec2 fragScreenPos = worldUV * uWindowSize;
	fragScreenPos.y = uWindowSize.y - fragScreenPos.y; // flip y (idek why there's a need for this but otherwise it flips the y axis)

	// Lens mask (Same logic as scene)
	float dist = distance(fragScreenPos, uMousePos);
	float featherSize = 30.0;
	float mask = smoothstep(uLensRadius + featherSize, uLensRadius - featherSize, dist);
	mask *= uLensRevealProgress;

	if (uIsFound > 0.5) {
		vec3 redTint = vec3(1.0, 0.2, 0.2);
		texColor.rgb = texColor.rgb * redTint;
		FragColor = texColor;
		return;
	}

	if (mask <= 0.0) {
		discard; // Only show entities inside the lens
	}

	texColor.a *= mask;

	FragColor = texColor;

}