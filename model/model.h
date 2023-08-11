#ifndef LIGHTLINK_VIEW_MODEL_H
#define LIGHTLINK_VIEW_MODEL_H

#include <iostream>
#include <vector>
#include <memory>
#include <ethread.h>
#include <lightlink_master.h>
#include "master.h"

class Model : public EObject
{
public:
    enum class PopupLevel
    {
        INFO,
        WARNING,
        ERROR,
    };

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
    void slaveSearchProgressReported(int nTotalPings, int nPings);
    void slaveFoundReported(std::shared_ptr<Slave> slave);
    void addPopup(PopupLevel popupLevel, const std::string& message);
    std::queue<std::pair<PopupLevel, std::string>>& popupQueue();
    void setDeviceViewTarget(const std::optional<std::pair<std::string, int>> &target);
    std::optional<std::pair<std::string, int>>& deviceViewTarget();
    void addMasterLog(std::shared_ptr<MasterLog> log);
    std::vector<std::shared_ptr<MasterLog>>& masterLogs();
private:
    std::map<std::string, std::shared_ptr<Master>> mMasters;
    std::vector<std::string> mPortNames;
    SlaveSearchInfo mSlaveSearchInfo;
    std::queue<std::pair<PopupLevel, std::string>> mPopupQueue;
    std::optional<std::pair<std::string, int>> mDeviceViewTarget;
    std::vector<std::shared_ptr<MasterLog>> mMasterLogs;
};

#endif
