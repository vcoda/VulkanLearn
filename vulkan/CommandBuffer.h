#pragma once

#include "DeviceObjectBase.h"

class CommandPool;
class GraphicPipeline;
class RenderPass;
class DescriptorSet;
class VertexBuffer;
class IndexBuffer;
class FrameBuffer;
class Buffer;

class CommandBuffer : public DeviceObjectBase<CommandBuffer>
{
public:
	typedef struct _DrawCmdData
	{
		std::shared_ptr<RenderPass>					pRenderPass;
		std::shared_ptr<FrameBuffer>				pFrameBuffer;
		std::shared_ptr<GraphicPipeline>			pPipeline;
		std::vector<std::shared_ptr<DescriptorSet>>	descriptorSets;

		std::vector<std::shared_ptr<VertexBuffer>>	vertexBuffers;
		std::shared_ptr<IndexBuffer>				pIndexBuffer;

		std::vector<VkClearValue>					clearValues;
	}DrawCmdData;

	typedef struct _BarrierData
	{
		std::shared_ptr<Buffer>						pBuffer;
		VkPipelineStageFlagBits						srcStages;
		VkPipelineStageFlagBits						dstStages;
		VkAccessFlags								srcAccess;
		VkAccessFlags								dstAccess;
		uint32_t									offset;
		uint32_t									size;
	}BarrierData;

	typedef struct _CopyData
	{
		std::shared_ptr<Buffer>						pSrcBuffer;
		VkBufferCopy								copyData;
		std::shared_ptr<Buffer>						pDstBuffer;
	}CopyData;

	typedef struct _BufferCopyCmdData
	{
		std::vector<BarrierData>					preBarriers;
		std::vector<CopyData>						copyData;
		std::vector<BarrierData>					postBarriers;
	}BufferCopyCmdData;

public:
	~CommandBuffer();

	bool Init(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandBuffer>& pSelf, const VkCommandBufferAllocateInfo& info);

public:
	VkCommandBuffer GetDeviceHandle() const { return m_commandBuffer; }
	VkCommandBufferAllocateInfo GetAllocateInfo() const { return m_info; }

	void PrepareNormalDrawCommands(const DrawCmdData& data);
	void PrepareBufferCopyCommands(const BufferCopyCmdData& data);

protected:
	static std::shared_ptr<CommandBuffer> Create(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<CommandPool>& pCmdPool, VkCommandBufferLevel cmdBufferLevel);

protected:
	bool IsValide() const { m_isValide; }
	void SetIsValide(bool flag) { m_isValide = flag; }

protected:
	VkCommandBuffer					m_commandBuffer;
	VkCommandBufferAllocateInfo		m_info;
	bool							m_isValide;

	std::shared_ptr<CommandPool>	m_pCommandPool;

	friend class CommandPool;
};