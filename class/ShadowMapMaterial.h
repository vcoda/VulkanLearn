#pragma once
#include "Material.h"

class ShadowMapMaterial : public Material
{
public:
	static std::shared_ptr<ShadowMapMaterial> CreateDefaultMaterial(bool skinned = false);

public:
	void Dispatch(const std::shared_ptr<CommandBuffer>& pCmdBuf, const Vector3f& groupNum, const Vector3f& groupSize, uint32_t pingpong = 0) override {}
};