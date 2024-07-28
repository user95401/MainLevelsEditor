#pragma once
namespace fs {
    using namespace std::filesystem;
    inline std::error_code last_err_code;
    template <typename T> inline auto rtnWithErrLog(T rtn, std::string log) { log::error("{}", log); return rtn; }
    inline auto exists(path path) {
        return cocos::fileExistsInSearchPaths(path.string().c_str());
    }
    inline auto read(path path) {
        unsigned long file_size = 0;
        auto buffer = CCFileUtils::sharedFileUtils()->getFileData(path.string().c_str(), "rb", &file_size);
        std::string data = "read failed...";
        if (buffer && file_size != 0) data = std::string(reinterpret_cast<char*>(buffer), file_size);
        return data;
    }
    inline auto rename(path old_path, path new_path) {
        std::filesystem::rename(old_path, new_path, last_err_code);
        log::debug(
            "{}(\n\told_path \"{}\", \n\told_path \"{}\"\n): last_err_code={}, last_err_code.message={}",
            __func__, old_path, new_path, last_err_code, last_err_code.message()
        );
        return true;
    }
}