#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 3, binding = 1) uniform samplerCube tex0;
layout (set = 3, binding = 2) uniform sampler2D tex1;

layout (location = 0) in vec2 inUv;

layout (location = 0) out vec4 outFragColor;

#include "uniform_layout.sh"

void main() 
{
	vec3 final = texture(tex0, normalize(vec3((inUv.st - 0.5) * 2.0, 1.0)), 3.0).xyz;
	
	final = Uncharted2Tonemap(final * globalData.GEW.y);
	final = final * (1.0 / Uncharted2Tonemap(vec3(globalData.GEW.z)));
	final = pow(final, vec3(globalData.GEW.x));

	outFragColor = vec4(final, 1.0);
}