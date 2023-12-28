#version 450

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D tex;

void main() 
{

	vec2 uv = inUV;

	outColor = texture(tex,uv);
}