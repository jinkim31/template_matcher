#ifndef LIGHTLINK_VIEW_DEVICE_NAVIGATOR_H
#define LIGHTLINK_VIEW_DEVICE_NAVIGATOR_H

#include <immapp/immapp.h>
#include <implot/implot.h>
#include "../model/model.h"

namespace DeviceNavigator
{
    void DeviceNavigator(Model& model);
    bool MasterView(Model& model, const std::string& portName);
    void AddMasterModal(Model &model);
    void SearchSlaveModal(Model& model);
}

#endif
