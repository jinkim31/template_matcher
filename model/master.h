#ifndef LLINK_MASTER_H
#define LLINK_MASTER_H

#include <ethread.h>
#include <epromise.h>
#include <optional>
#include "master_thread_worker.h"

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
private:
    const std::string mPortName;
    MasterThreadWorker mMasterThreadWorker;
    EThread mMasterAcquisitionThread;
    std::optional<bool> mIsOpen;
    Model* mModel;
};


#endif
