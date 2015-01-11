#ifndef GLTUTAPP_H
#define GLTUTAPP_H


#include <nx/nxcore.h>
#include <nx/sys/nxapp.h>
#include <nx/event/nxeventlistener.h>
#include <nx/io/nxfilemanager.h>
#include <nx/media/nxmediamanager.h>
#include <nx/gpu/nxgpuresourcemanager.h>

class GLTutApp : public nx::NXApp, public nx::NXEventListener
{
public:
    GLTutApp(const char* name);

    virtual ~GLTutApp();

    virtual void appRun();

    virtual void setAppOptions(const int ,
                               const char**,
                               nx::NXAppOptions&);

    bool handleEvent(const nx::NXEventData* pEvtData);

    bool appInit(const int, const char **);

    void appTerm();

    int windowWidth();

    int windowHeight();

protected:
    virtual void doRun(const double currentSeconds) = 0;

    virtual void onResize(const int,
                          const int) {}

    virtual bool doInit() { return true;}

    virtual void doTerm() {}

protected:
    nx::NXFileManager _fileManager;
    nx::NXMediaManager _mediaManager;
    nx::NXGPUResourceManager _gpuResManager;
};


#define GLTUTAPP(APP) NX_APP(APP)

#endif // GLTUTAPP_H
