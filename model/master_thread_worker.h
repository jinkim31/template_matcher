#ifndef LIGHTLINK_VIEW_MASTER_THREAD_WORKER_H
#define LIGHTLINK_VIEW_MASTER_THREAD_WORKER_H

#include <ethread.h>
#include <etimer.h>
#include <lightlink_master.h>
#include "slave.h"

using namespace ethr;

// forward decl
class Model;

class MasterThreadWorker : public EObject
{
public:
    MasterThreadWorker(EObjectRef<Model> modelRef);
    ~MasterThreadWorker();
    void setPortName(const std::string portName);
    LLINK_Error open();
    LLINK_Error close();
    bool search(int baudRate);
    void cancelSearch();
private:
    LLINK_Master* mLLinkMaster;
    ETimer mTimer;
    std::string mPortName;
    EObjectRef<Model> mModelRef;
    bool mSearchCancelFlag;
};

#endif
