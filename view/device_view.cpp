#include "device_view.h"

void DeviceView::DeviceView(Slave &slave)
{
    ImGui::Text("ID: %d (%d)", slave.id(),  slave.baudRate());

    const ImGuiTableFlags flags =
            ImGuiTableFlags_SizingFixedFit |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_Reorderable |
            ImGuiTableFlags_Hideable;

    const char LLINK_ACCESS_STRING[4][13] = {"inaccessible", "read", "write", "readwrite"};

    for(auto& [typeName, typedList] : slave.objectTable())
    {
        ImGui::Text("%s (%d)", typedList.typeName.c_str(),  typedList.typeId); ImGui::SameLine();
        ImGui::InputInt("Watch period(ms)", &(typedList.watchPeriodMs), 10); ImGui::SameLine();
        if(typedList.watchPeriodMs < 0) typedList.watchPeriodMs = 0;

        if (ImGui::BeginTable("device_view_table", 6, flags))
        {
            ImGui::TableSetupColumn("TypeID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("ObjectID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Access", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (const auto &object: typedList.objects)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", typedList.typeId);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", object.id);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", object.name.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", typedList.typeName.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", typedList.typeSize);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", LLINK_ACCESS_STRING[object.access]);
            }

            ImGui::EndTable();
        }
    }
}
