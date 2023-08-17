#ifndef LLINK_MASTER_H
#define LLINK_MASTER_H

#include <ethread.h>
#include <epromise.h>
#include <optional>
#include "master_thread_worker.h"
#include "slave.h"
#include "../util.h"
#include "master_log.h"

using namespace ethr;

class Master : public EObject
{
public:
    Master(const std::string &portName, Model *model);
    ~Master();
    void open();
    void close();
    std::optional<bool> isOpen();
    void search(int baudRate);
    void cancelSearch();
    void addSlave(std::shared_ptr<Slave> slave);
    std::map<int, std::shared_ptr<Slave>> getSlaves();
    void addReadTargets(Slave *slave, const int &typeId, const int &typeSize, const std::vector<uint8_t> &objectIds,
                        const int &periodMs);
    void writeObject(Slave* slave, const uint8_t& typeId, const std::vector<uint8_t> &objectIds, const std::vector<uint8_t>& values);
    void targetReadReported(uint8_t id, uint8_t typeId, uint8_t typeSize, std::vector<uint8_t> objectIds, uint8_t *data);
    void rxReported(std::vector<uint8_t>&& bytes);
    void txReported(std::vector<uint8_t>&& bytes);
    void test(util::PassTester &&passTester);
    void openLog();
private:
    const std::string mPortName;
    MasterThreadWorker mMasterThreadWorker;
    EThread mMasterAcquisitionThread;
    std::optional<bool> mIsOpen;
    Model* mModel;
    std::map<int, std::shared_ptr<Slave>> mSlaves;
    std::weak_ptr<MasterLog> mLog;
};


#endif
