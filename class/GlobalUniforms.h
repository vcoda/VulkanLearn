#pragma once

#include "UniformDataStorage.h"

typedef struct _GlobalVariables
{
	Matrix4f	projection;
	Matrix4f	vulkanNDC;
	Matrix4f	NP;		// vulkanNDC * projection
}GlobalVariables;

class GlobalUniforms : public UniformDataStorage
{
public:
	void SetProjectionMatrix(const Matrix4f& proj);
	Matrix4f GetProjectionMatrix() const { return m_globalVariables.projection; }
	void SetVulkanNDCMatrix(const Matrix4f& vndc);
	Matrix4f GetVulkanNDCMatrix() const { return m_globalVariables.vulkanNDC; }
	Matrix4f GetNPMatrix() const { return m_globalVariables.NP; }

public:
	bool Init(const std::shared_ptr<GlobalUniforms>& pSelf);
	static std::shared_ptr<GlobalUniforms> Create();

	UniformVarList PrepareUniformVarList() override;

protected:
	void SyncBufferDataInternal() override;
	void SetDirty() override;

protected:
	GlobalVariables					m_globalVariables;
};