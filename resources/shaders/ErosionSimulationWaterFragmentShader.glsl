#version 430 core

out vec4 FragColor;

in vec2 fTexCoords;
in vec3 fWorldPos;
in float fWaterHeight;

uniform vec3 uCameraPos;

void main() {
	vec3 toCamera = normalize(uCameraPos - fWorldPos);
	vec3 normal = normalize(cross(dFdx(fWorldPos), dFdy(fWorldPos)));

	float scale = dot(toCamera, normal);

	vec3 color = vec3(0.2f, 0.2f, 0.8f) * scale * 0.8f + vec3(0.2f, 0.2f, 0.8f) * 0.2f;

	FragColor = vec4(color, clamp(fWaterHeight / 1.0f, 0.0f, 0.9f));
}
