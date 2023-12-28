#version 450

layout(push_constant) uniform PushConst {
    mat4 mvp;
    int drawId;
} pushBlock;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out float drawId;

void main()
{
	fragPos = inPosition;
    drawId = pushBlock.drawId;
    gl_Position = pushBlock.mvp * vec4(inPosition, 1.0);
}
