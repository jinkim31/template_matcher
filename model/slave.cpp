#include "slave.h"

#include <utility>
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
        for(int objectId = 0; objectId < typedList->nObjects; objectId++)
        {
            LLINK_Master_Object* object = typedList->objects + objectId;
            Object objectModel;
            objectModel.id = objectId;
            objectModel.name = object->name;
            objectModel.access = object->access;
            objectModel.dataValid = false;
            if(object->access & LLINK_ACCESS_READ)
                typedListModel.readableObjectIds.push_back(objectId);
            objectModel.data = std::make_unique<uint8_t[]>(typedList->typeSize);
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
    mMaster = std::move(master);
}

void Slave::addTypedReadTarget(const int &typeId)
{
    mMaster.lock()->addReadTargets(
            this, typeId,
            mObjectTable[typeId].typeSize,
            mObjectTable[typeId].readableObjectIds,
            mObjectTable[typeId].watchPeriodMs);
}

void Slave::writeObject(const uint8_t &typeId, const std::vector<uint8_t> &objectIds, const std::vector<std::vector<uint8_t>> &values)
{
    std::vector<uint8_t> valuesCombined;
    for(const auto& value : values)
        for(const auto& byte : value)
            valuesCombined.push_back(byte);
    mMaster.lock()->writeObject(this, typeId, objectIds, valuesCombined);
}
