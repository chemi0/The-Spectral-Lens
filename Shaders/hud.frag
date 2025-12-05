#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uFontTexture;
uniform vec4 uTextColor;

void main() {
	float alpha = texture(uFontTexture, vTexCoord).r;
	FragColor = vec4(uTextColor.rgb, uTextColor.a * alpha);
}