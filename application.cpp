#include "application.h"
#include <IconsMaterialDesign.h>
#include <implot/implot.h>
#include "vutil.h"

Application::Application()
{
    this->moveToThread(EThread::mainThread());
    mModel.moveToThread(EThread::mainThread());
    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "Template";
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

    // dock
    runnerParams.imGuiWindowParams.enableViewports = false;

    // fonts
    runnerParams.callbacks.LoadAdditionalFonts = []{
        ImGuiIO& io = ImGui::GetIO();
        float baseFontSize = 15.0f * 2.0;
        float iconFontSize = baseFontSize*1.2;
        io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", baseFontSize);
        static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
        ImFontConfig icons_config;
        icons_config.GlyphOffset = {0, 10};
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = iconFontSize;
        io.Fonts->AddFontFromFileTTF( "assets/fonts/" FONT_ICON_FILE_NAME_MD, iconFontSize, &icons_config, icons_ranges );
    };

    ImmApp::Run(runnerParams, addOnsParams);
}

Application::~Application()
{
    mModel.removeFromThread();
}

void Application::render()
{
    ImGui::Begin("Test");
    if(ImGui::Button("Load Images"))
    {
       mModel.loadImage();
    }ImGui::SameLine();

    if(ImGui::Button("Match"))
    {
        mModel.templateMatch(cv::Point());
    }

    // image view
    if(!mModel.mImages.empty())
    {
        mModel.mParam.ImageDisplaySize = cv::Size(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y-100);
        ImmVision::Image("", mModel.mImages[mModel.mICurrentShowingImage], &mModel.mParam);
        mModel.mParam.RefreshImage = false;
        if(!mModel.mParam.WatchedPixels.empty())
        {
            mModel.templateMatch(mModel.mParam.WatchedPixels[0]);
            mModel.mParam.WatchedPixels.clear();
            mModel.mParam.ShowOptionsPanel = false;
        }
    }

    // image selector
    if(ImGui::Button("<"))
    {
       mModel.mICurrentShowingImage--;
       vutil::limit<int>(mModel.mICurrentShowingImage, 0, mModel.mImages.size()-1);
       mModel.mParam.RefreshImage = true;
    } ImGui::SameLine();
    ImGui::Text("%d", mModel.mICurrentShowingImage);ImGui::SameLine();
    if(ImGui::Button(">"))
    {
        mModel.mICurrentShowingImage++;
        vutil::limit<int>(mModel.mICurrentShowingImage, 0, mModel.mImages.size()-1);
        mModel.mParam.RefreshImage = true;
    }
    ImGui::End();

    ImGui::Begin("Result");
    // statistics
    auto& result = mModel.mResult;
    ImGui::Text("Mean: (%f, %f)", result.mean.x, result.mean.y);
    ImGui::Text("First Eigenvalue: %f", result.eigenvalues[0]);
    ImGui::Text("Second Eigenvalue: %f", result.eigenvalues[1]);

    // plot
    if (ImPlot::BeginPlot("Template Pos (px)", ImGui::GetContentRegionAvail()))
    {
        ImPlot::PlotScatter("Match", &mModel.mResult.xs[0], &mModel.mResult.ys[0], mModel.mResult.xs.size());
        ImPlot::EndPlot();
    }
    ImGui::End();

    ImPlot::ShowDemoWindow();
}
