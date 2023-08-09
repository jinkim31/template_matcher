#include "master_thread_worker.h"
#include "model.h"

MasterThreadWorker::MasterThreadWorker(EObjectRef<Model> modelRef)
{
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
    LLINK_Master_Summary summary;
    bool found;

    for(int id=0; id < 256; id++)
    {
        found = false;
        auto t = std::chrono::high_resolution_clock::now();
        if(LLINK_Master_pingDevice(mLLinkMaster, id, 10) == LLINK_ERROR_NO_ERROR)
        {
            LLINK_Master_summarizeDevice(mLLinkMaster, id, &summary, 10);

            mModelRef.callQueued(&Model::slaveFoundReported, std::make_shared<Slave>(&summary, id, baudRate));

            LLINK_Master_freeSummary(&summary);
        }

        mModelRef.callQueued(&Model::slaveSearchProgressReported, 256, id + 1);
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

