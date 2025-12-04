#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

uniform sampler2D uMoonTexture;
uniform float uTime;

void main() {
	vec4 moonColor = texture(uMoonTexture, fragTexCoord);

	if (moonColor.a < 0.01) {
		discard; // only process pixels with alpha > 0 (meaning only the Moon), skip transparent pixels
	}

	float glow = 0.5 + 0.5 * sin(uTime * 2.0); // Pulsating glow

	moonColor.rgb *= (1.0 + glow * 0.5); // Increase brightness based on glow, to 150%

	vec3 glowColor = vec3(1.0, 1.0, 0.9) * glow * 0.3; // aura
	moonColor.rgb += glowColor;

	moonColor.rgb = clamp(moonColor.rgb, 0.0, 1.0); // clamping to prevent oversaturation

	FragColor = moonColor;
}