#include "master_log_view.h"
#include <iomanip>
#include <sstream>
#include <string>
#include "../model/master_log.h"

bool MasterLogView::MasterLogView(MasterLog& masterLog)
{
    ImGui::Begin("Master Log");
    ImGui::PushID(masterLog.portName().c_str());

    // clear button
    if(ImGui::Button("Clear"))
    {
        masterLog.clear();
    }

    // display log
    ImGui::BeginChild("Log");
    const auto& logs = masterLog.logsStr();
    for(const auto& log : logs)
    {
        ImGui::Text("%s", log.c_str());
    }

    // autoscroll
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::PopID();
    ImGui::End();
    return false;
}
