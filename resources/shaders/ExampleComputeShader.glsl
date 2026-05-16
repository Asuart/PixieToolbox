#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main() {
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(imgOutput);

    if (texelCoord.x < size.x && texelCoord.y < size.y) {
        vec2 uv = (vec2(texelCoord) + 0.5) / vec2(size);
        vec4 color = vec4(uv.x, uv.y, 0.0, 1.0);
        imageStore(imgOutput, texelCoord, color);
    }
}