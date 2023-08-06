#include "master.h"
#include "model.h"
Master::Master(const std::string &portName, Model *model)
: mPortName(portName), mMasterThreadWorker(model->ref<Model>())
{
    mIsOpen = false;
    mModel = model;
    mMasterThreadWorker.setPortName(portName);
    mMasterAcquisitionThread.setName("masterAcqThread");
    mMasterThreadWorker.moveToThread(mMasterAcquisitionThread);
    mMasterAcquisitionThread.start();
}

Master::~Master()
{
    mMasterThreadWorker.callQueued(&MasterThreadWorker::close);
    mMasterAcquisitionThread.waitForEventHandleCompletion();
    mMasterThreadWorker.removeFromThread();
    mMasterAcquisitionThread.stop();
}

void Master::open()
{
    mIsOpen = std::nullopt;
    auto promise = new EPromise(mMasterThreadWorker.ref<MasterThreadWorker>(), &MasterThreadWorker::open);
    promise->then<int>(uref(), [&](LLINK_Error error){!error ? mIsOpen = true : mIsOpen = false; return 0;});
    promise->execute();
}

void Master::close()
{
    mIsOpen = std::nullopt;
    auto promise = new EPromise(mMasterThreadWorker.ref<MasterThreadWorker>(), &MasterThreadWorker::close);
    promise->then<int>(uref(), [&](LLINK_Error error){!error ? mIsOpen = false : mIsOpen = true; return 0;});
    promise->execute();
}

std::optional<bool> Master::isOpen()
{
    return mIsOpen;
}

void Master::search(int baudRate)
{
    mMasterThreadWorker.callQueued(&MasterThreadWorker::search, baudRate);
}

void Master::cancelSearch()
{
    mMasterThreadWorker.callQueued(&MasterThreadWorker::cancelSearch);
}



