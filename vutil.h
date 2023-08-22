#ifndef LIGHTLINK_VIEW_VUTIL_H
#define LIGHTLINK_VIEW_VUTIL_H

#include <iostream>
#include <immapp/immapp.h>

namespace vutil
{

class PassTester
{
public:
    PassTester(const int& num){mNum = num; std::cout<<"PassTester("<<num<<") constructed"<<std::endl;}
    PassTester(const PassTester& passTester){mNum = passTester.mNum; std::cout<<"PassTester("<<mNum<<") copy constructed"<<std::endl;}
    PassTester(PassTester&& passTester){mNum = passTester.mNum; std::cout<<"PassTester("<<mNum<<") moved"<<std::endl;}
    ~PassTester(){std::cout<<"PassTester("<<mNum<<") destructed"<<std::endl;}
    int num() const{return mNum;}
private:
    int mNum;
};

std::string hexStr(uint8_t* data, size_t len);
std::vector<uint8_t> strToByteArray(std::string &str, size_t len);
struct InputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);

template <typename T>
void limit(T &var, const T& min, const T& max)
{
    if(var < min)
        var = min;
    else if (max < var)
        var = max;
}

void print(){std::cout<<"print"<<std::endl;}
}

#endif
