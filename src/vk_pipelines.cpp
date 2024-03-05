#include <vk_pipelines.h>
#include <fstream>
#include <vk_initializers.h>

// set everything into default/empty properties
void PipelineBuilder::clear()
{
    // clear all of the structs we need back to 0/default with their correct sType
    // This is using cpp20 initializers, so the parameters we dont write from within
    // the brackets will be defaulted to 0. Most of the Info structures in vulkan
    // are designed so that 0 is valid clear/default option

    _inputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

    _rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

    _colorBlendAttachment = {};

    _multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

    _pipelineLayout = {};

    _depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

    _renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

    _shaderStages.clear();
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device)
{
    // make viewport state from our store viewport and scissor.
    // at the moment, we won't support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // setup dummy color blending. We aren't using transparent objects yet,
    // so the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    // completely clear VertexInputStateCreateInfo, as we have no need for it
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    // connect the renderInfo to the pNext extension mechanism
    pipelineInfo.pNext = &_renderInfo;

    pipelineInfo.stageCount = (uint32_t)_shaderStages.size();
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &_rasterizer;
    pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.layout = _pipelineLayout;

    // setting up dynamic state
    VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;
    
    pipelineInfo.pDynamicState = &dynamicInfo;
    
    // it's easy to error out on a create graphics pipeline, so we handle it a bit
    // better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline)
        != VK_SUCCESS)
    {
        fmt::print("Failed to create pipeline.");
        return VK_NULL_HANDLE; // failed to create graphics pipelines
    }
    else {
        return newPipeline;
    }
}

void PipelineBuilder::set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
    _shaderStages.clear();
    // vertex
    _shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
    // fragment
    _shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void PipelineBuilder::set_input_topology(VkPrimitiveTopology topology)
{
    _inputAssembly.topology = topology;
    // we are not going to use primitive restart for now, so we leave it on false
    _inputAssembly.primitiveRestartEnable = false;
}

void PipelineBuilder::set_polygon_mode(VkPolygonMode mode)
{
    _rasterizer.polygonMode = mode;
    _rasterizer.lineWidth = 1.0f;
}

void PipelineBuilder::set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace)
{
    _rasterizer.cullMode = cullMode;
    _rasterizer.frontFace = frontFace;
}

void PipelineBuilder::set_multisampling_none()
{
    _multisampling.sampleShadingEnable = VK_FALSE;
    // multisample defaulted to no multisampling (1 sample per pixel)
    _multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampling.minSampleShading = 1.0f;
    _multisampling.pSampleMask = nullptr;
    // no alpha to coverage either
    _multisampling.alphaToCoverageEnable = VK_FALSE;
    _multisampling.alphaToOneEnable = VK_FALSE;
}

void PipelineBuilder::disable_blending()
{
    // default write mask
    _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // no blending
    _colorBlendAttachment.blendEnable = VK_FALSE;
}

void PipelineBuilder::set_color_attachment_format(VkFormat format)
{
    _colorAttachmentFormat = format;
    // connect the format to the renderInfo structure
    _renderInfo.colorAttachmentCount = 1;
    // in things like deferred rendering where you draw to multiple images at once
    // we could have a higher count, but for now we can default it to 1.
    _renderInfo.pColorAttachmentFormats = &_colorAttachmentFormat;
}

void PipelineBuilder::set_depth_format(VkFormat format)
{
    _renderInfo.depthAttachmentFormat = format;
}

void PipelineBuilder::disable_depthtest()
{
    _depthStencil.depthTestEnable = VK_FALSE;
    _depthStencil.depthWriteEnable = VK_FALSE;
    _depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.0f;
    _depthStencil.maxDepthBounds = 1.0f;
}


bool vkutil::load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule)
{
    // open the file. With cursor at the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // find what the size of the file is by looking up the location of the cursor
    // because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = (size_t)file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read((char*)buffer.data(), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    // create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes, so multply the ints in the buffer by size of
    // int to know the real size of the buffer
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    // check that the creation goes well.
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }
    *outShaderModule = shaderModule;
    return true;
}