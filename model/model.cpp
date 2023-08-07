#include "model.h"

std::vector<const char*> Model::BAUD_RATE_STRINGS = {"1200","2400","4800","9600","14400","19200","38400","57600","115200","128000","256000"};

Model::Model()
{
    mSlaveSearchInfo.isDone = false;
    mSlaveSearchInfo.mBaudRateIdx = 8; // 115200
    mSlaveSearchInfo.currentPingingID = -1;
}

Model::~Model()
{
    for(auto const& [portName, master] : mMasters)
        master->removeFromThread();
}

bool Model::addMaster(const std::string &portName)
{
    if(mMasters.find(portName) != mMasters.end())
        return false;
    auto master = std::make_shared<Master>(portName, this);
    master->moveToThread(EThread::mainThread());
    mMasters.insert({portName,master});
    return true;
}

void Model::removeMaster(const std::string &portName)
{
    mMasters[portName]->removeFromThread();
    mMasters.erase(portName);
}

std::weak_ptr<Master> Model::getMaster(const std::string &portName)
{
    return mMasters[portName];
}

void Model::updatePortNames()
{
    mPortNames.clear();
    char** portNamesCStr;
    size_t nPorts = LLINK_Master_getPortNames(&portNamesCStr);
    for(int i=0; i<nPorts; i++)
        mPortNames.emplace_back(portNamesCStr[i]);
    LLINK_Master_freePortNames(portNamesCStr);
}

const std::vector<std::string>& Model::getPortNames()
{
    return mPortNames;
}

const std::map<std::string, std::shared_ptr<Master>> &Model::getMasters()
{
    return mMasters;
}

Model::SlaveSearchInfo &Model::getSlaveSearchInfo()
{
    return mSlaveSearchInfo;
}

void Model::searchSlave()
{
    auto master = mSlaveSearchInfo.mMaster.lock();
    master->search(std::stoi(BAUD_RATE_STRINGS[mSlaveSearchInfo.mBaudRateIdx]));
    mSlaveSearchInfo.isDone=std::nullopt;
}

void Model::slaveSearchProgressReported(int nTotalPings, int nPings, bool found, LLINK_Master_Summary summary)
{
    if(found)
    {
        LLINK_Master_printSummary(&summary);
        LLINK_Master_freeSummary(&summary);
    }
    mSlaveSearchInfo.currentPingingID = nPings;
}

std::queue<std::pair<Model::PopupLevel, std::string>> &Model::popupQueue()
{
    return mPopupQueue;
}

void Model::addPopup(Model::PopupLevel popupLevel, const std::string &message)
{
    mPopupQueue.emplace(popupLevel, message);
}
