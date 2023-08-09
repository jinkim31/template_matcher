#include "master_thread_worker.h"
#include "model.h"

MasterThreadWorker::MasterThreadWorker(EObjectRef<Model> modelRef)
{
    mModelRef = modelRef;
    mLLinkMaster = LLINK_Master_create();
}

void MasterThreadWorker::onMovedToThread(EThread &ethread)
{
    mTimer.moveToThread(ethread);
    mTimer.addTask(0, std::chrono::milliseconds(1000), []{
        std::cout<<"hi"<<std::endl;
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
    mReadTargets[{target.id, target.typeId}] = target;
}

