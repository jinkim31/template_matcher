#include "slave.h"
#include "master.h"

Slave::Slave(LLINK_Master_Summary *summary, int id, int baudRate)
{
    mId = id;
    mBaudRate = baudRate;

    for(int typeIndex = 0; typeIndex < summary->nObjectTypes; typeIndex++)
    {
        LLINK_Master_TypedObjectList* typedList = summary->objectLists + typeIndex;
        TypedObjectList typedListModel;
        typedListModel.typeId = typeIndex;
        typedListModel.typeName = typedList->typeName;
        typedListModel.typeSize = typedList->typeSize;
        typedListModel.watchPeriodMs = 500;
        typedListModel.watchPeriodCountMs = 0;
        for(int objectIndex = 0; objectIndex < typedList->nObjects; objectIndex++)
        {
            LLINK_Master_Object* object = typedList->objects+objectIndex;
            Object objectModel;
            objectModel.id = objectIndex;
            objectModel.name = object->name;
            objectModel.access = object->access;
            objectModel.value = std::make_unique<uint8_t[]>(typedList->typeSize);
            typedListModel.objects.push_back(std::move(objectModel));
        }
        mObjectTable.insert({typeIndex, std::move(typedListModel)});
    }
}

Slave::~Slave()
{

}

int Slave::id()
{
    return mId;
}

int Slave::baudRate()
{
    return mBaudRate;
}

std::map<int, Slave::TypedObjectList> &Slave::objectTable()
{
    return mObjectTable;
}

void Slave::setMaster(std::weak_ptr<Master> master)
{
    mMaster = master;
}

void Slave::addTypedReadTarget(const int &typeId, const std::vector<int> &objectIds, const int &periodMs)
{
    mMaster.lock()->addReadTargets(this, typeId, objectIds, periodMs);
}
