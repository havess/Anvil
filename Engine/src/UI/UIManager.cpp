#include <Engine/UIManager.h>
#include <Engine/Log.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
namespace Engine {

UIManager::UIManager() {}

void UIManager::init(GLFWwindow *window, const std::string &glsl_version) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
  ImGuiIO &io = ImGui::GetIO();
  io.FontGlobalScale = 2.1f;
  (void)io;
}

void UIManager::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UIManager::drawFrame() {
  ImGui_ImplOpenGL3_NewFrame();
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
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::registerWidget(std::string name,
                               std::function<void(bool *)> renderFunc) {
  mWidgets.push_back(renderFunc);
  mShowWidget.push_back(std::make_pair(name, true));
}

} // namespace Engine
