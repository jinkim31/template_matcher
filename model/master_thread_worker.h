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
    struct ReadTarget
    {
        int id;
        int typeId;
        std::vector<int> objectIds;
        int baudRate;
        int periodMs;
    };
    MasterThreadWorker(EObjectRef<Model> modelRef);
    ~MasterThreadWorker();
    void setPortName(const std::string portName);
    LLINK_Error open();
    LLINK_Error close();
    bool search(int baudRate);
    void cancelSearch();
    void addReadTarget(ReadTarget target);
private:
protected:
    void onMovedToThread(EThread &ethread) override;

    void onRemovedFromThread() override;

private:
    LLINK_Master* mLLinkMaster;
    std::string mPortName;
    ETimer mTimer;
    EObjectRef<Model> mModelRef;
    std::map<std::pair<int, int>, ReadTarget> mReadTargets;
    bool mSearchCancelFlag;
};

#endif
