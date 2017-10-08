
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <GUI/GUI.h>
#include <DirectX11/DirectX11.h>

XC_BEGIN_NAMESPACE_2(XC, GUI)
{
    using namespace Drawing2D;

    class TUserNativeWindow : public UserNativeWindow
    {
    public:
        TUserNativeWindow();

    private:
        void OnClicked();

    private:
        Button* mButton;
        TextBox* mTextBox;
        UserNativeWindow* mWindow;
    };

    TUserNativeWindow::TUserNativeWindow()
    {
        SetBoundary(Rectangle(Point(0, 0), Size(1000, 500)));
        HorizontalLayout* layout = new HorizontalLayout();
        mButton = new Button();
        mButton->SetBoundary(Drawing2D::Rectangle(Drawing2D::Point(200, 200), Drawing2D::Size(200, 200)));
        layout->AddWindow(*mButton);

        Label* nButton = new Label();
        nButton->SetBoundary(Drawing2D::Rectangle(Drawing2D::Point(0, 200), Drawing2D::Size(200, 200)));
       // nButton->SetText(L"��������");
        layout->AddWindow(*nButton);

        mButton->MClickedSignal.Add(this, &TUserNativeWindow::OnClicked);

        mTextBox = new TextBox();
        mTextBox->SetBoundary(Drawing2D::Rectangle(Drawing2D::Point(400, 200), Drawing2D::Size(200, 2000)));
        mTextBox->SetSizeAutoChangeable(false);
        layout->AddWindow(*mTextBox);

        mWindow = new UserNativeWindow();
        AddUserNativeWindow(*mWindow);
        layout->AddWindow(*mWindow);
        
        SetLayout(layout);
    }

    void TUserNativeWindow::OnClicked()
    {
        mButton->SetText(L"������ش򷨰�����");
        MessageWindow::Show(this, L"gewgw", L"������");
      //  mButton->SetBoundary(Rectangle(Point(100, 100), Size(1000, 1000)));
    }


    void Main()
    {
      //  wWinMain(GetModuleHandle(NULL), NULL, NULL, SW_MAXIMIZE);
        Application a;
     //   TUserNativeWindow window;
      //  window.Show();
        DirectX11::DirectX11NativeWindow window1;
        window1.Show();
        a.Execute();
    }

} XC_END_NAMESPACE_2;