#include <Anvil/Log.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Engine {

namespace {

const char *CategoryToString(Log::Category cat) {
  switch (cat) {
    case Log::Category::Error:
      return "ERROR";
    case Log::Category::Info:
      return "INFO";
    case Log::Category::Debug:
      return "DEBUG";
  }
  return "";
}
} // namespace

Log::Log() {
  mAutoScroll = true;
  mScrollToBottom = false;
  clear();
}

void Log::clear() {
  mBuf.clear();
  mLineOffsets.clear();
  mLineOffsets.push_back(0);
}

void Log::addLog(Log::Category cat, const char *TAG, const char *fmt, ...) {
  int old_size = mBuf.size();
  mBuf.appendf("[%s] [%s] ", CategoryToString(cat), TAG);
  va_list args;
  va_start(args, fmt);
  mBuf.appendfv(fmt, args);
  va_end(args);
  mBuf.append("\n");
  for (int new_size = mBuf.size(); old_size < new_size; old_size++)
    if (mBuf[old_size] == '\n')
      mLineOffsets.push_back(old_size + 1);
  if (mAutoScroll)
    mScrollToBottom = true;
}

void Log::draw(bool *p_open) {
  if (!ImGui::Begin("Engine Log", p_open)) {
    ImGui::End();
    return;
  }

  // Options menu
  if (ImGui::BeginPopup("Options")) {
    if (ImGui::Checkbox("Auto-scroll", &mAutoScroll))
      if (mAutoScroll)
        mScrollToBottom = true;
    ImGui::EndPopup();
  }

  // Main window
  if (ImGui::Button("Options"))
    ImGui::OpenPopup("Options");
  ImGui::SameLine();
  bool shouldClear = ImGui::Button("Clear");
  ImGui::SameLine();
  bool copy = ImGui::Button("Copy");
  ImGui::SameLine();
  mFilter.Draw("Filter", -100.0f);

  ImGui::Separator();
  ImGui::BeginChild("scrolling", ImVec2(0, 0), false,
                    ImGuiWindowFlags_HorizontalScrollbar);

  if (shouldClear)
    clear();
  if (copy)
    ImGui::LogToClipboard();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  const char *buf = mBuf.begin();
  const char *buf_end = mBuf.end();
  if (mFilter.IsActive()) {
    // In this example we don't use the clipper when mFilter is enabled.
    // This is because we don't have a random access on the result on our
    // filter. A real application processing logs with ten of thousands of
    // entries may want to store the result of search/filter. especially if
    // the filtering function is not trivial (e.g. reg-exp).
    for (int line_no = 0; line_no < mLineOffsets.size(); line_no++) {
      const char *line_start = buf + mLineOffsets[line_no];
      const char *line_end = (line_no + 1 < mLineOffsets.size())
                                 ? (buf + mLineOffsets[line_no + 1] - 1)
                                 : buf_end;
      if (mFilter.PassFilter(line_start, line_end))
        ImGui::TextUnformatted(line_start, line_end);
    }
  } else {
    // The simplest and easy way to display the entire buffer:
    //   ImGui::TextUnformatted(buf_begin, buf_end);
    // And it'll just work. TextUnformatted() has specialization for large
    // blob of text and will fast-forward to skip non-visible lines. Here we
    // instead demonstrate using the clipper to only process lines that are
    // within the visible area. If you have tens of thousands of items and
    // their processing cost is non-negligible, coarse clipping them on your
    // side is recommended. Using ImGuiListClipper requires A) random access
    // into your data, and B) items all being the  same height, both of which
    // we can handle since we an array pointing to the beginning of each line
    // of text. When using the filter (in the block of code above) we don't
    // have random access into the data to display anymore, which is why we
    // don't use the clipper. Storing or skimming through the search result
    // would make it possible (and would be recommended if you want to search
    // through tens of thousands of entries)
    ImGuiListClipper clipper;
    clipper.Begin(mLineOffsets.size());
    while (clipper.Step()) {
      for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd;
           line_no++) {
        const char *line_start = buf + mLineOffsets[line_no];
        const char *line_end = (line_no + 1 < mLineOffsets.size())
                                   ? (buf + mLineOffsets[line_no + 1] - 1)
                                   : buf_end;
        ImGui::TextUnformatted(line_start, line_end);
      }
    }
    clipper.End();
  }
  ImGui::PopStyleVar();

  if (mScrollToBottom)
    ImGui::SetScrollHereY(1.0f);
  mScrollToBottom = false;
  ImGui::EndChild();
  ImGui::End();
}
} // namespace Engine
