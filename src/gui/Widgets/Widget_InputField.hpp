//https://github.com/awwdev

#pragma once
#include "gui/Widgets/Widget_Window.hpp"

namespace rpg::gui {

template<typename T>
struct Widget_InputField
{
    com::Rectf  rect;
    com::String<30> label;
    com::String<30> value;
    bool isActive = false;

    T Update(gpu::RenderData& renderData, bool const insideWindow = true)
    {
        if (insideWindow == false)
            return;

        const com::Rectf back { 
            rect.x + rect.width/2,
            rect.y,
            rect.width/2,
            rect.height
        };

        const bool isMouseOnInput = com::IsPointInsideRect(wnd::glo::mouse_window_x, wnd::glo::mouse_window_y, back);
        if (wnd::MouseLeftButtonPressed()){
            isActive = isMouseOnInput;
        }

        if (isActive)
        {
            FOR_STRING(wnd::glo::chars, i) {

                if (wnd::glo::chars[i] == '\b'){
                    value.Pop();
                    continue;
                }

                if (value.Full() == false)
                    value.Append(wnd::glo::chars[i]);   
            }
        }

        AddText(renderData, label, rect.x, rect.y);
        AddRect(renderData, back, Colors::Black2_Alpha);
        if (isActive)
            AddRectOutline(renderData, back, Colors::Black3);
        AddText(renderData, value, back.x, back.y);

        return GetValue();
    }

    T Update(gpu::RenderData& renderData, Widget_Window& wnd)
    {
        if (wnd.isClosed)
            return;

        const bool insideWindow = wnd.CalculateRow(rect, LINE_HEIGHT);
        return Update(renderData, insideWindow);
    }

    T GetValue() const
    {
        return (T) std::atof(value.data);
    }

};

}//ns