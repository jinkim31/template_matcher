#include "device_navigator.h"

void DeviceNavigator::DeviceNavigator(Model& model)
{
    ImGui::Begin("Device Navigator");
    if(ImGui::Button("Add Master"))
    {
        model.updatePortNames();
        ImGui::OpenPopup("Add Master");
    }

    std::vector<std::string> masterRemoveList;
    for(auto& [portName, master] : model.getMasters())
    {
        if(MasterView(model, portName))
            masterRemoveList.push_back(portName);
    }
    
    for(const auto& masterRemovePortName : masterRemoveList)
        model.removeMaster(masterRemovePortName);

    // MODALS //////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(ImGui::BeginPopupModal("Add Master", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        AddMasterModal(model);ImGui::EndPopup();}
    if(ImGui::BeginPopupModal("Search", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        SearchSlaveModal(model);ImGui::EndPopup();}
    ImGui::End();
}

bool DeviceNavigator::MasterView(Model &model, const std::string &portName)
{
    auto master = model.getMaster(portName).lock();
    bool remove = false;

    // master title
    ImGui::Text("Master at [%s]", portName.c_str());
    ImGui::SameLine();

    // open close button text
    std::string openBtnText;
    if(!master->isOpen().has_value()) openBtnText = "Working";
    else if(master->isOpen().value()) openBtnText = "Close";
    else openBtnText = "Open";

    // open close button
    if(ImGui::Button((openBtnText+"##"+portName).c_str()))
    {
        if(!master->isOpen().has_value()){}
        else if(master->isOpen().value()) master->close();
        else master->open();
    } ImGui::SameLine();

    // search button
    if(!master->isOpen().has_value() || !master->isOpen().value()) ImGui::BeginDisabled();
    if(ImGui::Button(("Search##"+portName).c_str()))
    {
        model.getSlaveSearchInfo().mMaster = master;
        ImGui::OpenPopup("Search");
    } ImGui::SameLine();
    if(!master->isOpen().has_value() || !master->isOpen().value())  ImGui::EndDisabled();

    // remove button
    if(ImGui::Button(("Remove##"+portName).c_str()))
        remove = true;

    // slaves
    for(auto& [id, slave] : master->getSlaves())
    {
        ImGui::Text("ID: %d [%d]", slave->id(), slave->baudRate());
        ImGui::SameLine();
        if(ImGui::Button(("View##"+portName+"id"+std::to_string(id)).c_str()))
            model.setDeviceViewTarget(std::pair<std::string, int>(portName, id));
    }
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

