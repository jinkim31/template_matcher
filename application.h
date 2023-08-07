#ifndef LIGHTLINK_VIEW_APPLICATION_H
#define LIGHTLINK_VIEW_APPLICATION_H

#include <ethread.h>
#include <epromise.h>
#include <immapp/immapp.h>
#include <implot/implot.h>
#include "model/model.h"

using namespace ethr;

class Application : public EObject
{
public:
    Application();
    ~Application();
    bool PopupModal(const std::pair<Model::PopupLevel, std::string>& popup);
private:
    void render();
    Model model;
};


#endif
