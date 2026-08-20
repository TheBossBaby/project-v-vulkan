#include <projectV/engine/logging/log.hpp>

#include <vulkan/builder/vulkanGraphicsPipelineBuilder.hpp>

#include <stdexcept>

namespace projectv
{
    namespace vulkan::builder
    {
        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::addShaderStage(
            VkShaderModule module, VkShaderStageFlagBits stage, const char* entryPoint)
        {
            VkPipelineShaderStageCreateInfo stageInfo{};
            stageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage  = stage;
            stageInfo.module = module;
            stageInfo.pName  = entryPoint;

            m_shaderStages.push_back(stageInfo);
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setVertexInputState(
            std::span<const VkVertexInputBindingDescription> bindings,
            std::span<const VkVertexInputAttributeDescription> attributes)
        {
            m_vertexBindings.assign(bindings.begin(), bindings.end());
            m_vertexAttributes.assign(attributes.begin(), attributes.end());
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setInputAssemblyState(
            VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
        {
            m_topology               = topology;
            m_primitiveRestartEnable = primitiveRestartEnable;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setViewportState(
            uint32_t viewportCount, uint32_t scissorCount)
        {
            m_viewportCount = viewportCount;
            m_scissorCount  = scissorCount;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setRasterizationState(
            VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth)
        {
            m_polygonMode = polygonMode;
            m_cullMode    = cullMode;
            m_frontFace   = frontFace;
            m_lineWidth   = lineWidth;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setMultisampleState(VkSampleCountFlagBits sampleCount)
        {
            m_sampleCount = sampleCount;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::addColorBlendAttachment(
            const VkPipelineColorBlendAttachmentState& attachment)
        {
            m_colorBlendAttachments.push_back(attachment);
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::addColorBlendAttachment(
            VkBool32 blendEnable, VkColorComponentFlags colorWriteMask)
        {
            VkPipelineColorBlendAttachmentState attachment{};
            attachment.blendEnable         = blendEnable;
            attachment.colorWriteMask      = colorWriteMask;
            attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            attachment.colorBlendOp        = VK_BLEND_OP_ADD;
            attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

            return addColorBlendAttachment(attachment);
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setLayout(VkPipelineLayout layout)
        {
            m_pipelineLayout = layout;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setRenderPass(VkRenderPass renderPass, uint32_t subpass)
        {
            m_renderPass = renderPass;
            m_subpass    = subpass;
            return *this;
        }

        VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setFlags(VkPipelineCreateFlags flags)
        {
            m_flags = flags;
            return *this;
        }

        void VulkanGraphicsPipelineBuilder::validate(VkDevice device) const
        {
            if (device == VK_NULL_HANDLE)
            {
                engine::LogError("VulkanGraphicsPipelineBuilder::build, invalid VkDevice");
                throw std::runtime_error("VulkanGraphicsPipelineBuilder::build, invalid VkDevice");
            }

            if (m_shaderStages.empty())
            {
                engine::LogError("VulkanGraphicsPipelineBuilder::build, no shader stages set");
                throw std::runtime_error("VulkanGraphicsPipelineBuilder::build, no shader stages set");
            }

            for (const auto& stage : m_shaderStages)
            {
                if (stage.module == VK_NULL_HANDLE)
                {
                    engine::LogError("VulkanGraphicsPipelineBuilder::build, invalid VkShaderModule");
                    throw std::runtime_error("VulkanGraphicsPipelineBuilder::build, invalid VkShaderModule");
                }
            }

            if (m_pipelineLayout == VK_NULL_HANDLE)
            {
                engine::LogError("VulkanGraphicsPipelineBuilder::build, no pipeline layout set");
                throw std::runtime_error("VulkanGraphicsPipelineBuilder::build, no pipeline layout set");
            }

            if (m_renderPass == VK_NULL_HANDLE)
            {
                engine::LogError("VulkanGraphicsPipelineBuilder::build, no render pass set");
                throw std::runtime_error("VulkanGraphicsPipelineBuilder::build, no render pass set");
            }
        }

        std::vector<VkPipelineColorBlendAttachmentState> VulkanGraphicsPipelineBuilder::resolveColorBlendAttachments() const
        {
            if (!m_colorBlendAttachments.empty())
                return m_colorBlendAttachments;

            // A pipeline with one color attachment and no explicit blend state
            // configured still needs one (disabled) attachment entry.
            VkPipelineColorBlendAttachmentState defaultAttachment{};
            defaultAttachment.blendEnable    = VK_FALSE;
            defaultAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            return { defaultAttachment };
        }

        VkPipelineVertexInputStateCreateInfo VulkanGraphicsPipelineBuilder::buildVertexInputStateInfo() const
        {
            VkPipelineVertexInputStateCreateInfo vertexInputState{};
            vertexInputState.sType                          = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputState.vertexBindingDescriptionCount   = static_cast<uint32_t>(m_vertexBindings.size());
            vertexInputState.pVertexBindingDescriptions      = m_vertexBindings.empty() ? nullptr : m_vertexBindings.data();
            vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexAttributes.size());
            vertexInputState.pVertexAttributeDescriptions    = m_vertexAttributes.empty() ? nullptr : m_vertexAttributes.data();
            return vertexInputState;
        }

        VkPipelineInputAssemblyStateCreateInfo VulkanGraphicsPipelineBuilder::buildInputAssemblyStateInfo() const
        {
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
            inputAssemblyState.sType                 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyState.topology               = m_topology;
            inputAssemblyState.primitiveRestartEnable = m_primitiveRestartEnable;
            return inputAssemblyState;
        }

        VkPipelineViewportStateCreateInfo VulkanGraphicsPipelineBuilder::buildViewportStateInfo() const
        {
            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = m_viewportCount;
            viewportState.pViewports    = nullptr;
            viewportState.scissorCount  = m_scissorCount;
            viewportState.pScissors     = nullptr;
            return viewportState;
        }

        VkPipelineRasterizationStateCreateInfo VulkanGraphicsPipelineBuilder::buildRasterizationStateInfo() const
        {
            VkPipelineRasterizationStateCreateInfo rasterizationState{};
            rasterizationState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationState.depthClampEnable         = VK_FALSE;
            rasterizationState.rasterizerDiscardEnable  = VK_FALSE;
            rasterizationState.polygonMode              = m_polygonMode;
            rasterizationState.cullMode                 = m_cullMode;
            rasterizationState.frontFace                = m_frontFace;
            rasterizationState.depthBiasEnable          = VK_FALSE;
            rasterizationState.lineWidth                = m_lineWidth;
            return rasterizationState;
        }

        VkPipelineMultisampleStateCreateInfo VulkanGraphicsPipelineBuilder::buildMultisampleStateInfo() const
        {
            VkPipelineMultisampleStateCreateInfo multisampleState{};
            multisampleState.sType               = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampleState.rasterizationSamples = m_sampleCount;
            multisampleState.sampleShadingEnable  = VK_FALSE;
            return multisampleState;
        }

        VkPipelineColorBlendStateCreateInfo VulkanGraphicsPipelineBuilder::buildColorBlendStateInfo(
            const std::vector<VkPipelineColorBlendAttachmentState>& attachments) const
        {
            VkPipelineColorBlendStateCreateInfo colorBlendState{};
            colorBlendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlendState.logicOpEnable   = VK_FALSE;
            colorBlendState.logicOp         = VK_LOGIC_OP_COPY;
            colorBlendState.attachmentCount = static_cast<uint32_t>(attachments.size());
            colorBlendState.pAttachments    = attachments.data();
            return colorBlendState;
        }

        VkPipelineDynamicStateCreateInfo VulkanGraphicsPipelineBuilder::buildDynamicStateInfo() const
        {
            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
            dynamicState.pDynamicStates    = m_dynamicStates.empty() ? nullptr : m_dynamicStates.data();
            return dynamicState;
        }

        rendererResources::VulkanGraphicsPipeline VulkanGraphicsPipelineBuilder::build(VkDevice device)
        {
            validate(device);

            const std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = resolveColorBlendAttachments();

            VkPipelineVertexInputStateCreateInfo   vertexInputState   = buildVertexInputStateInfo();
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = buildInputAssemblyStateInfo();
            VkPipelineViewportStateCreateInfo      viewportState      = buildViewportStateInfo();
            VkPipelineRasterizationStateCreateInfo rasterizationState = buildRasterizationStateInfo();
            VkPipelineMultisampleStateCreateInfo   multisampleState   = buildMultisampleStateInfo();
            VkPipelineColorBlendStateCreateInfo    colorBlendState    = buildColorBlendStateInfo(colorBlendAttachments);
            VkPipelineDynamicStateCreateInfo       dynamicState       = buildDynamicStateInfo();

            VkGraphicsPipelineCreateInfo createInfo{};
            createInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            createInfo.flags               = m_flags;
            createInfo.stageCount          = static_cast<uint32_t>(m_shaderStages.size());
            createInfo.pStages             = m_shaderStages.data();
            createInfo.pVertexInputState   = &vertexInputState;
            createInfo.pInputAssemblyState = &inputAssemblyState;
            createInfo.pViewportState      = &viewportState;
            createInfo.pRasterizationState = &rasterizationState;
            createInfo.pMultisampleState   = &multisampleState;
            createInfo.pDepthStencilState  = nullptr;
            createInfo.pColorBlendState    = &colorBlendState;
            createInfo.pDynamicState       = &dynamicState;
            createInfo.layout              = m_pipelineLayout;
            createInfo.renderPass          = m_renderPass;
            createInfo.subpass             = m_subpass;
            createInfo.basePipelineHandle  = VK_NULL_HANDLE;
            createInfo.basePipelineIndex   = -1;

            return rendererResources::VulkanGraphicsPipeline(device, createInfo);
        }
    }
}
