#include <filesystem>
#include <iostream>
#include <fstream>
#include "SHA256/sha256.h"
#include <windows.h>

inline std::string CalculateFileHash(std::ifstream& file)
{
    sha256_buff buff;
    sha256_init(&buff);

    char buffer[1024];
    size_t size;

    while (file) {
        file.read(buffer, sizeof(buffer));
        size = file.gcount();
        sha256_update(&buff, buffer, size);
    }

    char hash[65] = { 0 };
    sha256_finalize(&buff);
    sha256_read_hex(&buff, hash);

    return hash;
}

bool CopyTextToClipboard(const std::string& text) {
    // ��������� ����� ������.
    if (!OpenClipboard(NULL)) {
        std::cerr << "�� ������� ������� ����� ������." << std::endl;
        return false;
    }

    // ������� ���������� ������ ������.
    if (!EmptyClipboard()) {
        std::cerr << "�� ������� �������� ����� ������." << std::endl;
        CloseClipboard();
        return false;
    }

    // �������� ����� � ����� ������.
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
    if (!hClipboardData) {
        std::cerr << "�� ������� �������� ������ ��� ������." << std::endl;
        CloseClipboard();
        return false;
    }

    char* pchData = (char*)GlobalLock(hClipboardData);
    if (strcpy_s(pchData, text.length() + 1, text.c_str()) != 0) {
        std::cerr << "�� ������� ����������� ����� � ����� ������." << std::endl;
        GlobalUnlock(hClipboardData);
        CloseClipboard();
        return false;
    }
    GlobalUnlock(hClipboardData);

    if (SetClipboardData(CF_TEXT, hClipboardData) == NULL) {
        std::cerr << "�� ������� ��������� ������ � ����� ������." << std::endl;
        CloseClipboard();
        return false;
    }

    // ��������� ����� ������.
    CloseClipboard();

    return true;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc > 1) {
        // ������ �������� (argv[1]) ����� ��������� ���� � ������������� �����.
        std::string filePath = argv[1];

        try {
            std::ifstream file(filePath, std::ios::binary);

            std::string hash = CalculateFileHash(file);

            if (!CopyTextToClipboard(hash)) {
                system("PAUSE");
                return 0;
            }

            std::cout << hash << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "������: " << e.what() << std::endl;
            system("PAUSE");
            return 0;
        }
        // ����� �� ������ �������� � ���� ������, ��������� ���� filePath.
    }
    else {
        std::cout << "�������� ���� �� .exe." << std::endl;
        system("PAUSE");
        return 0;
    }

    return 0;
}