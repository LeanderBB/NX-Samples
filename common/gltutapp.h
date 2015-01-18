#ifndef GLTUTAPP_H
#define GLTUTAPP_H


#include <nx/nxcore.h>
#include <nx/sys/nxapp.h>
#include <nx/event/nxeventlistener.h>
#include <nx/io/nxfilemanager.h>
#include <nx/media/nxmediamanager.h>
#include <nx/gpu/nxgpuresourcemanager.h>

class GLTutApp : public nx::NXApp
{
public:
    GLTutApp(const char* name,
             const char* archiveName);

    virtual ~GLTutApp();

    virtual void appRun() NX_CPP_OVERRIDE;

    virtual void setAppOptions(const int ,
                               const char**,
                               nx::NXAppOptions&);

    virtual bool handleEvent(const nx::NXEventData* pEvtData) NX_CPP_OVERRIDE;

    int windowWidth();

    int windowHeight();

protected:

    bool onAppInit(const int,
                   const char **) NX_CPP_OVERRIDE;

    void onAppWillTerm() NX_CPP_OVERRIDE;

    void onWindowCreated() NX_CPP_OVERRIDE;

    void onWindowWillBeDestroyed() NX_CPP_OVERRIDE;

    virtual void doRun(const double currentSeconds) = 0;

    virtual void onResize(const int,
                          const int) {}

    virtual void doInit() {}

    virtual void doTerm() {}

protected:
    const nx::NXString _archiveName;
    nx::NXFileManager _fileManager;
    nx::NXMediaManager _mediaManager;
    nx::NXGPUResourceManager _gpuResManager;
};


#define GLTUTAPP(APP) NX_APP(APP)

#endif // GLTUTAPP_H
