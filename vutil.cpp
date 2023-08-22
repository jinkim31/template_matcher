#include "vutil.h"
#include <iomanip>
#include <sstream>
#include <string>

std::string vutil::hexStr(uint8_t *data, size_t len)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    for( int i(0) ; i < len; ++i )
        ss << std::setw(2) << std::setfill('0') << (int)data[i] <<" ";
    return ss.str();
}

bool vutil::InputTextStdString(const char *label, std::string *str, ImGuiInputTextFlags flags,
                              ImGuiInputTextCallback callback, void *user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

std::vector<uint8_t> vutil::strToByteArray(std::string &str, size_t len)
{
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
    if(str.length()%2==1)
        str = '0' + str;
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < str.length(); i += 2) {
        std::string byteString = str.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    std::reverse(bytes.begin(), bytes.end());
    bytes.resize(len);
    std::reverse(bytes.begin(), bytes.end());

    return bytes;
}

