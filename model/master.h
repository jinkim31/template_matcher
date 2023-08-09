#ifndef LLINK_MASTER_H
#define LLINK_MASTER_H

#include <ethread.h>
#include <epromise.h>
#include <optional>
#include "master_thread_worker.h"
#include "slave.h"

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
private:
    const std::string mPortName;
    MasterThreadWorker mMasterThreadWorker;
    EThread mMasterAcquisitionThread;
    std::optional<bool> mIsOpen;
    Model* mModel;
    std::map<int, std::shared_ptr<Slave>> mSlaves;
    ETimer mTimer;

    void watch();
};


#endif
