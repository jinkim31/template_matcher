#include "application.h"
#include <ethread.h>

using namespace ethr;

int main(int , char *[])
{
    EThread mainThread;
    EThread::provideMainThread(mainThread);
    Application app;    // hangs here until GUI closes
    app.removeFromThread();
    return 0;
}
