#include "widgets.h"

void widget::BackgroundContent(const ImColor &col, const int &rows)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            {
                ImGui::GetCursorScreenPos().x+ImGui::GetContentRegionAvail().x,
                ImGui::GetCursorScreenPos().y+ImGui::GetFrameHeight() + (1 < rows ? ImGui::GetFrameHeightWithSpacing() * (rows-1): 0)},
            col, 0.0f,  ImDrawCornerFlags_All);
}

void widget::BackgroundContentAvail(const ImColor &col)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            {ImGui::GetCursorScreenPos().x+ImGui::GetContentRegionAvail().x, ImGui::GetCursorScreenPos().y+ImGui::GetContentRegionAvail().y},
            col, 0.0f,  ImDrawCornerFlags_All);
}

void widget::Led(const widget::LedColor &color)
{
    ImColor ledColor;
    switch (color)
    {
        case LedColor::GREEN:
            ledColor = ImColor(98,197,84);
            break;
        case LedColor::RED:
            ledColor = ImColor(237,106,95);
            break;
        case LedColor::YELLOW:
            ledColor = ImColor(245,191,76);
            break;
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    int width = 8;
    int height = ImGui::GetFrameHeight();

    draw_list->AddCircleFilled({ImGui::GetCursorScreenPos().x + width/2, ImGui::GetCursorScreenPos().y + height/2},
                               width/2.0, ledColor);
    ImGui::Dummy(ImVec2(width, height));
}

bool widget::IconButton(const char *label, const ImVec2 &size_arg)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    bool ret = ImGui::Button(label, size_arg);
    ImGui::PopStyleColor();

    return ret;
}
