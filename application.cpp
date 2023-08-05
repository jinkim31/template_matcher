#include "application.h"
#include "view/device_navigator.h"

Application::Application()
{
    HelloImGui::RunnerParams runnerParams;
    //runnerParams.callbacks.SetupImGuiStyle = Widget::setStyle;
    runnerParams.appWindowParams.windowTitle = "LightView";
    runnerParams.callbacks.ShowGui = [&]{
        mainThread.handleQueuedEvents();
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
    mainThread.setName("main");
    //model.moveToThread(mainThread);
    ImmApp::Run(runnerParams, addOnsParams);
}

void Application::render()
{
    ImGui::ShowDemoWindow();
    DeviceNavigator::DeviceNavigator(model);
    /*
    ImGui::Begin("Test");
        if(ImGui::Button("asdf"))
        {
            char** portNames;
            size_t nPorts = LLINK_Master_getPortNames(&portNames);
            for(int i=0; i<nPorts; i++)
                std::cout<<portNames[i]<<std::endl;
            Master master((std::string(portNames[2])));
            LLINK_Master_freePortNames(portNames);
            std::cout<<"open"<<master.open()<<std::endl;
            master.search({115200, 9600}, 0, 255);
            std::cout<<"close"<<master.close()<<std::endl;
        }
    ImGui::End();
     */
}

Application::~Application()
{

}
