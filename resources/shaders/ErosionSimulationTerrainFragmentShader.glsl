#version 430 core

out vec4 FragColor;

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fVertexColor;

uniform vec3 uCameraPos;

void main() {
	vec3 toCamera = normalize(uCameraPos - fWorldPos);
	vec3 normal = normalize(cross(dFdx(fWorldPos), dFdy(fWorldPos)));

	float scale = dot(toCamera, normal);

	vec3 color = fVertexColor * scale * 0.8f + fVertexColor * 0.2f;

	// color = color / (color + vec3(1.0f));
	// color = pow(color, vec3(1.0f / 2.2f));

	FragColor = vec4(color, 1.0f);
}
