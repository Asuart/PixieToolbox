#version 430 core

in vec2 fTexCoord;

out vec4 color;

uniform sampler2D displayTexture;

void main() {
	vec4 pixel = texture(displayTexture, fTexCoord);
	color = vec4(pixel.rgb, 1.0f);
}
