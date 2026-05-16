#version 430 core

out vec2 fTexCoord;

uniform vec2 uPos;
uniform vec2 uSize;

const vec2 pos[4] = vec2[4](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

const vec2 uv[4] = vec2[4](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

void main() {
	fTexCoord = uv[gl_VertexID];
	vec2 transformedPosition = vec2(pos[gl_VertexID].x * uSize.x + uPos.x, -pos[gl_VertexID].y * uSize.y - uPos.y) * 2.0 - vec2(1.0, -1.0);
	gl_Position = vec4(transformedPosition, 0.0, 1.0);
}
