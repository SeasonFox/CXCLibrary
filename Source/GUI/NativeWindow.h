#pragma once

#include <Windows.h>
#include <Core/Core.h>
#include <Drawing2D/Drawing2D.h>
#include <Drawing2D/GDI/GDI.h>
#include "Defines.h"
#include "Application.h"

XC_BEGIN_NAMESPACE_2(XC, GUI)
{
    class NativeWindow : public CallableObject
    {
    public:
        HWND GetWindowID();

        const Drawing2D::Rectangle& GetBoundary();

        void SetBoundary(const Drawing2D::Rectangle& boundary);

        virtual void SetParent(NativeWindow& window);

        void SetText(const String& text);

    protected:
        HWND mHWND = NULL;

    private:
        Drawing2D::Rectangle mBoundary;
    }; 

} XC_END_NAMESPACE_2