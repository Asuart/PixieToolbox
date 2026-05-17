#version 430 core

out vec2 fTexCoords;
out vec3 fWorldPos;
out float fWaterHeight;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 uv;

layout(std430, binding = 0) buffer terrain_height_buffer {
    float terrain_height[];
};

layout(std430, binding = 2) buffer water_height_buffer {
    float water_height[];
};

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

void main() {
	float terrainHeight = terrain_height[gl_VertexID];
	float waterHeight = water_height[gl_VertexID];
	float heightOffset = terrainHeight + waterHeight;

	fTexCoords = uv;
	fWorldPos = (mModel * vec4(pos.x, pos.y + heightOffset, pos.z, 1.0f)).xyz;
	fWaterHeight = waterHeight;

    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
