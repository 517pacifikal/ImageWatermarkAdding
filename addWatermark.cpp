#include <wx/wx.h>
#include "MainFrame.h"

// 声明保存和读取路径的函数
void SavePathsToConfig(const wxString& folderPath, const wxString& watermarkPath);
void LoadPathsFromConfig(wxString& folderPath, wxString& watermarkPath);

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MainFrame* frame = new MainFrame("添加水印示例");
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);