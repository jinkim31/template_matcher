#include "master.h"
#include "model.h"
Master::Master(const std::string &portName, Model *model)
: mPortName(portName), mMasterThreadWorker(model->ref<Model>(), ref<Master>())
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

void Master::addSlave(std::shared_ptr<Slave> slave)
{
    mSlaves[slave->id()] = slave;
}

std::map<int, std::shared_ptr<Slave>> Master::getSlaves()
{
    return mSlaves;
}

void Master::addReadTargets(Slave *slave, const int &typeId, const int &typeSize, const std::vector<uint8_t> &objectIds,
                            const int &periodMs)
{
    MasterThreadWorker::ReadTarget readTarget;
    readTarget.id = slave->id();
    readTarget.typeId = typeId;
    readTarget.typeSize = typeSize;
    readTarget.objectIds = objectIds;
    readTarget.periodMs = periodMs;
    mMasterThreadWorker.callQueued(&MasterThreadWorker::addReadTarget, readTarget);
}

void
Master::targetReadReported(uint8_t id, uint8_t typeId, uint8_t typeSize, std::vector<uint8_t> objectIds, uint8_t **data)
{
    if(mSlaves.find(id) == mSlaves.end())
    {
        std::cerr<<"Master::targetReadReported() invalid slave id"<<std::endl;
        return;
    }

    int iObject = 0;
    for(const auto& objectId : objectIds)
    {
        if(mSlaves[id]->objectTable().find(typeId) == mSlaves[id]->objectTable().end())
        {
            std::cerr<<"Master::targetReadReported() invalid type id"<<std::endl;
            return;
        }
        std::cout<<"assigning "<<(int)id<<">"<<(int)typeId<<">"<<(int)objectId<<std::endl;
        std::cout<<"dst"<<(int)*(mSlaves[id]->objectTable()[typeId].objects[objectId].data.get())<<std::endl;
        std::cout<<"src"<<(int)*(data[iObject])<<std::endl;
        memcpy(mSlaves[id]->objectTable()[typeId].objects[objectId].data.get(), data[iObject], typeSize);
        mSlaves[id]->objectTable()[typeId].objects[objectId].dataValid = true;

        iObject++;
    }
    // TODO: delete data array
}
