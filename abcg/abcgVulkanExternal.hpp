/**
 * @file abcgVulkanExternal.hpp
 * @brief Header file for including Vulkan-related dependencies.
 *
 * Preprocessor definitions and includes of Vulkan-related external
 * dependencies.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_VULKAN_EXTERNAL_HPP_
#define ABCG_VULKAN_EXTERNAL_HPP_

#include <volk.h>

// Required for Vulkan-Hpp to work with Volk with a static dispatch
#undef VK_NO_PROTOTYPES

// Enable designated initializers in Vulkan-Hpp
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>

#if !defined(NDEBUG)
// When defined, enable validation layer and debug reporting
#define ABCG_VULKAN_DEBUG_REPORT
#endif

#endif
