#include <builder/VulkanRenderPassBuilder.hpp>

namespace projectv
{
    namespace vulkan::builder
    {
        uint32_t VulkanRenderPassBuilder::addAttachment(const VkAttachmentDescription& desc)
        {
            m_attachments.push_back(desc);
            return static_cast<uint32_t>(m_attachments.size() - 1);
        }

        uint32_t VulkanRenderPassBuilder::addColorAttachment(
            VkFormat format,
            VkImageLayout finalLayout,
            VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp,
            VkAttachmentStoreOp storeOp)
        {
            VkAttachmentDescription desc{};
            desc.format         = format;
            desc.samples        = samples;
            desc.loadOp         = loadOp;
            desc.storeOp        = storeOp;
            desc.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout    = finalLayout;
            return addAttachment(desc);
        }

        uint32_t VulkanRenderPassBuilder::addDepthAttachment(
            VkFormat format,
            VkImageLayout finalLayout,
            VkSampleCountFlagBits samples)
        {
            VkAttachmentDescription desc{};
            desc.format         = format;
            desc.samples        = samples;
            desc.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout    = finalLayout;
            return addAttachment(desc);
        }

        uint32_t VulkanRenderPassBuilder::beginSubpass(VkPipelineBindPoint bindPoint)
        {
            m_subpasses.emplace_back();
            m_subpasses.back().bindPoint = bindPoint;
            return static_cast<uint32_t>(m_subpasses.size() - 1);
        }

        void VulkanRenderPassBuilder::addColorRef(uint32_t subpassIndex, uint32_t attachmentIndex,
            VkImageLayout layout)
        {
            m_subpasses[subpassIndex].colorRefs.push_back({ attachmentIndex, layout });
        }

        void VulkanRenderPassBuilder::addInputRef(uint32_t subpassIndex, uint32_t attachmentIndex,
            VkImageLayout layout)
        {
            m_subpasses[subpassIndex].inputRefs.push_back({ attachmentIndex, layout });
        }

        void VulkanRenderPassBuilder::addResolveRef(uint32_t subpassIndex, uint32_t attachmentIndex,
            VkImageLayout layout)
        {
            m_subpasses[subpassIndex].resolveRefs.push_back({ attachmentIndex, layout });
        }

        void VulkanRenderPassBuilder::setDepthRef(uint32_t subpassIndex, uint32_t attachmentIndex,
            VkImageLayout layout)
        {
            m_subpasses[subpassIndex].depthRef = VkAttachmentReference{ attachmentIndex, layout };
        }

        void VulkanRenderPassBuilder::addPreserveRef(uint32_t subpassIndex, uint32_t attachmentIndex)
        {
            m_subpasses[subpassIndex].preserveRefs.push_back(attachmentIndex);
        }

        
        void VulkanRenderPassBuilder::addDependency(const VkSubpassDependency& dependency)
        {
            m_dependencies.push_back(dependency);
        }

        void VulkanRenderPassBuilder::addDependency(
            uint32_t srcSubpass, uint32_t dstSubpass,
            VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
            VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
            VkDependencyFlags dependencyFlags)
        {
            VkSubpassDependency dep{};
            dep.srcSubpass    = srcSubpass;
            dep.dstSubpass    = dstSubpass;
            dep.srcStageMask  = srcStageMask;
            dep.dstStageMask  = dstStageMask;
            dep.srcAccessMask = srcAccessMask;
            dep.dstAccessMask = dstAccessMask;
            dep.dependencyFlags = dependencyFlags;
            m_dependencies.push_back(dep);
        }
        
        vulkan::VulkanRenderPass VulkanRenderPassBuilder::build(VkDevice device)
        {
            std::vector<VkSubpassDescription> subpassDescs;
            subpassDescs.reserve(m_subpasses.size());

            for (const auto& subPass : m_subpasses)
            {
                VkSubpassDescription desc{};
                desc.pipelineBindPoint = subPass.bindPoint;
                desc.colorAttachmentCount = static_cast<uint32_t>(subPass.colorRefs.size());
                desc.pColorAttachments    = subPass.colorRefs.empty() ? nullptr : subPass.colorRefs.data();
                desc.inputAttachmentCount = static_cast<uint32_t>(subPass.inputRefs.size());
                desc.pInputAttachments    = subPass.inputRefs.empty() ? nullptr : subPass.inputRefs.data();
                desc.pResolveAttachments = subPass.resolveRefs.empty() ? nullptr : subPass.resolveRefs.data();
                desc.pDepthStencilAttachment = subPass.depthRef.has_value() ? &subPass.depthRef.value() : nullptr;
                desc.preserveAttachmentCount = static_cast<uint32_t>(subPass.preserveRefs.size());
                desc.pPreserveAttachments    = subPass.preserveRefs.empty() ? nullptr : subPass.preserveRefs.data();
                subpassDescs.push_back(desc);
            }

            VkRenderPassCreateInfo createInfo{};
            createInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            createInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
            createInfo.pAttachments    = m_attachments.data();
            createInfo.subpassCount    = static_cast<uint32_t>(subpassDescs.size());
            createInfo.pSubpasses      = subpassDescs.data();
            createInfo.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
            createInfo.pDependencies   = m_dependencies.empty() ? nullptr : m_dependencies.data();

            return VulkanRenderPass(device, createInfo);
        }
    }
}