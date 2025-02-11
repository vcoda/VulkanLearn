#include "ComputePipeline.h"
#include "PipelineLayout.h"
#include "ShaderModule.h"
#include <fstream>

ComputePipeline::~ComputePipeline()
{
	vkDestroyPipeline(GetDevice()->GetDeviceHandle(), m_pipeline, nullptr);

	delete[] m_shaderStageInfo.pName;
}

bool ComputePipeline::Init(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ComputePipeline>& pSelf, const VkComputePipelineCreateInfo& info)
{
	if (!DeviceObjectBase::Init(pDevice, pSelf))
		return false;

	m_info = info;

	m_shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_shaderStageInfo.stage = m_pShaderModule->GetShaderStage();
	m_shaderStageInfo.module = m_pShaderModule->GetDeviceHandle();

	char* pEntryName = new char[ENTRY_NAME_LENGTH];
	strcpy(pEntryName, m_pShaderModule->GetEntryName().c_str());
	m_shaderStageInfo.pName = pEntryName;

	m_info.stage = m_shaderStageInfo;

	CHECK_VK_ERROR(vkCreateComputePipelines(m_pDevice->GetDeviceHandle(), 0, 1, &m_info, nullptr, &m_pipeline));

	return true;
}

std::shared_ptr<ComputePipeline> ComputePipeline::Create
(
	const std::shared_ptr<Device>& pDevice,
	const VkComputePipelineCreateInfo& info,
	const std::shared_ptr<ShaderModule>& pShader,
	const std::shared_ptr<PipelineLayout>& pPipelineLayout
)
{
	std::shared_ptr<ComputePipeline> pPipeline = std::make_shared<ComputePipeline>();

	pPipeline->m_pShaderModule = pShader;
	pPipeline->m_pPipelineLayout = pPipelineLayout;

	VkComputePipelineCreateInfo createInfo = info;
	createInfo.layout = pPipelineLayout->GetDeviceHandle();

	if (pPipeline.get() && pPipeline->Init(pDevice, pPipeline, createInfo))
		return pPipeline;
	return nullptr;
}