#include "MotionTileMaxMaterial.h"
#include "../vulkan/RenderPass.h"
#include "../vulkan/GlobalDeviceObjects.h"
#include "../vulkan/CommandBuffer.h"
#include "../vulkan/PerFrameResource.h"
#include "../vulkan/Framebuffer.h"
#include "../vulkan/GlobalVulkanStates.h"
#include "../vulkan/SharedIndirectBuffer.h"
#include "../vulkan/DescriptorSet.h"
#include "../vulkan/DepthStencilBuffer.h"
#include "../vulkan/Texture2D.h"
#include "../vulkan/GraphicPipeline.h"
#include "../vulkan/SwapChain.h"
#include "../vulkan/DescriptorPool.h"
#include "../vulkan/DescriptorSetLayout.h"
#include "RenderPassBase.h"
#include "RenderPassDiction.h"
#include "RenderWorkManager.h"
#include "GBufferPass.h"
#include "FrameBufferDiction.h"
#include "../common/Util.h"

std::shared_ptr<MotionTileMaxMaterial> MotionTileMaxMaterial::CreateDefaultMaterial()
{
	SimpleMaterialCreateInfo simpleMaterialInfo = {};
	simpleMaterialInfo.shaderPaths = { L"../data/shaders/screen_quad.vert.spv", L"", L"", L"", L"../data/shaders/motion_tile_max.frag.spv", L"" };
	simpleMaterialInfo.vertexFormat = VertexFormatNul;
	simpleMaterialInfo.vertexFormatInMem = VertexFormatNul;
	simpleMaterialInfo.subpassIndex = 0;
	simpleMaterialInfo.frameBufferType = FrameBufferDiction::FrameBufferType_MotionTileMax;
	simpleMaterialInfo.pRenderPass = RenderPassDiction::GetInstance()->GetPipelineRenderPass(RenderPassDiction::PipelineRenderPassMotionTileMax);
	simpleMaterialInfo.depthTestEnable = false;
	simpleMaterialInfo.depthWriteEnable = false;

	std::shared_ptr<MotionTileMaxMaterial> pMotionTileMaxMaterial = std::make_shared<MotionTileMaxMaterial>();

	VkGraphicsPipelineCreateInfo createInfo = {};

	std::vector<VkPipelineColorBlendAttachmentState> blendStatesInfo;
	uint32_t colorTargetCount = FrameBufferDiction::GetInstance()->GetFrameBuffer(simpleMaterialInfo.frameBufferType)->GetColorTargets().size();

	for (uint32_t i = 0; i < colorTargetCount; i++)
	{
		blendStatesInfo.push_back(
			{
				VK_FALSE,							// blend enabled

				VK_BLEND_FACTOR_SRC_ALPHA,			// src color blend factor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,// dst color blend factor
				VK_BLEND_OP_ADD,					// color blend op

				VK_BLEND_FACTOR_ONE,				// src alpha blend factor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,// dst alpha blend factor
				VK_BLEND_OP_ADD,					// alpha blend factor

				0xf,								// color mask
			}
		);
	}

	VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
	blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendCreateInfo.logicOpEnable = VK_FALSE;
	blendCreateInfo.attachmentCount = blendStatesInfo.size();
	blendCreateInfo.pAttachments = blendStatesInfo.data();

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_FALSE;
	depthStencilCreateInfo.depthWriteEnable = VK_FALSE;

	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {};
	assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineMultisampleStateCreateInfo multiSampleCreateInfo = {};
	multiSampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pScissors = nullptr;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pViewports = nullptr;

	std::vector<VkDynamicState>	 dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo = {};
	dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStatesCreateInfo.dynamicStateCount = dynamicStates.size();
	dynamicStatesCreateInfo.pDynamicStates = dynamicStates.data();

	std::vector<VkVertexInputBindingDescription> vertexBindingsInfo;
	std::vector<VkVertexInputAttributeDescription> vertexAttributesInfo;
	if (simpleMaterialInfo.vertexFormat)
	{
		vertexBindingsInfo.push_back(GenerateBindingDesc(0, simpleMaterialInfo.vertexFormatInMem));
		vertexAttributesInfo = GenerateAttribDesc(0, simpleMaterialInfo.vertexFormat, simpleMaterialInfo.vertexFormatInMem);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = vertexBindingsInfo.size();
	vertexInputCreateInfo.pVertexBindingDescriptions = vertexBindingsInfo.data();
	vertexInputCreateInfo.vertexAttributeDescriptionCount = vertexAttributesInfo.size();
	vertexInputCreateInfo.pVertexAttributeDescriptions = vertexAttributesInfo.data();

	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pColorBlendState = &blendCreateInfo;
	createInfo.pDepthStencilState = &depthStencilCreateInfo;
	createInfo.pInputAssemblyState = &assemblyCreateInfo;
	createInfo.pMultisampleState = &multiSampleCreateInfo;
	createInfo.pRasterizationState = &rasterizerCreateInfo;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pDynamicState = &dynamicStatesCreateInfo;
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.renderPass = simpleMaterialInfo.pRenderPass->GetRenderPass()->GetDeviceHandle();
	createInfo.subpass = simpleMaterialInfo.subpassIndex;

	if (pMotionTileMaxMaterial.get() && pMotionTileMaxMaterial->Init(pMotionTileMaxMaterial, simpleMaterialInfo.shaderPaths, simpleMaterialInfo.pRenderPass, createInfo, simpleMaterialInfo.materialUniformVars, simpleMaterialInfo.vertexFormat, simpleMaterialInfo.vertexFormatInMem))
		return pMotionTileMaxMaterial;

	return nullptr;
}

bool MotionTileMaxMaterial::Init(const std::shared_ptr<MotionTileMaxMaterial>& pSelf,
	const std::vector<std::wstring>	shaderPaths,
	const std::shared_ptr<RenderPassBase>& pRenderPass,
	const VkGraphicsPipelineCreateInfo& pipelineCreateInfo,
	const std::vector<UniformVar>& materialUniformVars,
	uint32_t vertexFormat,
	uint32_t vertexFormatInMem)
{
	if (!Material::Init(pSelf, shaderPaths, pRenderPass, pipelineCreateInfo, materialUniformVars, vertexFormat, vertexFormatInMem))
		return false;

	std::vector<CombinedImage> motionVector;
	for (uint32_t j = 0; j < GetSwapChain()->GetSwapChainImageCount(); j++)
	{
		std::shared_ptr<FrameBuffer> pGBuffer = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_GBuffer)[j];

		motionVector.push_back({
			pGBuffer->GetColorTarget(FrameBufferDiction::MotionVector),
			pGBuffer->GetColorTarget(FrameBufferDiction::MotionVector)->CreateLinearClampToEdgeSampler(),
			pGBuffer->GetColorTarget(FrameBufferDiction::MotionVector)->CreateDefaultImageView()
		});
	}

	m_pUniformStorageDescriptorSet->UpdateImages(MaterialUniformStorageTypeCount, motionVector);

	return true;
}

void MotionTileMaxMaterial::CustomizeMaterialLayout(std::vector<UniformVarList>& materialLayout)
{
		m_materialVariableLayout.push_back(
			{
				CombinedSampler,
				"MotionVector",
				{},
				GetSwapChain()->GetSwapChainImageCount()
			});
}

void MotionTileMaxMaterial::CustomizePoolSize(std::vector<uint32_t>& counts)
{
	counts[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] += (GetSwapChain()->GetSwapChainImageCount());
}

void MotionTileMaxMaterial::AttachResourceBarriers(const std::shared_ptr<CommandBuffer>& pCmdBuffer, uint32_t pingpong)
{
	std::shared_ptr<Image> pMotionVector = FrameBufferDiction::GetInstance()->GetFrameBuffers(FrameBufferDiction::FrameBufferType_GBuffer)[FrameMgr()->FrameIndex()]->GetColorTarget(FrameBufferDiction::MotionVector);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = pMotionVector->GetImageInfo().mipLevels;
	subresourceRange.layerCount = pMotionVector->GetImageInfo().arrayLayers;

	VkImageMemoryBarrier imgBarrier = {};
	imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgBarrier.image = pMotionVector->GetDeviceHandle();
	imgBarrier.subresourceRange = subresourceRange;
	imgBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imgBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	pCmdBuffer->AttachBarriers
	(
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		{},
		{},
		{ imgBarrier }
	);
}