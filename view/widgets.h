#ifndef LIGHTLINK_VIEW_WIDGETS_H
#define LIGHTLINK_VIEW_WIDGETS_H

#include <immapp/immapp.h>
#include <implot/implot.h>

namespace widget
{
void BackgroundContent(const ImColor &col, const int &rows=1);
void BackgroundContentAvail(const ImColor& col);

bool IconButton(const char* label, const ImVec2& size_arg = {0, 0});

enum class LedColor
{
    RED,
    GREEN,
    YELLOW,
};
void Led(const LedColor& color);
};

#endif