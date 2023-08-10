#include "device_view.h"
#include "../util.h"

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

    for(auto& [typeId, typedList] : slave.objectTable())
    {
        ImGui::Text("%s (%d)", typedList.typeName.c_str(),  typedList.typeId); ImGui::SameLine();
        if(ImGui::InputInt(("watch Period(ms)##"+std::to_string(typeId)).c_str(), &(typedList.watchPeriodMs), 10))
        {
            slave.addTypedReadTarget(typeId);
            std::cout<<"period changed to "<<typedList.watchPeriodMs<<std::endl;
        }
        if(typedList.watchPeriodMs < 0) typedList.watchPeriodMs = 0;

        if (ImGui::BeginTable("device_view_table", 7, flags))
        {
            ImGui::TableSetupColumn("TypeID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("ObjectID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Access", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Raw", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto &object: typedList.objects)
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
                ImGui::Text("%zu", typedList.typeSize);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", LLINK_ACCESS_STRING[object.access]);
                ImGui::TableSetColumnIndex(6);
                switch(object.access)
                {
                    case LLINK_ACCESS_READWRITE:
                        if(util::InputTextStdString(
                                ("test##text" +std::to_string(typeId)+"/"+std::to_string(object.id)).c_str(),
                                &object.rawText,
                                ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue,
                                NULL,
                                NULL))
                        {
                            std::cout<<"new text: "<<object.rawText<<std::endl;
                            auto byteArray = util::strToByteArray(object.rawText, typedList.typeSize);
                            for(const auto& byte : byteArray)std::cout<< std::hex << (int)byte <<" ";
                            std::cout<<std::endl;
                            slave.writeObject(typeId, {object.id}, {byteArray});
                        }
                        break;
                    case LLINK_ACCESS_WRITE:
                        ImGui::Text("write only");
                        break;
                    case LLINK_ACCESS_READ:
                        ImGui::Text("%s", object.dataValid ? object.rawText.c_str() : "N/A");
                        break;
                }
            }

            ImGui::EndTable();
        }
    }
}