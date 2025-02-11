#pragma once

#include "../Maths/Matrix.h"
#include "UniformBase.h"

class DescriptorSet;
class Texture2D;

class GBufferInputUniforms : public UniformBase
{
public:
	bool Init(const std::shared_ptr<GBufferInputUniforms>& pSelf);
	static std::shared_ptr<GBufferInputUniforms> Create();

public:
	virtual std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;
};