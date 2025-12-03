#version 330 core

in vec2 fragTexCoord;
out vec4 FragColor;

// Textures
uniform sampler2D uTexture; // Normal world
uniform sampler2D uHiddenTexture; // Hidden world

// Lens properties
uniform vec2 uMousePos;
uniform float uLensRadius; // radius (can be adjustable via MWHLUP/MWHLDWN)
uniform float uLensRevealProgress; // 0.0 -> 1.0 animation progress
uniform vec2 uWindowSize; // screen resolution for coordinare conversion

void main() {
	vec4 normalColor = texture (uTexture, fragTexCoord);
	vec4 hiddenColor = texture (uHiddenTexture, fragTexCoord);

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
	}

	FragColor = finalColor;
}