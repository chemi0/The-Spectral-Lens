#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

// Textures
uniform sampler2D uTexture; // Normal world
uniform sampler2D uHiddenTexture; // Hidden world
uniform sampler2D uSnowTexture; // Snow texture for falling snow effect
uniform sampler2D uMoonTexture;

// Lens properties
uniform vec2 uMousePos;
uniform float uLensRadius; // radius (can be adjustable via MWHLUP/MWHLDWN)
uniform float uLensRevealProgress; // 0.0 -> 1.0 animation progress
uniform vec2 uWindowSize; // screen resolution for coordinare conversion

uniform float uTime; // Elapsed time for the snow falling animation

void main() {
	vec4 normalColor = texture (uTexture, fragTexCoord);
	vec4 hiddenColor = texture (uHiddenTexture, fragTexCoord);

	// Main background fog effect using sin waves
	float fogNoise1 = sin(fragTexCoord.x * 3.0 + uTime * 0.3) * 0.5 + 0.5; // 0.0 to 1.0 x
	float fogNoise2 = sin(fragTexCoord.y * 2.0 + uTime * 0.2) * 0.5 + 0.5; // 0.0 to 1.0 y
	float fogDensity = (fogNoise1 + fogNoise2) * 0.7; // 0.0 to 0.3 density

	vec3 fogColor = vec3(0.569, 0.506, 0.506);
	normalColor.rgb = mix(normalColor.rgb, fogColor, fogDensity);

	vec4 finalColor = normalColor; // By default shows normal world

	if (uLensRevealProgress > 0.0) {
		vec2 fragScreenPos = fragTexCoord * uWindowSize; // fragment pos from uv to screen space

		fragScreenPos.y = uWindowSize.y - fragScreenPos.y; // Flip y axis to match the mouse pos

		float dist = distance(fragScreenPos, uMousePos); // distance from fragment to mouse pos

		// circular mask using smoothstep
		float featherSize = 30.0;
		float mask = smoothstep(uLensRadius + featherSize, uLensRadius - featherSize, dist);

		mask *= uLensRevealProgress; // Multiplying the mask by reveal progress for a smooth fade in/out
		/* Blend between normal and hidden based on the mask; mask = 0.0 -> normal world, mask = 1.0 -> hidden world
		Basically this means that when the dist is < (radius - 30) it returns 1.0 and that means it's inside of the lens and it shows the hidden world
		And when the dist is > (radius + 30) it returns 0.0 meaning its outside of the lens, aka shows the normal world;
		In between the values are blended smoothly
		*/
		finalColor = mix(normalColor, hiddenColor, mask);


		 // Snowing effect (only within the secret landscape)
		if (mask > 0.0) {
			vec2 snowUV = fragTexCoord;
			snowUV.y += uTime * 0.03; // scrolling down the snow texture over time (3% speed)

			vec4 snow = texture(uSnowTexture, snowUV);

			finalColor.rgb = mix(finalColor.rgb, vec3(1.0), snow.a * 0.7 * mask); // Blending the snow with the mask
		}

		// Moon effect (moved to here to consitently have the Moon hidden from the normal world)
		if (mask > 0.0) {
			vec2 moonCenter = vec2(0.85, 0.85);
			float moonRadius = 0.15;

			float r = distance (fragTexCoord, moonCenter) / moonRadius;

			float radialGlow = exp(-r * r * 0.5); // Gaussian-like profile for a radial glow fallow
			float glowPulse = 0.6 + 0.4 * sin(uTime * 1.5); // Pulsating glow

			// aura
			vec3 glowCol = vec3(1.0, 0.96, 0.88) * glowPulse * radialGlow * 0.45 * mask;
			finalColor.rgb += glowCol;

			// Sampling the moon texture only within its disc
			if (r <= 1.2) {
				vec2 moonUV = (fragTexCoord - moonCenter) / (moonRadius * 2.0) + 0.5;

				if (moonUV.x >= 0.0 && moonUV.x <= 1.0 && moonUV.y >= 0.0 && moonUV.y <= 1.0) {
					vec4 moonTex = texture(uMoonTexture, moonUV);

					float edgeFeather = 1.0 - smoothstep(0.95, 1.0, r); // Feathering the edge of the moon to avoid borders

					moonTex.rgb *= (1.0 + glowPulse * 0.3); // Increase brightness slightly based on gli

					float alpha = moonTex.a * edgeFeather * mask;

					finalColor.rgb = mix(finalColor.rgb, moonTex.rgb, alpha);
					}
			}
		}
	}

	FragColor = finalColor;
}