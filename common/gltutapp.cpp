#include "gltutapp.h"
#include "nx/nxcore.h"
#include "nx/sys/nxwindow.h"
#if defined(NX_SYSTEM_SDL2)
#include <SDL2/SDL_timer.h>
#endif
#include "nx/event/nxeventmanager.h"
#include "nx/sys/nxsysevents.h"
#include "nx/util/nxtime.h"
#include <nx/os/nxpath.h>

GLTutApp::GLTutApp(const char* name,
                   const char* archiveName):
    nx::NXApp(name),
    _archiveName(archiveName),
    _fileManager(),
    _mediaManager(_fileManager),
    _pGPUInterface(nullptr)
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
    nx::NXString archive_path;
#if defined(NX_OS_ANDROID)
    archive_path = "/storage/sdcard0/nx.samples.";
    archive_path += _archiveName;
    archive_path += ".yaaf";
#else
    archive_path = nx::NXPath::join(nx::NXPath::cwd(), "nx.samples.");
    archive_path += _archiveName;
    archive_path += ".yaaf";
#endif
    if (!_fileManager.mountArchive(archive_path.c_str(), ""))
    {
        return false;
    }
    nx::NXLog("Mounted Archive '%s'", archive_path.c_str());

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
    _pGPUInterface = nx::NXGPUInterface::create();

    if(!_pGPUInterface->init())
    {
        nx::NXLogError("Failed to init GPU Interface");
        quit();
        return;
    }
    _mediaManager.setGPUInterface(_pGPUInterface);
    doInit();
}

void
GLTutApp::onWindowWillBeDestroyed()
{
    doTerm();
    NX_SAFE_DELETE(_pGPUInterface);
}

