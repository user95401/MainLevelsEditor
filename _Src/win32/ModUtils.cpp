#ifdef GEODE_IS_WINDOWS
#include "ModUtils.hpp"
#include <fstream>
#include <array>
#include <filesystem>

/*
    Write current process memory with bytes vector
    example:
    //Fixes trail cutting on high refresh rates
    write_bytes((DWORD)GetModuleHandleA("libcocos2d.dll") + 0xAE9BD, { 0xBB , 0xFF , 0x00 , 0x00 , 0x00 , 0x90 });
    //Verify Hack
    write_bytes(gd::base + 0x71D48, { 0xEB });
*/
bool WriteProcMem(const std::uintptr_t address, std::vector<uint8_t> const& bytes, std::string title) {
    //nothing to rewrite
    if (ReadProcMem(address, bytes.size()) == bytes) return false;
    //stringstream
    std::stringstream log;
    //some values
    std::string sbaserva = toHex((int)address - (int)GetModuleHandleA(0));
    if (address < (int)GetModuleHandleA(0)) sbaserva = "NA - CANT BE IN MODULE";
    std::string starg = toHex((int)address);
    //log func and addr
    log << __func__ << (title != "" ? std::format(" \"{}\"", title) : "") << " at " << starg;
    //rva also
    log << " (base rva: " << sbaserva << ")";
    //with what
    log << " with \"";
    for (uint8_t value : bytes) {
        log << ((value != bytes[0]) ? " " : "") << std::hex << (int)value;
    }
    log << "\"";
    //was what
    log << ", org was \"" << ReadProcMemAsStr(address, bytes.size() < 16 ? 16 : bytes.size()) << "\"";
    //a
    bool rtn = WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr);
    log.clear();
    return rtn;
}
bool WriteProcMem(const std::uintptr_t address, std::vector<uint8_t> const& bytes) {
    return WriteProcMem(address, bytes, "");
}

std::vector<uint8_t> ReadProcMem(DWORD address, int length) {
    std::vector<uint8_t> buffer(length);
    ReadProcessMemory(GetCurrentProcess(), (void*)address, buffer.data(), length, NULL);
    return buffer;
}

std::string ReadProcMemAsStr(DWORD address, int length) {
    std::stringstream asd;
    std::vector<uint8_t> asdser = ReadProcMem(address, length);
    bool FirstWasAdded = false;//brainfuckway
    for (uint8_t value : asdser) {
        asd << (FirstWasAdded ? " " : "") << std::hex << (int)value;
        FirstWasAdded = true;
    }
    return asd.str();
}

//std::vector<uint8_t> { intToBytes(777)[0], intToBytes(777)[1], intToBytes(777)[2],intToBytes(777)[3] }
std::vector<unsigned char> intToBytes(int value) {
    std::vector<unsigned char> result;
    result.push_back(value & 0x000000ff);
    result.push_back((value & 0x0000ff00) >> 8);
    result.push_back((value & 0x00ff0000) >> 16);
    result.push_back((value & 0xff000000) >> 24);
    return result;
}

std::string toHex(const size_tType& number) {
    std::stringstream stream;
    stream << std::hex << number;
    return stream.str();
}

//return string as relative path of random file in target directory
std::string getRandomFileNameFromDir(std::string path, std::string or_else) {
    if(cocos2d::CCFileUtils::sharedFileUtils()->isFileExist(path)){
        std::vector<std::string> files;
        for (auto& p : std::filesystem::directory_iterator(std::filesystem::current_path() / path)) {
            files.push_back(p.path().filename().string());
        }
        if (files.size() == 0) { return or_else; }
        return path + std::string("/") + files[rand() % files.size()].c_str();
    }
    else {
        //                          :skull:
        MessageBox(nullptr, std::string("The directory '" + path + "' isn't founded").c_str(), std::string("getRandomFileNameFromDir("+ path +", " + or_else + ")").c_str(), MB_ICONERROR | MB_OK);
    }
    return or_else;
}

//set every char of string to lower
void strToLower(std::string& str) {
    for (auto& c : str) c = tolower(c);
}

//its copy text to clipboard...
void copyToClipboard(const char* text) {
    const size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), text, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

//for exaple: "(*&*$() 101 cvcds /*-`1мивфд" => "(*%26*%24()+101+cvcds+%2F*-%601мивфд"
std::string url_encode(const std::string& value) {
    using namespace std;
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char)c);
        escaped << nouppercase;
    }

    return escaped.str();
}

//for exaple: "dW5ubw==" => "unno"
std::string base64_encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

//for exaple: "unno" => "dW5ubw=="
std::string base64_decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    int val = 0, valb = -8;
    for (char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static std::string NextCaption;
static std::string NextMsg;
static UINT NextType;
DWORD WINAPI MsgThread(void* hModule) {
    Sleep(100);
    MessageBoxA(nullptr, NextMsg.c_str(), NextCaption.c_str(), NextType);
    ExitThread(0);
    return 0;
}
void ShowSafeMessageBox(std::string Caption, std::string Msg, UINT uType) {
    NextCaption = Caption;
    NextMsg = Msg;
    if (!uType) NextType = (MB_ICONINFORMATION | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
    else NextType = uType;
    CreateThread(0, 0, MsgThread, nullptr, 0, 0);
}
#endif