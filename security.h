#include <iostream>
#include <string>
#include <Windows.h>
#include <intrin.h>

std::string encrypt(const std::string& input, const std::string& key) {
    std::string encrypted = input;
    for (int i = 0; i < input.length(); i++) {
        encrypted[i] = input[i] ^ key[i % key.length()];
    }
    return encrypted;
}

std::string decrypt(const std::string& input, const std::string& key) {
    return encrypt(input, key);
}

std::string getHWID()
{
    std::string hwid = "";

    int CPUInfo[4] = { 0 };
    __cpuid(CPUInfo, 1);

    hwid += std::to_string(CPUInfo[1]);
    hwid += std::to_string(CPUInfo[3]);
    hwid += std::to_string(CPUInfo[2]);

    WCHAR volumeName[MAX_PATH];
    WCHAR fileSystem[MAX_PATH];
    DWORD serialNumber;
    DWORD maxComponentLength;
    DWORD fileSystemFlags;

    if (GetVolumeInformation(
        L"C:\\",
        volumeName,
        sizeof(volumeName),
        &serialNumber,
        &maxComponentLength,
        &fileSystemFlags,
        fileSystem,
        sizeof(fileSystem))) {

        hwid += std::to_string(serialNumber);
    }

    return hwid;
}

std::string generateSecretKey() {
    std::string result = "";
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < 666; i++) {
        char randomChar = 'a' + (std::rand() % 26);
        if (std::rand() % 2 == 1) {
            randomChar = std::toupper(randomChar);
        }
        result += randomChar;
    }

    return result;
}