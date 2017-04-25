#pragma once
#include "../common/RefCounted.h"
#include "vulkan.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include <vector>

class Device
{
public:
	~Device();

	const VkDevice GetDeviceHandle() const { return m_device; }
	const std::shared_ptr<PhysicalDevice> GetPhysicalDevice() const { return m_pPhysicalDevice; }
	const std::shared_ptr<Instance> GetInstance() const { return m_pVulkanInst; }

	bool Init(const std::shared_ptr<Instance> pInst, const std::shared_ptr<PhysicalDevice> pPhyisicalDevice);

protected:
	VkDevice							m_device;
	std::shared_ptr<PhysicalDevice>		m_pPhysicalDevice;
	std::shared_ptr<Instance>			m_pVulkanInst;
};