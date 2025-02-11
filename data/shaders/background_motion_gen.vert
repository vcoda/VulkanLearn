#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec2 outUv;
layout (location = 1) out vec3 prevClipSpacePos;
layout (location = 2) out vec3 currClipSpacePos;

#include "uniform_layout.sh"

void main() 
{
	outUv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

	gl_Position = vec4(outUv * 2.0f - 1.0f, 0.00001f, 1.0f);

	vec3 viewSpacePos = vec3((perFrameData.eyeSpaceSize.xy / 2.0f) * gl_Position.xy, -perFrameData.nearFarAB.x);
	vec4 worldSpacePos = perFrameData.viewCoordSystem * vec4(viewSpacePos, 0.0f);
	vec3 prevViewSpacePos = (perFrameData.prevView * worldSpacePos).xyz;
	prevClipSpacePos = (globalData.projection * vec4(prevViewSpacePos, 1.0f)).xyw;
	currClipSpacePos = (globalData.projection * vec4(viewSpacePos, 1.0f)).xyw;
	gl_Position.y *= -1.0f;
	outUv.y = 1.0f - outUv.y;
}
