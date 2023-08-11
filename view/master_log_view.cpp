#include "master_log_view.h"
#include "../model/master_log.h"

bool MasterLogView::MasterLogView(MasterLog& masterLog)
{
    ImGui::Begin("Master Log");

    static std::vector<std::string> data;

    data.emplace_back("push");
    ImGui::BeginChild("scrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& str: data)
    {
        ImGui::Button(str.c_str());
    }

    // autoscroll
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
    return false;
}
