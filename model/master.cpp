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
    //mTimer.moveToThread(EThread::mainThread());
    //mTimer.start();
    mMasterAcquisitionThread.start();
    /*mTimer.addTask(0, std::chrono::milliseconds(0), [&]{
        static auto t = std::chrono::high_resolution_clock::now();
        //watch();
        std::cout<<"timer: "<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t).count()<<std::endl;
        t = std::chrono::high_resolution_clock::now();
    });*/
}

Master::~Master()
{
    mMasterThreadWorker.callQueued(&MasterThreadWorker::close);
    mMasterAcquisitionThread.waitForEventHandleCompletion();
    mMasterThreadWorker.removeFromThread();
    //mTimer.stop();
    //mTimer.removeFromThread();
    mMasterAcquisitionThread.stop();
}

void Master::open()
{
    mIsOpen = std::nullopt;
    auto promise = new EPromise(mMasterThreadWorker.ref<MasterThreadWorker>(), &MasterThreadWorker::open);
    promise->then<int>(uref(), [&](LLINK_Error error){
        if(!error) mIsOpen = true;
        else{mIsOpen = false; mModel->addPopup(Model::PopupLevel::WARNING, "Port open failed.");}
        return 0;});
    promise->execute();
}

void Master::close()
{
    mIsOpen = std::nullopt;
    auto promise = new EPromise(mMasterThreadWorker.ref<MasterThreadWorker>(), &MasterThreadWorker::close);
    promise->then<int>(uref(), [&](LLINK_Error error){
        if(!error) mIsOpen = false;
        else{mIsOpen = true; mModel->addPopup(Model::PopupLevel::WARNING, "Port close failed.");}
        return 0;});
    promise->execute();
}

std::optional<bool> Master::isOpen()
{
    return mIsOpen;
}

void Master::search(int baudRate)
{
    auto promise = new EPromise(mMasterThreadWorker.ref<MasterThreadWorker>(), &MasterThreadWorker::search);
    promise->then<int>(uref(), [&](bool ret){mModel->getSlaveSearchInfo().isDone = true; return 0;});
    promise->execute(baudRate);
}

void Master::cancelSearch()
{
    mMasterThreadWorker.callQueued(&MasterThreadWorker::cancelSearch);
}

void Master::watch()
{
    for(auto& slave : mSlaves)
    {
        for(auto& [typeId, typedList] : slave.second->objectTable())
        {
            typedList.watchPeriodCountMs += 10;
            if(typedList.watchPeriodMs <= typedList.watchPeriodCountMs)
            {
                static int c = 0;
                std::cout<<"detected "<<c++<<std::endl;
                typedList.watchPeriodCountMs = 0;
            }
        }
    }
}

void Master::addSlave(std::shared_ptr<Slave> slave)
{
    mSlaves[slave->id()] = slave;
}

std::map<int, std::shared_ptr<Slave>> Master::getSlaves()
{
    return mSlaves;
}

void Master::addReadTargets(Slave *slave, const int &typeId, const std::vector<int> &objectIds, const int &periodMs)
{

}



