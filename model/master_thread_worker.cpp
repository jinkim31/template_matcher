#include "master_thread_worker.h"
#include "model.h"

MasterThreadWorker::MasterThreadWorker(EObjectRef<Model> modelRef, EObjectRef<Master> masterRef)
{
    mModelRef = modelRef;
    mMasterRef = masterRef;
    mLLinkMaster = LLINK_Master_create();
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

        std::cout<<target.periodMs<<"ms"<<std::endl;
        target.periodCountMs=0;


        std::cout<<"read array:"<<target.objectIds.size()<<"*"<<(int)target.typeSize<<std::endl;
        uint8_t** read = new uint8_t*[target.objectIds.size()];
        for(int i=0; i<target.objectIds.size(); i++)
            read[i] = new uint8_t[target.typeSize];

        std::cout<<"read ret: "<<LLINK_Master_readDevice(
                mLLinkMaster,
                idPair.first, idPair.second,
                target.typeSize,
                target.objectIds.size(),
                &(target.objectIds[0]),
                read,
                10)<<std::endl;

        for(int i=0; i<4; i++)
        {
            std::cout<<i<<": "<<(int)read[i][0]<<std::endl;
        }

        mMasterRef.callQueued(&Master::targetReadReported, idPair.first, idPair.second, target.typeSize, target.objectIds, read);
    }
}
