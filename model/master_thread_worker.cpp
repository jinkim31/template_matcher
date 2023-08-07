#include "master_thread_worker.h"
#include "model.h"

MasterThreadWorker::MasterThreadWorker(EObjectRef<Model> modelRef)
{
    mIsOpen = false;
    mModelRef = modelRef;
    mLLinkMaster = LLINK_Master_create();
}

MasterThreadWorker::~MasterThreadWorker()
{
    LLINK_Master_destroy(mLLinkMaster);
}

LLINK_Error MasterThreadWorker::open()
{
    LLINK_Error err = LLINK_Master_open(mLLinkMaster, mPortName.c_str());
    return err;
}

LLINK_Error MasterThreadWorker::close()
{
    return LLINK_Master_close(mLLinkMaster);
}

void MasterThreadWorker::setPortName(const std::string portName)
{
    mPortName = portName;
}

bool MasterThreadWorker::search(int baudRate)
{
    mSearchCancelFlag = false;
    LLINK_Master_setBaudRate(mLLinkMaster, baudRate);
    for(int i=0; i<256; i++)
    {
        std::cout<<"pinging "<<i<<std::endl;
        auto t = std::chrono::high_resolution_clock::now();
        if(LLINK_Master_pingDevice(mLLinkMaster, i, 10) == LLINK_ERROR_NO_ERROR)
        {
            std::cout<<"found "<<i<<std::endl;
        }

        std::cout<<"time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count()<<std::endl;

        mModelRef.callQueued(&Model::slaveSearchProgressReported, i);
        threadInAffinity().handleQueuedEvents(); // check for search cancel
        if(mSearchCancelFlag)
            return false;
    }

    return true;
}

void MasterThreadWorker::cancelSearch()
{
    mSearchCancelFlag = true;
}

