#include "gltutapp.h"
#include "nx/nxcore.h"
#include "nx/sys/nxwindow.h"
#if defined(NX_SYSTEM_SDL2)
#include <SDL2/SDL_timer.h>
#endif
#include "nx/event/nxeventmanager.h"
#include "nx/sys/nxsysevents.h"
#include "nx/util/nxtime.h"

GLTutApp::GLTutApp(const char* name):
    nx::NXApp(name),
    _fileManager(),
    _mediaManager(_fileManager),
    _gpuResManager(_mediaManager)
{
}

GLTutApp::~GLTutApp()
{

}

void
GLTutApp::setAppOptions(const int ,
                        const char**,
                        nx::NXAppOptions& options)
{
    options.width = 1280;
    options.height = 720;
    options.resizable = true;
}


bool
GLTutApp::handleEvent(const nx::NXEventData* pEvtData)
{
    switch(pEvtData->type)
    {
    case nx::kSystemEventWinResize:
    {
        const nx::NXSysEvtWinResize* p_evt = static_cast<const nx::NXSysEvtWinResize*>(pEvtData);
        onResize(p_evt->w, p_evt->h);
        break;
    }

    default:
        return NXApp::handleEvent(pEvtData);
    }
    return false;
}

int
GLTutApp::windowWidth()
{
    return system()->window()->width();
}

int
GLTutApp::windowHeight()
{
    return system()->window()->height();
}

void
GLTutApp::appRun()
{
    // update fps
    static double previous_seconds = ((double) nx::nxGetTicks())/ 1000.0;
    static int frame_count;
    double current_seconds = ((double) nx::nxGetTicks())/ 1000.0;
    double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;
    if (elapsed_seconds > 0.25)
    {
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[256];
        snprintf (tmp, 256, "%s @ fps: %.2f", nx::NXApp::name(), fps);
        this->system()->window()->setTile(tmp);
        frame_count = 0;
    }
    frame_count++;

    doRun(elapsed_seconds);
}

bool
GLTutApp::onAppInit(const int,
                    const char **)
{
    if (!_fileManager.init())
    {
        return false;
    }
#if defined(NX_OS_ANDROID)
    if (!_fileManager.mountArchive("/storage/sdcard0/data.yaaf", ""))
#else
    if (!_fileManager.mountArchive("data.yaaf", ""))
#endif
    {
        return false;
    }
    nx::NXLog("Mounted Archive 'data.yaaf'");

    system()->eventManager()->addListener(nx::NXSysEvtWinResize::sEvtType, this);
    return true;
}

void
GLTutApp::onAppWillTerm()
{
    _mediaManager.clear();
    _fileManager.shutdown();
    system()->eventManager()->removeListener(nx::NXSysEvtWinResize::sEvtType, this);
}

void
GLTutApp::onWindowCreated()
{
    doInit();
}

void
GLTutApp::onWindowWillBeDestroyed()
{
    doTerm();
    _gpuResManager.clear();
}

