#ifndef LIGHTLINK_VIEW_MASTER_THREAD_WORKER_H
#define LIGHTLINK_VIEW_MASTER_THREAD_WORKER_H

#include <ethread.h>
#include <etimer.h>
#include <lightlink_master.h>
#include "slave.h"

using namespace ethr;

// forward decl
class Model;
class Master;

class MasterThreadWorker : public EObject
{
public:
    struct ReadTarget
    {
        uint8_t id;
        uint8_t typeId;
        std::vector<uint8_t> objectIds;
        uint8_t typeSize;
        int baudRate;
        int periodMs;
        int periodCountMs;
    };

    struct WriteTarget
    {
        uint8_t id;
        uint8_t typeId;
        std::vector<uint8_t> objectIds;
        std::vector<uint8_t> values;
        uint8_t typeSize;
        int baudRate;
    };
    MasterThreadWorker(EObjectRef<Model> modelRef, EObjectRef<Master> masterRef);
    ~MasterThreadWorker();
    void setPortName(const std::string portName);
    LLINK_Error open();
    LLINK_Error close();
    bool search(int baudRate);
    void cancelSearch();
    void addReadTarget(ReadTarget target);
    void writeObject(std::shared_ptr<MasterThreadWorker::WriteTarget> target);
protected:
    void onMovedToThread(EThread &ethread) override;
    void onRemovedFromThread() override;
private:
    LLINK_Master* mLLinkMaster;
    std::string mPortName;
    ETimer mTimer;
    EObjectRef<Model> mModelRef;
    EObjectRef<Master> mMasterRef;
    std::map<std::pair<uint8_t, uint8_t>, ReadTarget> mReadTargets;
    bool mSearchCancelFlag;
    void watch();

};

#endif
