#version 430 core

out vec2 fTexCoords;
out vec3 fWorldPos;
out vec3 fVertexColor;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 uv;

layout(std430, binding = 0) buffer terrain_height_buffer {
    float terrain_height[];
};

layout(std430, binding = 2) buffer water_height_buffer {
    float water_height[];
};

layout(std430, binding = 9) buffer velocity_x_buffer {
    float velocity_x[];
};

layout(std430, binding = 10) buffer velocity_y_buffer {
    float velocity_y[];
};

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;
uniform float uDeltaTime;

void main() {
	float terrainHeight = terrain_height[gl_VertexID];

	fTexCoords = uv;
	fWorldPos = (mModel * vec4(pos.x, pos.y + terrainHeight, pos.z, 1.0f)).xyz;
    fVertexColor = vec3(0.2f, 0.8f, 0.2f);
    if ((velocity_x[gl_VertexID] * uDeltaTime) > 1.0f) {
        fVertexColor = vec3(1.0f, 0.0f, 0.0f);
    }
    if ((velocity_y[gl_VertexID] * uDeltaTime) > 1.0f) {
        fVertexColor = vec3(1.0f, 0.0f, 0.0f);
    }

    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
