#include <vector>
#include <string>
#include <iostream>
#include <windows.h>

import libpe;
using namespace libpe;

// ����һ���ص���������
typedef void (*ImportFunctionCallback)(const char*);

// ����������ʵ��
int AnalyzePE(const wchar_t* filepath, ImportFunctionCallback callback) {
    // ���ļ�
    HANDLE hFile = CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open file." << std::endl;
        std::string errorString = std::to_string(114514); // ������ת��Ϊ�ַ���
        callback(errorString.c_str()); // ���ûص������������ַ�����C���ָ��
        return -1; // �޷����ļ�ʱ����
    }

    // ��ȡDOSͷ
    IMAGE_DOS_HEADER dosHeader;
    DWORD bytesRead;
    if (!ReadFile(hFile, &dosHeader, sizeof(dosHeader), &bytesRead, NULL) || bytesRead != sizeof(dosHeader)) {
        std::cerr << "Failed to read DOS header." << std::endl;
        CloseHandle(hFile); // �ر��ļ����
        std::string errorString = std::to_string(114514); // ������ת��Ϊ�ַ���
        callback(errorString.c_str()); // ���ûص������������ַ�����C���ָ��
        return -1; // ��ȡʧ��ʱ����
    }

    // ���DOSͷ��e_magic�Ƿ�Ϊ'MZ'
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Not a valid PE file." << std::endl;
        CloseHandle(hFile); // �ر��ļ����
        std::string errorString = std::to_string(114514); // ������ת��Ϊ�ַ���
        callback(errorString.c_str()); // ���ûص������������ַ�����C���ָ��
        return -1; // ������Ч��PE�ļ�ʱ����
    }
    CloseHandle(hFile);
    Clibpe pe(filepath);
    const auto peImp = pe.GetImport();
    if (!peImp) {
        return -1;
    }
    for (const auto& itModule : *peImp) {
        for (const auto& itFunc : itModule.vecImportFunc) {
            // ���ûص�����
            callback(itFunc.strFuncName.c_str());
        }
    }
}