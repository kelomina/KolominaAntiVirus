#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <Windows.h>
#include <thread>
#include <json/json.h>

// 导入 libpe 库
import libpe;
using namespace libpe;

typedef std::function<void(const std::vector<std::vector<std::string>>&)> ImportFunctionCallback;

// 解析导入列表格式的字符串，并返回导入列表
std::vector<std::vector<std::string>> ParseImportList(const std::string& importListStr) {
    std::vector<std::vector<std::string>> importLists;
    std::istringstream iss(importListStr);
    char ch;
    while (iss >> ch) {
        if (ch == '[') {
            std::vector<std::string> importList;
            while (iss >> ch && ch != ']') {
                if (ch == '\'' || ch == ',') continue;
                std::string functionName;
                while (iss >> ch && ch != '\'') {
                    functionName += ch;
                }
                importList.push_back(functionName);
            }
            importLists.push_back(importList);
        }
    }
    return importLists;
}

// 读取文件中的导入列表并返回导入列表
std::vector<std::vector<std::string>> ReadImportListFromFile(const std::wstring& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << std::string(filepath.begin(), filepath.end()) << std::endl;
        return {};
    }

    std::string importListStr;
    std::getline(file, importListStr);

    return ParseImportList(importListStr);
}

double CalculateSimilarity(const std::vector<std::vector<std::string>>& list1, const std::vector<std::vector<std::string>>& list2) {
    std::vector<std::vector<std::string>> intersection;
    for (const auto& importList : list1) {
        auto it = std::find(list2.begin(), list2.end(), importList);
        if (it != list2.end()) {
            intersection.push_back(importList);
        }
    }

    double similarity = static_cast<double>(intersection.size()) / (list1.size() > list2.size() ? list1.size() : list2.size());
    return similarity;
}

void ProcessGetPEImportTable(const std::wstring& filepath, int threadId, const std::vector<std::vector<std::string>>& referenceLists) {
    std::vector<std::vector<std::string>> importedFunctionLists = ReadImportListFromFile(filepath);
    double similarity = CalculateSimilarity(referenceLists, importedFunctionLists);
    std::cout << "线程 " << threadId << ": " << std::fixed << std::setprecision(2) << similarity << std::endl;
}

int main() {
    const std::wstring filepath = L"C:\\Users\\Kolomina\\OneDrive - MSFT\\KolominaAntiVirus_New\\7z.dll";
    std::vector<std::thread> threads;

    // 读取参考导入列表
    std::vector<std::vector<std::string>> referenceLists = ReadImportListFromFile(L"reference_import_list.txt");

    for (int i = 0; i < 1; ++i) {
        threads.emplace_back(ProcessGetPEImportTable, filepath, i + 1, std::cref(referenceLists));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
