
#include "gltutapp.h"
#include "nx/io/nxiobase.h"
#include "nx/ogl/nxoglinternal.h"
#include "nx/ogl/nxoglprogram.h"
#include "nx/gpu/nxgpuprogramsource.h"
using namespace nx;


class GLTriangle : public GLTutApp
{
public:
    GLTriangle():
        GLTutApp("triangle"),
        _pProgram(nullptr),
        _vao(0),
        _vbo(0),
        _colourLoc(-1),
        _params(-1)
    {
    }


    bool doInit()
    {
        NXHdl hdl_prog = _gpuResManager.create("programs/triangle.nxprog", nx::kGPUResourceTypeProgram);

        if (!hdl_prog.valid())
        {
            return false;
        }

        if (!_gpuResManager.isLoaded(hdl_prog))
        {
            return false;
        }

        _pProgram = static_cast<NXOGLProgram*>(_gpuResManager.get(hdl_prog));

        if (!_pProgram)
        {
            return false;
        }

        GLfloat points[] = {
             0.0f,	0.5f,	0.0f,
             0.5f, -0.5f,	0.0f,
            -0.5f, -0.5f,	0.0f
        };

        /* tell GL to only draw onto a pixel if the shape is closer to the viewer */
        glEnable (GL_DEPTH_TEST); /* enable depth-testing */
        glDepthFunc (GL_LESS); /* depth-testing interprets a smaller value as "closer" */


        glGenBuffers (1, &_vbo);
        glBindBuffer (GL_ARRAY_BUFFER, _vbo);
        glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (GLfloat), points, GL_STATIC_DRAW);

        glGenVertexArrays (1, &_vao);
        glBindVertexArray (_vao);
        glEnableVertexAttribArray (0);
        glBindBuffer (GL_ARRAY_BUFFER, _vbo);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


        _colourLoc = glGetUniformLocation(_pProgram->oglHdl(), "inputColour");
        if (_colourLoc == -1)
        {
            NXLogError("Failed to located uniform 'inputColour'");
            return false;
        }

        glUseProgram(_pProgram->oglHdl());
        glUniform4f (_colourLoc, 1.0f, 0.0f, 0.0f, 1.0f);
        return true;
    }


    void doTerm()
    {

    }



    void onResize(const int w, const int h)
    {
        nx::NXLog("Resize %dx%d", w, h);
    }

    void doRun(const double elapsedSec)
    {
        (void) elapsedSec;
        /* wipe the drawing surface clear */
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport (0, 0, windowWidth(), windowHeight());

        glUseProgram(_pProgram->oglHdl());
        glBindVertexArray (_vao);
        /* draw points 0-3 from the currently bound VAO with current in-use shader */
        glDrawArrays (GL_TRIANGLES, 0, 3);
    }


protected:
    NXOGLProgram* _pProgram;
    GLuint _vao;
    GLuint _vbo;
    GLint _colourLoc;
    int _params;
};



GLTUTAPP(GLTriangle)

