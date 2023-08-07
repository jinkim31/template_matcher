#include "application.h"
#include "view/device_navigator.h"

Application::Application()
{
    this->moveToThread(EThread::mainThread());
    model.moveToThread(EThread::mainThread());
    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "LightView";
    runnerParams.callbacks.ShowGui = [&]{
        EThread::mainThread().handleQueuedEvents();
        render();
    };
    runnerParams.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    runnerParams.imGuiWindowParams.enableViewports = true;
    runnerParams.fpsIdling.enableIdling = false;
    ImmApp::AddOnsParams addOnsParams;

    runnerParams.imGuiWindowParams.showMenuBar = true;

    addOnsParams.withMarkdown = true;
    addOnsParams.withImplot = true;
    addOnsParams.withTexInspect = true;
    ImmApp::Run(runnerParams, addOnsParams);
}

Application::~Application()
{
    model.removeFromThread();
}

void Application::render()
{
    ImGui::ShowDemoWindow();
    DeviceNavigator::DeviceNavigator(model);

    if(!model.popupQueue().empty())
        ImGui::OpenPopup("Warning");
    if(ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(PopupModal(model.popupQueue().front()))
            model.popupQueue().pop();
        ImGui::EndPopup();
    }
}

bool Application::PopupModal(const std::pair<Model::PopupLevel, std::string>& popup)
{
    ImGui::Text("%s", popup.second.c_str());
    if(ImGui::Button("Ok"))
    {
        ImGui::CloseCurrentPopup();
        return true;
    }
    return false;
}

