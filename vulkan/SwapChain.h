#pragma once
#include "../common/RefCounted.h"
#include "vulkan.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include <vector>
#include <memory>
#include "DeviceObjectBase.h"
#include "SwapChainImage.h"

class SwapChain : public DeviceObjectBase
{
public:
	~SwapChain();

	bool Init(const std::shared_ptr<Device>& pDevice) override;

	const VkSwapchainKHR GetDeviceHandle() const { return m_swapchain; }
	const std::shared_ptr<SwapChainImage> GetSwapChainImage(uint32_t index) { assert(index < m_swapchainImages.size()); return m_swapchainImages[index]; }
	uint32_t GetSwapChainImageCount() const { return m_swapchainImages.size(); }

	void EnsureSwapChainImageLayout();

	PFN_vkAcquireNextImageKHR GetAcquireNextImageFuncPtr() const { return m_fpAcquireNextImageKHR; }
	PFN_vkQueuePresentKHR GetQueuePresentFuncPtr() const { return m_fpQueuePresentKHR; }
	PFN_vkGetSwapchainImagesKHR GetGetSwapchainImagesFuncPtr() const { return m_fpGetSwapchainImagesKHR; }

public:
	static std::shared_ptr<SwapChain> Create(const std::shared_ptr<Device>& pDevice);

protected:
	VkSwapchainKHR						m_swapchain;

	PFN_vkCreateSwapchainKHR			m_fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR			m_fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR			m_fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR			m_fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR				m_fpQueuePresentKHR;

	std::vector<std::shared_ptr<SwapChainImage>> m_swapchainImages;
};