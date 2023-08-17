#include "device_navigator.h"
#include "widgets.h"
#include <IconsMaterialDesign.h>

void DeviceNavigator::DeviceNavigator(Model& model)
{
    bool open = true;
    ImGui::Begin("Device Navigator", &open, ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar())
    {
        if(ImGui::MenuItem(ICON_MD_ADD "Add Master"))
        {
            model.updatePortNames();
            ImGui::OpenPopup("Add Master");
        }

        if(ImGui::BeginPopupModal("Add Master", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            AddMasterModal(model);ImGui::EndPopup();}

        ImGui::EndMenuBar();
    }

    std::vector<std::string> masterRemoveList;
    for(auto& [portName, master] : model.getMasters())
    {
        ImGui::PushID(portName.c_str());
        if(MasterView(model, portName))
            masterRemoveList.push_back(portName);
        ImGui::PopID();
    }
    
    for(const auto& masterRemovePortName : masterRemoveList)
        model.removeMaster(masterRemovePortName);

    ImGui::End();
}

bool DeviceNavigator::MasterView(Model &model, const std::string &portName)
{
    auto master = model.getMaster(portName).lock();
    bool remove = false;
    ImGuiStorage* storage = ImGui::GetStateStorage();
    bool* slavesShown = storage->GetBoolRef(ImGui::GetItemID(), true);
    // background
    widget::BackgroundContent(ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Header)));

    // toggle button
    if (widget::IconButton(*slavesShown? ICON_MD_EXPAND_MORE : ICON_MD_CHEVRON_RIGHT))
    {
        *slavesShown = !*slavesShown;
    }ImGui::SameLine();

    // LED
    widget::Led(widget::LedColor::GREEN);
    ImGui::SameLine();

    // master title
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", portName.c_str());
    ImGui::SameLine();

    // open close button
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,  ImVec2(0.0f, 0.0f));
    if(widget::IconButton(master->isOpen().has_value() ? (master->isOpen().value() ? ICON_MD_STOP : ICON_MD_PLAY_ARROW) : ICON_MD_HOURGLASS_FULL ))
    {
        if(!master->isOpen().has_value()){}
        else if(master->isOpen().value()) master->close();
        else master->open();
    } ImGui::SameLine();

    // search button
    if(!master->isOpen().has_value() || !master->isOpen().value()) ImGui::BeginDisabled();
    if(widget::IconButton(ICON_MD_SEARCH))
    {
        model.getSlaveSearchInfo().mMaster = master;
        ImGui::OpenPopup("Search");
    } ImGui::SameLine();
    if(!master->isOpen().has_value() || !master->isOpen().value())  ImGui::EndDisabled();

    // log button
    if(widget::IconButton(ICON_MD_DESCRIPTION))
    {
        master->openLog();
    }ImGui::SameLine();

    // remove button
    if(widget::IconButton(ICON_MD_DELETE))
        remove = true;
    ImGui::PopStyleVar();

    // search popup
    if(ImGui::BeginPopupModal("Search", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        SearchSlaveModal(model);ImGui::EndPopup();}

    // slaves
    if(!*slavesShown)
        return remove;
    ImGui::Indent();
    for(auto& [id, slave] : master->getSlaves())
    {
        ImGui::PushID(slave->id());
        ImGui::AlignTextToFramePadding();
        ImGui::Bullet();
        ImGui::Text("ID: %d (%d)", slave->id(), slave->baudRate());
        ImGui::SameLine();
        if(widget::IconButton(ICON_MD_VISIBILITY))
            model.setDeviceViewTarget(std::pair<std::string, int>(portName, id));
        ImGui::PopID();
    }

    ImGui::Unindent();
    return remove;
}

void DeviceNavigator::AddMasterModal(Model &model)
{
    std::vector<const char*> portNamesCStr;
    portNamesCStr.reserve(model.getPortNames().size());
    for(const auto& name : model.getPortNames())
        portNamesCStr.push_back(name.c_str());
    static int portIndex = 0;
    ImGui::Combo("Port##combo_port_names", &portIndex, &portNamesCStr[0], model.getPortNames().size());

    if(ImGui::Button("Cancel"))
    {
        ImGui::CloseCurrentPopup();
    }ImGui::SameLine();

    if(ImGui::Button("Add"))
    {
        ImGui::CloseCurrentPopup();
        model.addMaster(model.getPortNames()[portIndex]);
    }
}

void DeviceNavigator::SearchSlaveModal(Model& model)
{
    if(model.getSlaveSearchInfo().isDone.has_value() && !model.getSlaveSearchInfo().isDone.value())
    {
        ImGui::Combo("Port##combo_port_names", &model.getSlaveSearchInfo().mBaudRateIdx, &Model::BAUD_RATE_STRINGS[0],
                     Model::BAUD_RATE_STRINGS.size());

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Search"))
        {
            model.searchSlave();
        }
    }
    else if(!model.getSlaveSearchInfo().isDone.has_value())
    {
        ImGui::ProgressBar(model.getSlaveSearchInfo().currentPingingID / 255.0);
        if(ImGui::Button("Cancel"))
            model.getSlaveSearchInfo().mMaster.lock()->cancelSearch();
    }
    else
    {
        ImGui::CloseCurrentPopup();
        model.getSlaveSearchInfo().isDone = false;
    }
}

