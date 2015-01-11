#include "gltutapp.h"
#include "nx/nxcore.h"
#include "nx/sys/nxwindow.h"
#if defined(NX_SYSTEM_SDL2)
#include <SDL2/SDL_timer.h>
#endif
#include "nx/event/nxeventmanager.h"
#include "nx/sys/nxsysevents.h"
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
        nx::NXLogWarning("GLTutAPP: Unknown event type %x", pEvtData->type);
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

bool
GLTutApp::appInit(const int,
                  const char **)
{

    if (!_fileManager.init())
    {
        return false;
    }
    if (!_fileManager.mountArchive("data.yaaf", ""))
    {
        return false;
    }
    nx::NXLog("Mounted Archive 'data.yaaf'");

    system()->eventManager()->addListener(nx::NXSysEvtWinResize::sEvtType, this);
    return doInit();
}

void
GLTutApp::appTerm()
{
    doTerm();
    _gpuResManager.clear();
    _mediaManager.clear();
    _fileManager.shutdown();
    system()->eventManager()->removeListener(nx::NXSysEvtWinResize::sEvtType, this);
}

static double
getTicks()
{
#if defined(NX_SYSTEM_SDL2)
    return (double) SDL_GetTicks()/ 1000.0;
#else
    return 0.0;
#endif
}

void
GLTutApp::appRun()
{
    // update fps
    static double previous_seconds = getTicks();
    static int frame_count;
    double current_seconds = getTicks();
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

    // tick input
    inputManager()->tick();

    doRun(elapsed_seconds);
}
