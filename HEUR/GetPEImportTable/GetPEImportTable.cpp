#include <vector>
#include <string>
#include <iostream>
#include <windows.h>

import libpe;
using namespace libpe;

// 定义一个回调函数类型
typedef void (*ImportFunctionCallback)(const char*);

// 导出函数的实现
int AnalyzePE(const wchar_t* filepath, ImportFunctionCallback callback) {
    // 打开文件
    HANDLE hFile = CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open file." << std::endl;
        std::string errorString = std::to_string(114514); // 将整数转换为字符串
        callback(errorString.c_str()); // 调用回调函数，传递字符串的C风格指针
        return -1; // 无法打开文件时返回
    }

    // 读取DOS头
    IMAGE_DOS_HEADER dosHeader;
    DWORD bytesRead;
    if (!ReadFile(hFile, &dosHeader, sizeof(dosHeader), &bytesRead, NULL) || bytesRead != sizeof(dosHeader)) {
        std::cerr << "Failed to read DOS header." << std::endl;
        CloseHandle(hFile); // 关闭文件句柄
        std::string errorString = std::to_string(114514); // 将整数转换为字符串
        callback(errorString.c_str()); // 调用回调函数，传递字符串的C风格指针
        return -1; // 读取失败时返回
    }

    // 检查DOS头的e_magic是否为'MZ'
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Not a valid PE file." << std::endl;
        CloseHandle(hFile); // 关闭文件句柄
        std::string errorString = std::to_string(114514); // 将整数转换为字符串
        callback(errorString.c_str()); // 调用回调函数，传递字符串的C风格指针
        return -1; // 不是有效的PE文件时返回
    }
    CloseHandle(hFile);
    Clibpe pe(filepath);
    const auto peImp = pe.GetImport();
    if (!peImp) {
        return -1;
    }
    for (const auto& itModule : *peImp) {
        for (const auto& itFunc : itModule.vecImportFunc) {
            // 调用回调函数
            callback(itFunc.strFuncName.c_str());
        }
    }
}