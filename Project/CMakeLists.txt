# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)


set(SOURCES
    "main.cpp"
	"stb_image.h"
    "vulkanbase/VulkanBase.h" 
	"vulkanbase/VulkanBase.cpp"
    "vulkanbase/VulkanUtil.h"
    "vulkanbase/VulkanUtil.cpp"
	
    # Add other source files here
    #"labwork/Week01.cpp"
    #"labwork/Week02.cpp" 
    #"labwork/Week03.cpp" 
    #"labwork/Week04.cpp" 
    #"labwork/Week05.cpp"
    #"labwork/Week06.cpp"
    "GP2Shader.h" 
    "GP2Shader.cpp" 
    "GP2CommandPool.h" 
    "GP2CommandPool.cpp" 
	"GP2CommandBuffer.h" 
	"GP2CommandBuffer.cpp" 
	"GP2VertexBuffer.h" 
	"GP2VertexBuffer.cpp" 
	"GP2Buffer.h" 
	"GP2Buffer.cpp" 
	"GP2IndexBuffer.h" 
	"GP2IndexBuffer.cpp" 
	"GP2Mesh.h" 
	"GP2Mesh.cpp" 
	"GP2GraphicsPipeline2D.h" 
	"GP2GraphicsPipeline2D.cpp" 
    ##Restructuring:
	"vxl_Window.h"
    "vxl_Window.cpp"
    "vxl_App.h"
    "vxl_App.cpp"
    "vxl_GraphicsPipeline.h"
    "vxl_GraphicsPipeline.cpp"
    "vxl_Device.h"
    "vxl_Device.cpp"
    "vxl_SwapChain.h"
    "vxl_SwapChain.cpp"
    "vxl_Model.h"
    "vxl_Model.cpp"
    "vxl_Utils.h"
    "vxl_Utils.cpp"

    "vxl_GraphicsPipeline3D.h"
    "vxl_GraphicsPipeline3D.cpp"
    "vxl_SwapChain3D.h"
    "vxl_SwapChain3D.cpp"
    "vxl_Model3D.h"
    "vxl_Model3D.cpp"
 "vxl_UBO.h" "vxl_UBO.cpp" "vxl_Camera.h" "vxl_Camera.cpp" "vxl_Block.h" "vxl_Block.cpp" "vxl_Chunk.h" "vxl_Chunk.cpp" "vxl_World.h" "vxl_World.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Shaders)

# Custom command to copy Resources folder
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)


# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
