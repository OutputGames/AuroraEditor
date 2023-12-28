#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 camPos;
} transformationBuffer;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 camPos;

void main() {
    gl_Position = transformationBuffer.proj * transformationBuffer.view * transformationBuffer.model * vec4(inPosition, 1.0);

    //mat4 rotView = mat4(mat3(transformationBuffer.view));
    //vec4 clipPos = transformationBuffer.proj * rotView * vec4(inPosition, 1.0);

    //gl_Position = clipPos.xyww;

    //gl_Position = vec4(inPosition,1.0);
    fragNormal = transpose(inverse(mat3(transformationBuffer.model))) * inNormal;

    //fragNormal = inNormal;

    fragTexCoord = inTexCoord;

    fragPos = vec3(transformationBuffer.model * vec4(inPosition,1.0));
    camPos = vec3(transformationBuffer.camPos);
}