#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

// Textures
uniform sampler2D uTexture; // Normal world
uniform sampler2D uHiddenTexture; // Hidden world
uniform sampler2D uSnowTexture; // Snow texture for falling snow effect
uniform sampler2D uMoonTexture;
uniform sampler2D uFontTexture;

// Lens properties
uniform vec2 uMousePos;
uniform float uLensRadius; // radius (can be adjustable via MWHLUP/MWHLDWN)
uniform float uLensRevealProgress; // 0.0 -> 1.0 animation progress
uniform float uHiddenText; // 1.0 -> show, 0.0 -> hide
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
			
		// Hidden Text
		if (uHiddenText > 0.5 && mask > 0.0) {
	
			vec2 sigPos = vec2(0.02, 0.88); // Top-left corner
			float charScale = 0.015;
			float spacing = charScale * 1.2;

			int text[27];
			// MILAN SPASOJEVIC RA147/2022
			text[0] = 23;  // M
			text[1] = 19;  // I
			text[2] = 22;  // L
			text[3] = 11;  // A
			text[4] = 24;  // N
			text[5] = 38;  // (space)
			text[6] = 29;  // S
			text[7] = 26;  // P
			text[8] = 11;  // A
			text[9] = 29;  // S
			text[10] = 25; // O
			text[11] = 20; // J
			text[12] = 15; // E
			text[13] = 32; // V
			text[14] = 19; // I
			text[15] = 13; // C
			text[16] = 38;; // (space)
			text[17] = 28; // R
			text[18] = 11; // A
			text[19] = 1;  // 1
			text[20] = 4;  // 4
			text[21] = 7;  // 7
			text[22] = 10; // /
			text[23] = 2;  // 2
			text[24] = 0;  // 0
			text[25] = 2;  // 2
			text[26] = 2;  // 2

			float totalTextWidth = 27.0 * spacing;
			float textHeight = charScale * 1.5;
	
			// Check if current fragment is within signature bounds
			if (fragTexCoord.x >= sigPos.x && fragTexCoord.x <= sigPos.x + totalTextWidth &&
				fragTexCoord.y >= sigPos.y && fragTexCoord.y <= sigPos.y + textHeight) {
		
				// Calculate which character wer on
				float relX = fragTexCoord.x - sigPos.x;
				int charIndex = int(relX / spacing);

				if (charIndex >= 0 && charIndex < 27) {
					float charStartX = float(charIndex) * spacing;
					float localX = (relX - charStartX) / charScale;
					float localY = 1.0 - (fragTexCoord.y - sigPos.y) / textHeight;

					// Local UV within the character quad
					if (localX >= 0.0 && localX <= 1.0 && localY >= 0.0 && localY <= 1.0) {
						int atlasIndex = text[charIndex];
						float charWidth = 1.0 / 39.0;
						float atlasU = (float(atlasIndex) + localX) * charWidth;
						float atlasV = localY;

						float charValue = texture(uFontTexture, vec2(atlasU, atlasV)).r;

						if (charValue > 0.1) {
							// Glow effect
							vec3 baseColor = vec3(0.4, 0.8, 1.0);
							vec3 glowColor = vec3(0.7, 0.95, 1.0);

							float pulse = 0.7 + 0.3 * sin(uTime * 2.0); // pulsating effect

							float glow = smoothstep(0.1, 0.5, charValue); // soft glow around the text
							vec3 textColor = mix(glowColor, baseColor, glow);

							float alpha = charValue * pulse * mask * 0.9;
							finalColor.rgb = mix(finalColor.rgb, textColor, alpha);
						}
					}
				}
			}
		}
	}
	FragColor = finalColor;
}