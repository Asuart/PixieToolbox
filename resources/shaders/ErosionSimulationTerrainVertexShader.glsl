#version 430 core

out vec2 fTexCoords;
out vec3 fWorldPos;
out vec3 fVertexColor;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 uv;

uniform sampler2D heightMap;

layout(std430, binding = 0) buffer terrain_height_buffer {
    float terrain_height[];
};

layout(std430, binding = 2) buffer water_height_buffer {
    float water_height[];
};

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

vec3 lert(vec3 from, vec3 to, float t) {
	float clampedT = min(1.0f, max(0.0f, t));
	return from * (1.0f - clampedT) + to * clampedT;
}

void main() {
	float terrainHeight = terrain_height[gl_VertexID];
	float waterHeight = water_height[gl_VertexID];
	float heightOffset = terrainHeight + waterHeight;

	fVertexColor = lert(vec3(0.2f, 0.8f, 0.2f), vec3(0.2f, 0.2f, 0.8f), waterHeight / 0.25f);
	fTexCoords = uv;
	fWorldPos = (mModel * vec4(pos.x, pos.y + terrainHeight, pos.z, 1.0f)).xyz;

    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
