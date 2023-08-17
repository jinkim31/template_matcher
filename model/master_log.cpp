#include "master_log.h"
#include <iomanip>
#include <sstream>
#include <string>

const std::string &MasterLog::portName() const
{
    return mPortName;
}

void MasterLog::addLog(const MasterLog::RxTx &rxtx, const std::vector<uint8_t> &data)
{
    mLogs.emplace_back(rxtx, data);
    std::stringstream ss;
    const static std::string rxtxStr[] = {"RX", "TX"};
    ss <<"["<< rxtxStr[rxtx] << "] ";
    ss << std::uppercase << std::hex;
    for(const auto& byte : data)
    {
        ss << std::setw(2) << std::setfill('0') << (int)byte <<" ";
    }
    mLogsStr.emplace_back(ss.str());
}

const std::vector<std::pair<MasterLog::RxTx, std::vector<uint8_t>>> &MasterLog::logs()
{
    return mLogs;
}

const std::vector<std::string> & MasterLog::logsStr()
{
    return mLogsStr;
}

void MasterLog::clear()
{
    mLogsStr.clear();
    mLogs.clear();
}
