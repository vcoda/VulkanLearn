#pragma once

#include "../Maths/Matrix.h"
#include "ChunkBasedUniforms.h"

class DescriptorSet;

typedef struct _PerObjectVariables
{
	Matrix4f modelMatrix;
	Matrix4f MVP;	// projection * view * model

	Matrix4f prevModelMatrix;
	Matrix4f prevMVP;
}PerObjectVariables;


class PerObjectUniforms : public ChunkBasedUniforms
{
protected:
	bool Init(const std::shared_ptr<PerObjectUniforms>& pSelf);

public:
	static std::shared_ptr<PerObjectUniforms> Create();

public:
	void SetModelMatrix(uint32_t index, const Matrix4f& modelMatrix);
	Matrix4f GetModelMatrix(uint32_t index) const { return m_perObjectVariables[index].modelMatrix; }
	Matrix4f GetMVP(uint32_t index) const { return m_perObjectVariables[index].MVP; }

	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_perObjectVariables[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_perObjectVariables); }

protected:
	PerObjectVariables		m_perObjectVariables[MAXIMUM_OBJECTS];
	std::vector<uint32_t>	m_dirtyChunks;
};