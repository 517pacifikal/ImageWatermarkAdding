#include <wx/wx.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <opencv2/opencv.hpp>
class MainFrame : public wxFrame {
public:

    /*
     * @brief 主框架 
     */
    MainFrame(const wxString& title);

private:

    /*
     * @brief 指定要添加水印的图片文件夹
     * @param 点击事件
     * 
     */
    void OnSelectFolder(wxCommandEvent& event);

    /*
     * @brief 选择要添加的水印图片
     * @param 点击事件
     *
     */
    void OnSelectWatermark(wxCommandEvent& event);

    /*
     * @brief 执行添加水印的程序
     * @param 点击事件
     *
     */
    void OnProcessImages(wxCommandEvent& event);

    /*
     * 文本框，点击选择待处理文件夹
     */
    wxStaticText* folderLabel;

    /*
     * 文本框，点击指定水印文件
     */
    wxStaticText* watermarkLabel;

    /*
     * 文件夹路径
     */
    std::string selectedFolder;

    /*
     * 水印路径
     */
    std::string selectedWatermark;
};