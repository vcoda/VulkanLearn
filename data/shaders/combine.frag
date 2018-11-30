#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "uniform_layout.h"

layout (set = 3, binding = 2) uniform sampler2D TemporalResult[3];
layout (set = 3, binding = 3) uniform sampler2D BloomTextures[3];

layout (location = 0) out vec4 outCombineResult;

layout (location = 0) in vec2 inUv;

const float bloomMagnitude = 0.2f;
const float bloomExposure = 1.3f;

int index = int(perFrameData.camDir.a);


void main() 
{
	vec3 bloom = pow(texture(BloomTextures[index], inUv).rgb * bloomMagnitude, vec3(bloomExposure));
	vec3 temporal = texture(TemporalResult[index], inUv).rgb;

	outCombineResult = vec4(bloom + temporal, 1.0f);
}