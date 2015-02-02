
#include "gltutapp.h"
#include "nx/io/nxiobase.h"
#include "nx/ogl/nxoglinternal.h"
#include "nx/ogl/nxoglprogram.h"
#include "nx/gpu/nxgpuprogramsource.h"
#include "nx/resource/nxgpuprogramresource.h"
#include "nx/gpu/nxgpushaderinput.h"
using namespace nx;


class GLTriangle : public GLTutApp
{
public:
    GLTriangle():
        GLTutApp("triangle", "gltriangle"),
        _program(),
        _vao(),
        _vbo(),
        _colourLoc(-1),
        _params(-1)
    {
    }


    void doInit() NX_CPP_OVERRIDE
    {
        _program = _mediaManager.create("prog", "programs/triangle.nxprog");

        if (!_program.valid())
        {
            quit();
            return;
        }

        _mediaManager.load(_program);
        if (!_mediaManager.isLoaded(_program))
        {
            quit();
            return;
        }

        NXGPUProgramResourcePtr_t prog_ptr = _mediaManager.get(_program);

        if (!_program)
        {
            quit();
            return;
        }

        GLfloat points[] = {
            0.0f,	0.5f,	0.0f,
            0.5f, -0.5f,	0.0f,
            -0.5f, -0.5f,	0.0f
        };


        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LESS);


        NXGPUInterface* gpu_interface = _mediaManager.gpuInterface();

        // create gpu buffer
        NXGPUBufferDesc buffer_desc;
        buffer_desc.flags = 0;
        buffer_desc.mode = 0;
        buffer_desc.size = sizeof(float) * 9;
        buffer_desc.type = kGPUBufferTypeData;

        _vbo = gpu_interface->allocBuffer(buffer_desc, points);

        if (!_vbo)
        {
            NXLogError("Failed to create buffer");
            quit();
            return;
        }

        NXGPUShaderInput shader_input;

        // set input
        NXGPUShaderInputDesc input_desc;
        input_desc.binding_idx = 0;
        input_desc.data_count = 3;
        input_desc.data_idx = kGPUShaderInputIdxVertices;
        input_desc.data_offset = 0;
        input_desc.data_type = kGPUDataTypeFloat;

        if (!shader_input.addInput(input_desc))
        {
            NXLogError("Failed to set shader input");
            quit();
            return;
        }

        NXGPUBufferHdl buffer_hdl;
        buffer_hdl.gpuhdl = _vbo;

        if (!shader_input.addBuffer(buffer_hdl, 0))
        {
            NXLogError("Failed to set buffer binding");
            quit();
            return;
        }

        _vao = gpu_interface->allocShaderInput(shader_input);
        if (!_vao)
        {
            NXLogError("Failed to create shader input");
            quit();
            return;
        }

        _colourLoc = _pGPUInterface->uniformLocation(prog_ptr->gpuHdl(), "inputColour");
        if (_colourLoc == -1)
        {
            NXLogError("Failed to located uniform 'inputColour'");
            quit();
            return;
        }
        _pGPUInterface->bindProgram(prog_ptr->gpuHdl());
        glUniform4f (_colourLoc, 1.0f, 0.0f, 0.0f, 1.0f);
    }


    void doTerm() NX_CPP_OVERRIDE
    {
        // nothign to do, handled by managers
    }



    void onResize(const int w, const int h) NX_CPP_OVERRIDE
    {
        nx::NXLog("Resize %dx%d", w, h);
    }

    void doRun(const double elapsedSec) NX_CPP_OVERRIDE
    {
        (void) elapsedSec;
        /* wipe the drawing surface clear */
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport (0, 0, windowWidth(), windowHeight());

        _mediaManager.gpuInterface()->bindShaderInput(_vao);
        /* draw points 0-3 from the currently bound VAO with current in-use shader */
        glDrawArrays (GL_TRIANGLES, 0, 3);
    }


protected:
    NXHdl _program;
    NXHdl _vao;
    NXHdl _vbo;
    GLint _colourLoc;
    int _params;
};



GLTUTAPP(GLTriangle)

