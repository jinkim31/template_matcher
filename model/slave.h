#ifndef SLAVE_H
#define SLAVE_H

#include <ethread.h>
#include <lightlink_master.h>
#include <map>
#include <vector>
#include <optional>

class Master;

class Slave
{
public:
    struct Object
    {
        uint8_t id;
        std::string name;
        LLINK_Access access;
        bool dataValid;
        std::unique_ptr<uint8_t[]> data;
        std::string rawText;
        std::string dataText;
    };
    struct TypedObjectList
    {
        int typeId;
        std::string typeName;
        size_t typeSize;
        std::vector<Object> objects;
        // watch
        int watchPeriodMs;
        std::vector<uint8_t> readableObjectIds;
    };

    Slave(LLINK_Master_Summary *summary, int id, int baudRate);
    ~Slave();
    int id();
    int baudRate();
    std::map<int, TypedObjectList>& objectTable();
    void addTypedReadTarget(const int &typeId);
    void writeObject(const uint8_t& typeId, const std::vector<uint8_t> &objectIds, const std::vector<std::vector<uint8_t>> &values);
    void setMaster(std::weak_ptr<Master> master);
private:
    int mId;
    int mBaudRate;
    std::map<int, TypedObjectList> mObjectTable;
    std::weak_ptr<Master> mMaster;
};

#endif
