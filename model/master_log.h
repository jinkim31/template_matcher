#ifndef LIGHTLINK_VIEW_MASTER_LOG_H
#define LIGHTLINK_VIEW_MASTER_LOG_H


#include <iostream>
#include <vector>

class MasterLog
{
public:
    enum RxTx
    {
        RX,
        TX,
    };
    void addLog(const RxTx& rxtx, const std::vector<uint8_t>& data);
    void clear();
private:
    std::string mPortName;
    std::vector<std::pair<RxTx, std::vector<uint8_t>>> mLogs;
    std::vector<std::string> mLogsStr;
public:
    const std::string &portName() const;
    const std::vector<std::pair<RxTx, std::vector<uint8_t>>>& logs();
    const std::vector<std::string> & logsStr();
};

#endif
