#ifdef GEODE_IS_WINDOWS
#include <Windows.h>

/*
    Write current process memory with bytes vector
    example:
    //Fixes trail cutting on high refresh rates
    ModUtils::write_bytes((DWORD)GetModuleHandleA("libcocos2d.dll") + 0xAE9BD, { 0xBB , 0xFF , 0x00 , 0x00 , 0x00 , 0x90 });
    //Verify Hack
    ModUtils::write_bytes(gd::base + 0x71D48, { 0xEB });
*/
bool WriteProcMem(const std::uintptr_t, std::vector<uint8_t> const&);
bool WriteProcMem(const std::uintptr_t, std::vector<uint8_t> const&, std::string desc);

std::vector<uint8_t> ReadProcMem(DWORD address, int length);

std::string ReadProcMemAsStr(DWORD address, int length);

//std::vector<uint8_t> { intToBytes(777)[0], intToBytes(777)[1], intToBytes(777)[2],intToBytes(777)[3] }
std::vector<unsigned char> intToBytes(int value);

typedef size_t size_tType;
std::string toHex(const size_tType& number);

//return string as relative path of random file in target directory
std::string getRandomFileNameFromDir(std::string path, std::string or_else);

/*
    create strings vector by splitting string with separator char
    example:
    ModUtils::log(ModUtils::explode("zero_str.str1.strTwo", '.')[2]); //print strTwo
*/
std::vector<std::string> explode(const std::string& str, const char& ch);

//its copy text to clipboard...
void copyToClipboard(const char* text);

//set every char of string to lower
void strToLower(std::string& str);

//for example: "(*&*$() 101 cvcds /*-`1мивфд" => "(*%26*%24()+101+cvcds+%2F*-%601мивфд"
std::string url_encode(const std::string& value);

//for example: "dW5ubw==" => "unno"
std::string base64_encode(const std::string& in);

//for example: "unno" => "dW5ubw=="
std::string base64_decode(const std::string& in);

//MessageBox in new thread and sleep for 100ms
void ShowSafeMessageBox(std::string caption, std::string msg, UINT uType);
#endif
