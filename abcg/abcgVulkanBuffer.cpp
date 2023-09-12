/**
 * @file abcgVulkanBuffer.cpp
 * @brief Definition of abcg::VulkanBuffer
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanBuffer.hpp"

#include <gsl/gsl>

#include <set>

#include "abcgException.hpp"

void abcg::VulkanBuffer::create(VulkanDevice const &device,
                                VulkanBufferCreateInfo const &createInfo) {
  m_device = static_cast<vk::Device>(device);

  if (createInfo.properties & vk::MemoryPropertyFlagBits::eHostVisible) {
    std::tie(m_buffer, m_deviceMemory) = createBuffer(
        device, createInfo.size, createInfo.usage, createInfo.properties);

    if (createInfo.data.has_value()) {
      loadData(createInfo.data.value(), createInfo.size);
    }
  } else if (createInfo.data.has_value()) {
    // Use a staging buffer for mapping, and a device local buffer as the final
    // destination
    auto [stagingBuffer, stagingBufferMemory]{createBuffer(
        device, createInfo.size, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent)};

    // Copy data to mapped staging buffer
    // Transfer of data to the GPU will happen in the background before the next
    // call to vkQueueSubmit
    void *data{m_device.mapMemory(stagingBufferMemory, vk::DeviceSize{0},
                                  createInfo.size)};
    memcpy(data, createInfo.data->get(), createInfo.size);
    m_device.unmapMemory(stagingBufferMemory);

    // Create buffer in device local memory
    std::tie(m_buffer, m_deviceMemory) =
        createBuffer(device, createInfo.size,
                     createInfo.usage | vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);

    // Copy from staging buffer to device local buffer
    device.withCommandBuffer(
        [this, &stagingBuffer, &createInfo](const auto &commandBuffer) {
          commandBuffer.copyBuffer(stagingBuffer, m_buffer,
                                   {{.size = createInfo.size}});
        },
        vk::QueueFlagBits::eTransfer);

    // Release staging buffer
    m_device.destroyBuffer(stagingBuffer);
    m_device.freeMemory(stagingBufferMemory);
  }
}

void abcg::VulkanBuffer::destroy() {
  m_device.destroyBuffer(m_buffer);
  m_device.freeMemory(m_deviceMemory);
}

/**
 * @brief Loads data to the buffer.
 *
 * @param data Pointer to the beginning of the data.
 * @param size Size of the data fo the copied, in bytes.
 * @param offset Offset from the beginning of the buffer memory.
 */
void abcg::VulkanBuffer::loadData(gsl::not_null<void const *> data,
                                  vk::DeviceSize size, vk::DeviceSize offset) {
  // Transfer of data to the GPU will happen in the background before the next
  // call to vkQueueSubmit
  auto *mappedData{m_device.mapMemory(m_deviceMemory, offset, size)};
  memcpy(mappedData, data, size);
  m_device.unmapMemory(m_deviceMemory);
}

std::pair<vk::Buffer, vk::DeviceMemory> abcg::VulkanBuffer::createBuffer(
    VulkanDevice const &device, vk::DeviceSize size, vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties) const {
  auto const &physicalDevice{device.getPhysicalDevice()};
  auto const &queuesFamilies{physicalDevice.getQueuesFamilies()};

  std::set indices{queuesFamilies.graphics.value_or(VK_QUEUE_FAMILY_IGNORED),
                   queuesFamilies.compute.value_or(VK_QUEUE_FAMILY_IGNORED),
                   queuesFamilies.transfer.value_or(VK_QUEUE_FAMILY_IGNORED)};
  indices.erase(VK_QUEUE_FAMILY_IGNORED);

  std::vector const queueFamilyIndices(indices.begin(), indices.end());

  auto const separateTransferQueue{
      queuesFamilies.graphics.value_or(VK_QUEUE_FAMILY_IGNORED) !=
      queuesFamilies.transfer.value_or(VK_QUEUE_FAMILY_IGNORED)};

  // Create buffer object
  auto buffer{m_device.createBuffer(
      {.size = size,
       .usage = usage,
       .sharingMode = separateTransferQueue ? vk::SharingMode::eConcurrent
                                            : vk::SharingMode::eExclusive,
       .queueFamilyIndexCount =
           gsl::narrow<uint32_t>(queueFamilyIndices.size()),
       .pQueueFamilyIndices = queueFamilyIndices.data()})};

  // Get memory requirements
  auto const memoryRequirements{m_device.getBufferMemoryRequirements(buffer)};

  // Allocate buffer memory
  auto const memoryType{physicalDevice.findMemoryType(
      memoryRequirements.memoryTypeBits, properties)};
  if (!memoryType.has_value()) {
    throw abcg::RuntimeError("Failed to find suitable memory type");
  }
  auto bufferMemory{
      m_device.allocateMemory({.allocationSize = memoryRequirements.size,
                               .memoryTypeIndex = memoryType.value()})};

  // Associate buffer memory to buffer
  m_device.bindBufferMemory(buffer, bufferMemory, 0);

  return {buffer, bufferMemory};
}

/**
 * @brief Conversion to vk::Buffer.
 */
abcg::VulkanBuffer::operator vk::Buffer const &() const noexcept {
  return m_buffer;
}

/**
 * @brief Returns the opaque handle to the device memory object associated
 * with the buffer.
 *
 * @return Device memory object.
 */
vk::DeviceMemory const &abcg::VulkanBuffer::getDeviceMemory() const noexcept {
  return m_deviceMemory;
}