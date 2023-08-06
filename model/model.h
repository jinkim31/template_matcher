#ifndef LIGHTLINK_VIEW_MODEL_H
#define LIGHTLINK_VIEW_MODEL_H

#include <iostream>
#include <vector>
#include <memory>
#include <ethread.h>
#include "master.h"

class Model : public EObject
{
public:
    Model();
    ~Model();
    static std::vector<const char*> BAUD_RATE_STRINGS;
    struct SlaveSearchInfo
    {
        std::weak_ptr<Master> mMaster;
        int mBaudRateIdx;
        std::optional<bool> isDone;
        int currentPingingID;
    };
    bool addMaster(const std::string& portName);
    void removeMaster(const std::string& portName);
    std::weak_ptr<Master> getMaster(const std::string& portName);
    const std::map<std::string, std::shared_ptr<Master>>& getMasters();
    void updatePortNames();
    const std::vector<std::string>& getPortNames();
    SlaveSearchInfo& getSlaveSearchInfo();
    void searchSlave();
    void slaveSearchProgressReported(int currentPingingID);
private:
    std::map<std::string, std::shared_ptr<Master>> mMasters;
    std::vector<std::string> mPortNames;
    SlaveSearchInfo mSlaveSearchInfo;
};

#endif
