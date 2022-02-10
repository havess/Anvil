#include <Engine/UIManager.h>
#include <Engine/Log.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

namespace Engine {
UIManager::UIManager() {}

void UIManager::init(GLFWwindow *window, const std::string &glsl_version) {
  //ImGui::CreateContext();

  /*//1: create descriptor pool for IMGUI
	// the size of the pool is very oversized, but it's copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));

	// ----------- Init IMGUI ---------------

	//this initializes the core structures of imgui
	ImGui::CreateContext();

	//this initializes imgui for SDL
  ImGui_ImplGlfw_InitForVulkan(window);

	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, _renderPass);

	//Upload imgui font textures
	immediate_submit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	//add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {

		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		});

  ImGui::StyleColorsDark();
  ImGuiIO &io = ImGui::GetIO();
  io.FontGlobalScale = 2.1f;
  (void)io;*/
}

void UIManager::shutdown() {
  //ImGui_ImplVulkan_Shutdown();
  //ImGui_ImplGlfw_Shutdown();
  //ImGui::DestroyContext();
}

void UIManager::drawFrame() {
  /*ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  static bool show_log = false;
  static bool show_demo = false;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Demo Window", "", &show_demo)) {}
      if (ImGui::MenuItem("Log", "", &show_log)) {}
      for (auto &p : mShowWidget) {
        if (ImGui::MenuItem(p.first.c_str(), "", &p.second)) {
        }
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  for (int i = 0; i < mWidgets.size(); i++) {
    if (mShowWidget[i].second) {
      mWidgets[i](&mShowWidget[i].second);
    }
  }
  if (show_demo)
    ImGui::ShowDemoWindow(&show_demo);
  if (show_log)
    Log::getInstance().draw();

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), );*/
}

void UIManager::registerWidget(std::string name,
                               std::function<void(bool *)> renderFunc) {
  mWidgets.push_back(renderFunc);
  mShowWidget.push_back(std::make_pair(name, true));
}

} // namespace Engine
