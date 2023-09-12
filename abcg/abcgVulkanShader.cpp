/**
 * @file abcgVulkanShader.cpp
 * @brief Definition of helper functions for creating Vulkan shaders.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgVulkanShader.hpp"
#include "abcgException.hpp"

#include <glslang/SPIRV/GlslangToSpv.h>

#include <fmt/core.h>
#include <gsl/gsl>

#include <filesystem>
#include <fstream>

namespace {
TBuiltInResource InitResources() {
  TBuiltInResource Resources{
      .maxLights = 32,
      .maxClipPlanes = 6,
      .maxTextureUnits = 32,
      .maxTextureCoords = 32,
      .maxVertexAttribs = 64,
      .maxVertexUniformComponents = 4096,
      .maxVaryingFloats = 64,
      .maxVertexTextureImageUnits = 32,
      .maxCombinedTextureImageUnits = 80,
      .maxTextureImageUnits = 32,
      .maxFragmentUniformComponents = 4096,
      .maxDrawBuffers = 32,
      .maxVertexUniformVectors = 128,
      .maxVaryingVectors = 8,
      .maxFragmentUniformVectors = 16,
      .maxVertexOutputVectors = 16,
      .maxFragmentInputVectors = 15,
      .minProgramTexelOffset = -8,
      .maxProgramTexelOffset = 7,
      .maxClipDistances = 8,
      .maxComputeWorkGroupCountX = 65535,
      .maxComputeWorkGroupCountY = 65535,
      .maxComputeWorkGroupCountZ = 65535,
      .maxComputeWorkGroupSizeX = 1024,
      .maxComputeWorkGroupSizeY = 1024,
      .maxComputeWorkGroupSizeZ = 64,
      .maxComputeUniformComponents = 1024,
      .maxComputeTextureImageUnits = 16,
      .maxComputeImageUniforms = 8,
      .maxComputeAtomicCounters = 8,
      .maxComputeAtomicCounterBuffers = 1,
      .maxVaryingComponents = 60,
      .maxVertexOutputComponents = 64,
      .maxGeometryInputComponents = 64,
      .maxGeometryOutputComponents = 128,
      .maxFragmentInputComponents = 128,
      .maxImageUnits = 8,
      .maxCombinedImageUnitsAndFragmentOutputs = 8,
      .maxCombinedShaderOutputResources = 8,
      .maxImageSamples = 0,
      .maxVertexImageUniforms = 0,
      .maxTessControlImageUniforms = 0,
      .maxTessEvaluationImageUniforms = 0,
      .maxGeometryImageUniforms = 0,
      .maxFragmentImageUniforms = 8,
      .maxCombinedImageUniforms = 8,
      .maxGeometryTextureImageUnits = 16,
      .maxGeometryOutputVertices = 256,
      .maxGeometryTotalOutputComponents = 1024,
      .maxGeometryUniformComponents = 1024,
      .maxGeometryVaryingComponents = 64,
      .maxTessControlInputComponents = 128,
      .maxTessControlOutputComponents = 128,
      .maxTessControlTextureImageUnits = 16,
      .maxTessControlUniformComponents = 1024,
      .maxTessControlTotalOutputComponents = 4096,
      .maxTessEvaluationInputComponents = 128,
      .maxTessEvaluationOutputComponents = 128,
      .maxTessEvaluationTextureImageUnits = 16,
      .maxTessEvaluationUniformComponents = 1024,
      .maxTessPatchComponents = 120,
      .maxPatchVertices = 32,
      .maxTessGenLevel = 64,
      .maxViewports = 16,
      .maxVertexAtomicCounters = 0,
      .maxTessControlAtomicCounters = 0,
      .maxTessEvaluationAtomicCounters = 0,
      .maxGeometryAtomicCounters = 0,
      .maxFragmentAtomicCounters = 8,
      .maxCombinedAtomicCounters = 8,
      .maxAtomicCounterBindings = 1,
      .maxVertexAtomicCounterBuffers = 0,
      .maxTessControlAtomicCounterBuffers = 0,
      .maxTessEvaluationAtomicCounterBuffers = 0,
      .maxGeometryAtomicCounterBuffers = 0,
      .maxFragmentAtomicCounterBuffers = 1,
      .maxCombinedAtomicCounterBuffers = 1,
      .maxAtomicCounterBufferSize = 16384,
      .maxTransformFeedbackBuffers = 4,
      .maxTransformFeedbackInterleavedComponents = 64,
      .maxCullDistances = 8,
      .maxCombinedClipAndCullDistances = 8,
      .maxSamples = 4,
      .maxMeshOutputVerticesNV = 256,
      .maxMeshOutputPrimitivesNV = 512,
      .maxMeshWorkGroupSizeX_NV = 32,
      .maxMeshWorkGroupSizeY_NV = 1,
      .maxMeshWorkGroupSizeZ_NV = 1,
      .maxTaskWorkGroupSizeX_NV = 32,
      .maxTaskWorkGroupSizeY_NV = 1,
      .maxTaskWorkGroupSizeZ_NV = 1,
      .maxMeshViewCountNV = 4,
      .maxDualSourceDrawBuffersEXT = 1,
      .limits = {.nonInductiveForLoops = true,
                 .whileLoops = true,
                 .doWhileLoops = true,
                 .generalUniformIndexing = true,
                 .generalAttributeMatrixVectorIndexing = true,
                 .generalVaryingIndexing = true,
                 .generalSamplerIndexing = true,
                 .generalVariableIndexing = true,
                 .generalConstantMatrixVectorIndexing = true}};

  return Resources;
}

[[nodiscard]] vk::ShaderStageFlagBits
abcgStageToVulkanStage(abcg::ShaderStage stage) {
  switch (stage) {
  case abcg::ShaderStage::Vertex:
    return vk::ShaderStageFlagBits::eVertex;
  case abcg::ShaderStage::TessellationControl:
    return vk::ShaderStageFlagBits::eTessellationControl;
  case abcg::ShaderStage::TessellationEvaluation:
    return vk::ShaderStageFlagBits::eTessellationEvaluation;
  case abcg::ShaderStage::Geometry:
    return vk::ShaderStageFlagBits::eGeometry;
  case abcg::ShaderStage::Fragment:
    return vk::ShaderStageFlagBits::eFragment;
  case abcg::ShaderStage::Compute:
    return vk::ShaderStageFlagBits::eCompute;
  case abcg::ShaderStage::RayGen:
    return vk::ShaderStageFlagBits::eRaygenKHR;
  case abcg::ShaderStage::Intersection:
    return vk::ShaderStageFlagBits::eIntersectionKHR;
  case abcg::ShaderStage::AnyHit:
    return vk::ShaderStageFlagBits::eAnyHitKHR;
  case abcg::ShaderStage::ClosestHit:
    return vk::ShaderStageFlagBits::eClosestHitKHR;
  case abcg::ShaderStage::Miss:
    return vk::ShaderStageFlagBits::eMissKHR;
  case abcg::ShaderStage::Callable:
    return vk::ShaderStageFlagBits::eCallableKHR;
  case abcg::ShaderStage::Task:
    return vk::ShaderStageFlagBits::eTaskNV;
  case abcg::ShaderStage::Mesh:
    return vk::ShaderStageFlagBits::eMeshNV;
  default:
    throw abcg::RuntimeError("Unknown shader stage");
  }
}

[[nodiscard]] EShLanguage abcgStageToGlslangStage(abcg::ShaderStage stage) {
  switch (stage) {
  case abcg::ShaderStage::Vertex:
    return EShLangVertex;
  case abcg::ShaderStage::TessellationControl:
    return EShLangTessControl;
  case abcg::ShaderStage::TessellationEvaluation:
    return EShLangTessEvaluation;
  case abcg::ShaderStage::Geometry:
    return EShLangGeometry;
  case abcg::ShaderStage::Fragment:
    return EShLangFragment;
  case abcg::ShaderStage::Compute:
    return EShLangCompute;
  case abcg::ShaderStage::RayGen:
    return EShLangRayGen;
  case abcg::ShaderStage::Intersection:
    return EShLangIntersect;
  case abcg::ShaderStage::AnyHit:
    return EShLangAnyHit;
  case abcg::ShaderStage::ClosestHit:
    return EShLangClosestHit;
  case abcg::ShaderStage::Miss:
    return EShLangMiss;
  case abcg::ShaderStage::Callable:
    return EShLangCallable;
  case abcg::ShaderStage::Task:
    return EShLangTaskNV;
  case abcg::ShaderStage::Mesh:
    return EShLangMeshNV;
  default:
    throw abcg::RuntimeError("Unknown shader stage");
  }
}

[[nodiscard]] char const *glslangStageToText(EShLanguage stage) {
  switch (stage) {
  case EShLangVertex:
    return "vertex";
  case EShLangTessControl:
    return "tess control";
  case EShLangTessEvaluation:
    return "tess evaluation";
  case EShLangGeometry:
    return "geometry";
  case EShLangFragment:
    return "fragment";
  case EShLangCompute:
    return "compute";
  case EShLangRayGen:
    return "raygen";
  case EShLangIntersect:
    return "intersection";
  case EShLangAnyHit:
    return "any-hit";
  case EShLangClosestHit:
    return "closest hit";
  case EShLangMiss:
    return "miss";
  case EShLangCallable:
    return "callable";
  case EShLangTaskNV:
    return "task";
  case EShLangMeshNV:
    return "mesh";
  default:
    return "unknown";
  }
}

// If filenameOrText is a filename, returns the contents of the file (assumed
// to be in text format). Otherwise, returns filenameOrText.
[[nodiscard]] std::string toSource(std::string_view filenameOrText) {
  static const std::size_t maxPathSize{260};
  if (filenameOrText.size() > maxPathSize ||
      !std::filesystem::exists(filenameOrText)) {
    return filenameOrText.data();
  }
  std::stringstream source;
  if (std::ifstream stream(filenameOrText.data()); stream) {
    source << stream.rdbuf();
    stream.close();
  } else {
    throw abcg::RuntimeError(
        fmt::format("Failed to read file {}", filenameOrText));
  }
  return source.str();
}
} // namespace

// Compiles the given GLSL shader source into Vulkan SPIR-V.
std::vector<uint32_t> GLSLtoSPV(abcg::ShaderSource shaderSource) {
  // Prints out log info for compiling and linking
  auto printLog{[](glslang::TShader &shader, std::string_view name) {
    if (std::string const log{shader.getInfoLog()}; !log.empty()) {
      fmt::print("Shader information log ({} shader):\n{}\n", name, log);
    }
    if (std::string const log{shader.getInfoDebugLog()}; !log.empty()) {
      fmt::print("Shader information debug log ({} shader):\n{}\n", name, log);
    }
  }};

  auto const *data{shaderSource.source.data()};
  auto const stage{abcgStageToGlslangStage(shaderSource.stage)};
  glslang::TShader shader(stage);
  shader.setStrings(&data, 1);

  // Enable SPIR-V and Vulkan rules when parsing GLSL
  auto messages{gsl::narrow<EShMessages>(EShMsgSpvRules |
                                         EShMsgVulkanRules /* |
                         EShMsgEnhanced*/)};

  // Compiles
  TBuiltInResource const resources{InitResources()};
  if (!shader.parse(&resources, 100, false, messages)) {
    auto const *shaderStage{glslangStageToText(stage)};
    printLog(shader, shaderStage);
    throw abcg::RuntimeError(
        fmt::format("Failed to compile {} shader", shaderStage));
  }

  // Links
  glslang::TProgram program;
  program.addShader(&shader);
  if (!program.link(messages)) {
    auto const *shaderStage{glslangStageToText(stage)};
    printLog(shader, shaderStage);
    throw abcg::RuntimeError(
        fmt::format("Failed to link {} shader", shaderStage));
  }

  std::vector<uint32_t> outCode;
  glslang::GlslangToSpv(*program.getIntermediate(stage), outCode);

  return outCode;
}

/**
 * @brief Compiles a GLSL shader to SPIR-V and creates its module.
 *
 * @param device Vulkan device to be used to create the shader module.
 * @param pathOrSource Path or source code of the GLSL shader to be compiled to
 * SPIR-V.
 *
 * @throw abcg::RuntimeError if the shader could not be read from file or has
 * failed to compile.
 */
void abcg::VulkanShader::create(VulkanDevice const &device,
                                ShaderSource const &pathOrSource) {
  m_device = static_cast<vk::Device>(device);

  ShaderSource const source{.source = toSource(pathOrSource.source),
                            .stage = pathOrSource.stage};

  glslang::InitializeProcess();
  std::vector<uint32_t> shader{GLSLtoSPV(source)};
  m_stage = abcgStageToVulkanStage(source.stage);
  glslang::FinalizeProcess();

  m_module = m_device.createShaderModule(
      {.codeSize = shader.size() * sizeof(uint32_t), .pCode = shader.data()});
}

/**
 * @brief Destroys the shader module.
 */
void abcg::VulkanShader::destroy() {
  if (!m_device) {
    return;
  }

  m_device.destroyShaderModule(m_module);
}

/**
 * @brief Returns the shader stage bitmask.
 *
 * @return Shader stage bitmask.
 */
vk::ShaderStageFlagBits const &abcg::VulkanShader::getStage() const noexcept {
  return m_stage;
}

/**
 * @brief Returns the opaque handle to the shader module object.
 *
 * @return Shader module.
 */
vk::ShaderModule const &abcg::VulkanShader::getModule() const noexcept {
  return m_module;
}