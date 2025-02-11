#pragma once

#include "DeviceObjectBase.h"
#include <gli\gli.hpp>

class SwapChain;
class MemoryKey;
class CommandBuffer;
class StagingBuffer;
class ImageView;
class Sampler;

typedef struct _GliImageWrapper
{
	std::vector<gli::texture>	textures;
}GliImageWrapper;

class Image : public DeviceObjectBase<Image>
{
public:
	~Image();

public:
	VkImage GetDeviceHandle() const { return m_image; }
	const VkImageCreateInfo& GetImageInfo() const { return m_info; }
	virtual uint32_t GetMemoryProperty() const { return m_memProperty; }
	virtual VkMemoryRequirements GetMemoryReqirments() const;
	virtual void EnsureImageLayout();

	VkPipelineStageFlags GetAccessStages() const { return m_accessStages; }
	VkAccessFlags GetAccessFlags() const { return m_accessFlags; }
	uint32_t GetBytesPerPixel() const { return m_bytesPerPixel; }

	void UpdateByteStream(const GliImageWrapper& gliTex);
	void UpdateByteStream(const GliImageWrapper& gliTex, uint32_t layer);

	virtual std::shared_ptr<ImageView> CreateDefaultImageView() const;
	virtual std::shared_ptr<Sampler> CreateLinearRepeatSampler() const;
	virtual std::shared_ptr<Sampler> CreateNearestRepeatSampler() const;
	virtual std::shared_ptr<Sampler> CreateLinearClampToEdgeSampler() const;

	virtual void InsertTexture(const gli::texture2d& texture, uint32_t layer) {}

protected:
	virtual void BindMemory(VkDeviceMemory memory, uint32_t offset) const;

	bool Init(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pSelf, VkImage img);
	bool Init(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pSelf, const VkImageCreateInfo& info, uint32_t memoryPropertyFlag);
	bool Init(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Image>& pSelf, const GliImageWrapper& gliTex, const VkImageCreateInfo& info, uint32_t memoryPropertyFlag);

	virtual std::shared_ptr<StagingBuffer> PrepareStagingBuffer(const GliImageWrapper& gliTex, const std::shared_ptr<CommandBuffer>& pCmdBuffer) = 0;
	virtual void ExecuteCopy(const GliImageWrapper& gliTex, const std::shared_ptr<StagingBuffer>& pStagingBuffer, const std::shared_ptr<CommandBuffer>& pCmdBuffer) = 0;
	virtual void ExecuteCopy(const GliImageWrapper& gliTex, uint32_t layer, const std::shared_ptr<StagingBuffer>& pStagingBuffer, const std::shared_ptr<CommandBuffer>& pCmdBuffer) = 0;

protected:
	VkImage						m_image;
	VkImageCreateInfo			m_info;

	bool						m_shouldDestoryRawImage = true;

	uint32_t					m_memProperty;

	std::shared_ptr<MemoryKey>	m_pMemKey;

	VkPipelineStageFlags		m_accessStages;
	VkAccessFlags				m_accessFlags;
	uint32_t					m_bytesPerPixel;

	friend class DeviceMemoryManager;
};