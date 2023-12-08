#include "MainFrame.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include <opencv2/opencv.hpp>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 200)) {
    // 创建界面元素
    folderLabel = new wxStaticText(this, wxID_ANY, "选择的文件夹：", wxPoint(150, 15));
    watermarkLabel = new wxStaticText(this, wxID_ANY, "选择的水印图片：", wxPoint(150, 55));

    // 创建选择文件夹按钮，并绑定选择文件夹事件
    wxButton* folderButton = new wxButton(this, wxID_ANY, "选择文件夹", wxPoint(10, 10));
    folderButton->Bind(wxEVT_BUTTON, &MainFrame::OnSelectFolder, this);

    // 创建选择水印图片按钮，并绑定选择水印图片事件
    wxButton* watermarkButton = new wxButton(this, wxID_ANY, "选择水印图片", wxPoint(10, 50));
    watermarkButton->Bind(wxEVT_BUTTON, &MainFrame::OnSelectWatermark, this);

    // 创建处理图片按钮，并绑定处理图片事件
    wxButton* processButton = new wxButton(this, wxID_ANY, "添加水印并导出", wxPoint(10, 90));
    processButton->Bind(wxEVT_BUTTON, &MainFrame::OnProcessImages, this);
}

// 处理选择文件夹事件
void MainFrame::OnSelectFolder(wxCommandEvent& event) {
    // 创建文件夹选择对话框
    wxDirDialog dialog(this, "选择文件夹");
    if (dialog.ShowModal() == wxID_OK) {
        // 获取所选文件夹路径，并更新界面上的文件夹标签
        selectedFolder = dialog.GetPath().ToStdString();
        wxFileName folderPath(dialog.GetPath());
        folderLabel->SetLabel("选择的文件夹：" + folderPath.GetFullPath());
    }
}

// 处理选择水印图片事件
void MainFrame::OnSelectWatermark(wxCommandEvent& event) {
    // 创建水印图片选择对话框
    wxFileDialog dialog(this, "选择水印图片");
    if (dialog.ShowModal() == wxID_OK) {
        // 获取所选水印图片路径，并更新界面上的水印图片标签
        selectedWatermark = dialog.GetPath().ToStdString();
        watermarkLabel->SetLabel("选择的水印图片：" + dialog.GetPath());
    }
}

// 处理处理图片事件
void MainFrame::OnProcessImages(wxCommandEvent& event) {
    if (selectedFolder.empty() || selectedWatermark.empty()) {
        wxMessageBox("请先选择文件夹和水印图片。", "警告", wxICON_WARNING);
        return;
    }

    // 读取水印图片
    cv::Mat watermarkImage = cv::imread(selectedWatermark, cv::IMREAD_UNCHANGED); // 加载图像包含 alpha 通道
    if (watermarkImage.empty()) {
        wxMessageBox("无法加载水印图片。", "警告", wxICON_WARNING);
        return;
    }

    // 设置输出文件夹路径
    std::string outputFolderPath = selectedFolder + "\\添加水印";

    // 如果输出文件夹不存在，则创建它
    if (!wxDirExists(outputFolderPath)) {
        wxMkdir(outputFolderPath);
    }

    // 打开选定的文件夹
    wxDir dir(selectedFolder);
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DEFAULT);
    while (cont) {
        // 获取当前处理的输入图片路径和输出图片路径
        std::string inputImagePath = selectedFolder + "\\" + filename.ToStdString();
        std::string outputImagePath = outputFolderPath + "\\" + filename.ToStdString();

        // 读取输入图片
        cv::Mat image = cv::imread(inputImagePath, cv::IMREAD_UNCHANGED); // 加载图像包含 alpha 通道
        if (image.empty()) {
            wxMessageBox("无法加载图片" + wxString(inputImagePath)+",或图片已处理完成", "警告", wxICON_WARNING);
            cont = dir.GetNext(&filename);
            continue;
        }

        // 如果图像不是4通道（含有alpha通道）的图像，将其转换为4通道图像
        if (image.channels() == 3) {
            cv::cvtColor(image, image, cv::COLOR_BGR2BGRA);
        }

        // 在图片右下角添加水印
        int posX = image.cols - watermarkImage.cols - 10; // 右边距
        int posY = image.rows - watermarkImage.rows - 10; // 下边距

        // 计算水印的透明度（假设透明度为0.5，可根据需求修改）
        float watermarkAlpha = 0.65; // 设置65%的不透明度

        // 合并水印和图像像素
        for (int y = 0; y < watermarkImage.rows; ++y) {
            for (int x = 0; x < watermarkImage.cols; ++x) {
                // 获取水印图像和目标图像的像素值
                cv::Vec4b& pixel = watermarkImage.at<cv::Vec4b>(y, x);  // 水印图像像素值
                cv::Vec4b& imagePixel = image.at<cv::Vec4b>(posY + y, posX + x);  // 目标图像像素值

                // 计算水印的透明度（根据图像顺序设置固定的水印透明度）
                float alpha = pixel[3] / 255.0 * watermarkAlpha; // 使用固定的水印透明度

                // 合并水印和图像的每个通道像素值（RGB通道）
                for (int c = 0; c < 3; ++c) {
                    // 根据透明度调整水印和图像像素值的混合比例
                    imagePixel[c] = static_cast<uchar>(
                        imagePixel[c] * (1.0 - alpha) + pixel[c] * alpha
                        );
                }
            }
        }



        // 保存带有水印的图片
        cv::imwrite(outputImagePath, image);

        cont = dir.GetNext(&filename);
    }

    wxMessageBox("水印添加完成。", "提示", wxICON_INFORMATION);
}
