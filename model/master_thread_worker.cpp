#include "master_thread_worker.h"
#include "model.h"

MasterThreadWorker::MasterThreadWorker(EObjectRef<Model> modelRef, EObjectRef<Master> masterRef)
{
    mModelRef = modelRef;
    mMasterRef = masterRef;
    mLLinkMaster = LLINK_Master_create();
    LLINK_Master_setRxCallback(mLLinkMaster, callRxCallback, this);
    LLINK_Master_setTxCallback(mLLinkMaster, callTxCallback, this);
}

void MasterThreadWorker::onMovedToThread(EThread &ethread)
{
    mTimer.moveToThread(ethread);
    mTimer.addTask(0, std::chrono::milliseconds(10), [&]{
        watch();
    });
    mTimer.start();
}

void MasterThreadWorker::onRemovedFromThread()
{
    mTimer.removeFromThread();
}


MasterThreadWorker::~MasterThreadWorker()
{
    mTimer.stop();
    mTimer.removeFromThread();
    LLINK_Master_destroy(mLLinkMaster);
}

LLINK_Error MasterThreadWorker::open()
{
    LLINK_Error err = LLINK_Master_open(mLLinkMaster, mPortName.c_str());
    if(!err) mTimer.start();
    return err;
}

LLINK_Error MasterThreadWorker::close()
{
    LLINK_Error err =  LLINK_Master_close(mLLinkMaster);
    if(!err) mTimer.stop();
    return err;
}

void MasterThreadWorker::setPortName(const std::string portName)
{
    mPortName = portName;
}

bool MasterThreadWorker::search(int baudRate)
{
    mSearchCancelFlag = false;
    LLINK_Master_setBaudRate(mLLinkMaster, baudRate);
    LLINK_Master_Summary summary;

    for(int id=0; id < 256; id++)
    {
        mModelRef.callQueued(&Model::slaveSearchProgressReported, 256, id + 1);

        threadInAffinity().handleQueuedEvents(); // check for search cancel
        if(mSearchCancelFlag)
            return false;

        if(LLINK_Master_pingDevice(mLLinkMaster, id, 10) != LLINK_ERROR_NO_ERROR)
            continue;
        if(LLINK_Master_summarizeDevice(mLLinkMaster, id, &summary, 10) != LLINK_ERROR_NO_ERROR)
            continue;

        LLINK_Master_printSummary(&summary);
        mModelRef.callQueued(&Model::slaveFoundReported, std::make_shared<Slave>(&summary, id, baudRate));
        LLINK_Master_freeSummary(&summary);

    }

    return true;
}

void MasterThreadWorker::cancelSearch()
{
    mSearchCancelFlag = true;
}

void MasterThreadWorker::addReadTarget(ReadTarget target)
{
    target.periodCountMs = 0;
    mReadTargets[{target.id, target.typeId}] = target;
}

void MasterThreadWorker::watch()
{
    for(auto& [idPair, target] : mReadTargets)
    {
        target.periodCountMs+=10;
        if(target.periodMs > target.periodCountMs)
            continue;

        std::cout<<"watching id: "<<(int)target.id<<", tid: "<<(int)target.typeId<<"("<<target.periodMs<<")"<<std::endl;
        target.periodCountMs=0;

        uint8_t* read = new uint8_t[target.typeSize * target.objectIds.size()];

        auto error = LLINK_Master_readDevice(
                mLLinkMaster,
                idPair.first, idPair.second,
                target.typeSize,
                target.objectIds.size(),
                &(target.objectIds[0]),
                read,
                100);

        if(error)
        {
            std::cout << "read error: " << error << std::endl;
            return;
        }

        mMasterRef.callQueued(&Master::targetReadReported, idPair.first, idPair.second, target.typeSize, target.objectIds, read);
    }
}

void MasterThreadWorker::writeObject(std::shared_ptr<MasterThreadWorker::WriteTarget> target)
{
    auto error = LLINK_Master_writeDevice(mLLinkMaster, target->id, target->typeId,
                             target->typeSize, target->objectIds.size(),
                             &(target->objectIds[0]), &(target->values[0]), 10);
    if(error)
    {
        std::cout << "write error: " << error << std::endl;
        return;
    }
}

void MasterThreadWorker::rxCallback(uint8_t *bytes, size_t &len)
{
    std::cout<<"rx"<<std::endl;
}

void MasterThreadWorker::txCallback(uint8_t *bytes, size_t &len)
{
    std::cout<<"tx"<<std::endl;
}

void MasterThreadWorker::callRxCallback(uint8_t *bytes, size_t len, void *arg)
{
    ((MasterThreadWorker*)arg)->rxCallback(bytes, len);
}

void MasterThreadWorker::callTxCallback(uint8_t *bytes, size_t len, void *arg)
{
    ((MasterThreadWorker*)arg)->txCallback(bytes, len);
}
