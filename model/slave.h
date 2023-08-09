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
        int id;
        std::string name;
        LLINK_Access access;
        std::unique_ptr<uint8_t[]> value;
    };
    struct TypedObjectList
    {
        int typeId;
        std::string typeName;
        size_t typeSize;
        std::vector<Object> objects;
        // watch
        int watchPeriodMs;
        int watchPeriodCountMs;
    };

    Slave(LLINK_Master_Summary *summary, int id, int baudRate);
    ~Slave();
    int id();
    int baudRate();
    std::map<int, TypedObjectList>& objectTable();
    void setMaster(std::weak_ptr<Master> master);
private:
    int mId;
    int mBaudRate;
    std::map<int, TypedObjectList> mObjectTable;
    std::weak_ptr<Master> mMaster;
};

#endif
