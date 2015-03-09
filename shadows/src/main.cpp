//
// This file is part of the NX Project
//
// Copyright (c) 2014-2015 Leander Beernaert
//
// NX Project is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// NX Project is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with NX. If not, see <http://www.gnu.org/licenses/>.
//
#include "gltutapp.h"
#include "nx/io/nxiobase.h"
#include "nx/ogl/nxoglinternal.h"
#include "nx/ogl/nxoglprogram.h"
#include "nx/gpu/nxgpuprogramsource.h"
#include "nx/resource/nxgpuprogramresource.h"
#include "nx/gpu/nxgpushaderinput.h"
#include "nx/ogl/nxoglrendertarget.h"
#include "nx/gpu/nxgputexture.h"
#include "nx/math/nx3dmath.h"
#include "nx/math/nxtransform.h"
#include "nx/gpu/nxgpumesh.h"
#include "nx/resource/nxgpumeshresource.h"
#include "nx/sys/nxinputctx.h"

using namespace nx;


#define LOC_CAMVIEW_PROJ (int(1))
#define LOC_CAMVIEW_VIEW (int(0))
#define LOC_CAMVIEW_SHADOW (int(2))
#define LOC_CAMVIEW_SHADING (int(10))

#define LOC_LIGHT_MVP (int(0))

enum
{
    kNumMeshs = 4
};

const char* s_meshes[kNumMeshs] =
{
    "models/dragon.nx3d",
    "models/sphere.nx3d",
    "models/bunny.nx3d",
    "models/happy.nx3d"
};


class GLShadows : public GLTutApp, public NXInputCtx
{
public:
    GLShadows():
        GLTutApp("shadows", "glshadows"),
        NXInputCtx("shadows-input", 0)
    {
    }

    void doInit() NX_CPP_OVERRIDE;

    void doTerm() NX_CPP_OVERRIDE;

    void onResize(const int w, const int h) NX_CPP_OVERRIDE;

    void doRun(const double elapsedSec) NX_CPP_OVERRIDE;


protected:

    void renderScene(bool fromLight);

    void renderModel(const NXHdl hdl);

    bool rebuildRT();

    bool handleInputEvent(const NXInputEvent& pEvtData) NX_CPP_OVERRIDE;

    nx_u32 depthTexDim()
    {
        //nx_u32 w = windowWidth();
        //nx_u32 h = windowHeight();
        //return std::max(w, h);
        return 4096;
    }

protected:
    nx_u32 _vao = 0;
    NXHdl _progShadowCam;
    NXHdl _progShadowLight;
    NXHdl _progShadowLightView;
    NXHdl _meshes[kNumMeshs];
    NXOGLRenderTargetPtr_t _rt;
    NXOGLTexturePtr_t _tex, _texdbg;
    glm::mat4 _lightViewMat;
    glm::mat4 _lightProjMat;
    glm::mat4 _cameraViewMat;
    glm::mat4 _cameraProjMat;
    glm::mat4 _meshModelMat[kNumMeshs];
    bool _renderDepth=false;
    bool _paused = false;
};

void
GLShadows::doInit()
{

    // load shaders -----------------------------------------------------------
    _progShadowCam = _mediaManager.createAndLoad("shadow-cam","programs/shadow-camera.nxprog");
    if (!_mediaManager.isLoaded(_progShadowCam))
    {
        quit();
        return;
    }

    _progShadowLight = _mediaManager.createAndLoad("shadow-light","programs/shadow-light.nxprog");
    if (!_mediaManager.isLoaded(_progShadowLight))
    {
        quit();
        return;
    }

    _progShadowLightView = _mediaManager.createAndLoad("shadow-light-view","programs/shadow-light-view.nxprog");
    if (!_mediaManager.isLoaded(_progShadowLightView))
    {
        quit();
        return;
    }

    // load models ------------------------------------------------------------
    for (int i = 0; i < kNumMeshs; ++i)
    {
        _meshes[i] = _mediaManager.createAndLoad(s_meshes[i], s_meshes[i]);
        if (!_mediaManager.isLoaded(_meshes[i]))
        {
            quit();
            return;
        }
    }

    // create RT --------------------------------------------------------------
    if (!rebuildRT())
    {
        quit();
        return;
    }

    // setup intial state -----------------------------------------------------
    const glm::vec3 light_position(20.0f, 20.0f, 20.0f);
    const glm::vec3 view_position(0.0f, 0.0f, 40.0f);

    _lightProjMat = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);

    _lightViewMat = glm::lookAt(light_position,
                                glm::vec3(0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));

    _cameraViewMat = glm::lookAt(view_position,
                                 glm::vec3(0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    _cameraProjMat = glm::perspective(glm::radians(50.0f),
                                      (float)windowWidth() / (float)windowHeight(),
                                      1.0f, 200.0f);

    // setup intial OpenGL state-----------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glCreateVertexArrays(1, &_vao);

    inputManager()->addInputCtx(this);
}


void
GLShadows::doTerm()
{
    // release Render Target
    if (_vao)
    {
        glDeleteVertexArrays(1, &_vao);
    }
    _rt.reset();
    _tex.reset();

    inputManager()->remInputCtx(this);
}



void
GLShadows::onResize(const int w,
                    const int h)
{
    _cameraProjMat = glm::perspective(glm::radians(50.0f),
                                      (float)w / (float)h,
                                      1.0f, 200.0f);
    if (!rebuildRT())
    {
        quit();
    }
}

void
GLShadows::doRun(const double elapsedSec)
{
    (void) elapsedSec;


    static double last_time = 0.0;
    static double total_time = 0.0;
    if (!_paused)
    {
        total_time += (elapsedSec - last_time);
    }
    else
    {
        return;
    }

    const float time = (float)total_time + 30.0f;

    _meshModelMat[0] = nxRotate(time * 14.5f, 0.0f, 1.0f, 0.0f) *
            nxRotate(time * 14.5f, 0.0f, 1.0f, 0.0f) *
            nxRotate(20.0f, 1.0f, 0.0f, 0.0f) *
            nxTranslate(0.0f, -4.0f, 0.0f);


    _meshModelMat[1] = nxRotate(time * 3.7f, 0.0f, 1.0f, 0.0f) *
            nxTranslate(sinf(time * 0.37f) * 12.0f, cosf(time * 0.37f) * 12.0f, 0.0f) *
            nxScale(1.0f);

    _meshModelMat[2] = nxRotate(time * 6.45f, 0.0f, 1.0f, 0.0f) *
            nxTranslate(sinf(time * 0.25f) * 10.0f, cosf(time * 0.25f) * 10.0f, 0.0f) *
            nxRotate(time * 99.0f, 0.0f, 0.0f, 1.0f) *
            nxScale(0.5f);

    _meshModelMat[3] = nxRotate(time * 5.25f, 0.0f, 1.0f, 0.0f) *
            nxTranslate(sinf(time * 0.51f) * 14.0f, cosf(time * 0.51f) * 14.0f, 0.0f) *
            nxRotate(time * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            nxScale(1.0f);

    glEnable(GL_DEPTH_TEST);
    renderScene(true);



    if (_renderDepth)
    {
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(_vao);
        _pGPUInterface->bindProgram(_progShadowLightView);
        _texdbg->bind(0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else
    {
        renderScene(false);
    }
}


void
GLShadows::renderScene(bool fromLight)
{

    static const GLfloat ones[] = { 1.0f };
    static const GLfloat zero[] = { 0.0f };
    static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
    static const glm::mat4 scale_bias_matrix = glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                                         glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                         glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                                         glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));


    glm::mat4 light_vp_matrix = _lightProjMat * _lightViewMat;
    glm::mat4 shadow_sbpv_matrix = scale_bias_matrix * _lightProjMat * _lightViewMat;

    if (fromLight)
    {
        _rt->bindForReadWrite();
        nx_u32 tex_dim = depthTexDim();
        glViewport(0, 0, tex_dim, tex_dim);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(4.0f, 4.0f);

        _pGPUInterface->bindProgram(_progShadowLight);
        static const GPURTAttachment buffs[] = { kGPURTAttachmentColor0 };
        _rt->enableColorAttachments(buffs, 1);

        glClearNamedFramebufferfv(_rt->oglHdl(), GL_COLOR, 0, zero);
    }
    else
    {
        glViewport(0, 0, windowWidth(), windowHeight());
        glClearBufferfv(GL_COLOR, 0, gray);

        _pGPUInterface->bindProgram(_progShadowCam);
        _tex->bind(0);

        glUniformMatrix4fv(LOC_CAMVIEW_PROJ, 1, GL_FALSE, glm::value_ptr(_cameraProjMat));
        GLenum buff[] = {GL_BACK};
        glDrawBuffers(1, buff);
    }

    glClearBufferfv(GL_DEPTH, 0, ones);

    unsigned i;
    for (i = 0; i < kNumMeshs; i++)
    {
        glm::mat4& model_matrix = _meshModelMat[i];
        if (fromLight)
        {
            glm::mat4 mvp = light_vp_matrix * model_matrix;
            glUniformMatrix4fv(LOC_LIGHT_MVP, 1, GL_FALSE, glm::value_ptr(mvp));
        }
        else
        {
            glm::mat4 shadow_matrix = shadow_sbpv_matrix * model_matrix;
            glm::mat4 view_mat = _cameraViewMat * model_matrix;
            glUniformMatrix4fv(LOC_CAMVIEW_SHADOW, 1, GL_FALSE, glm::value_ptr(shadow_matrix));
            glUniformMatrix4fv(LOC_CAMVIEW_VIEW, 1, GL_FALSE,  glm::value_ptr(view_mat));
            glUniform1i(LOC_CAMVIEW_SHADING, _renderDepth ? 0 : 1);
        }
        renderModel(_meshes[i]);
    }

    if (fromLight)
    {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        glBindTextureUnit(0,0);
    }

}

void
GLShadows::renderModel(const NXHdl hdl)
{
    NXGPUMeshResourcePtr_t mesh_res_ptr = _mediaManager.get(hdl);
    NXGPUMeshPtr_t mesh_ptr = mesh_res_ptr->mesh();

    for (auto& sub_mesh : mesh_ptr->submeshes())
    {
        NXHdl vao = sub_mesh->gpuHdl();

        _pGPUInterface->bindShaderInput(vao);
        nx_u32 n_idx = sub_mesh->indexCount();
        if (n_idx)
        {
            glDrawElements(GL_TRIANGLES, n_idx, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, sub_mesh->vertexCount());
        }
    }
}

bool
GLShadows::rebuildRT()
{
    // release current rt
    _rt.reset();

    nx_u32 tex_dim = depthTexDim();

    // create rt textures
    NXGPUTextureDesc texdesc_depth, texdesc_depth_dbg;
    texdesc_depth.width = tex_dim;
    texdesc_depth.height = tex_dim;
    texdesc_depth.format = kGPUTextureFormatDepth32F;
    texdesc_depth.other = kGPUTextureDescOtherDepthRefCompare;

    texdesc_depth_dbg.width = tex_dim;
    texdesc_depth_dbg.height = tex_dim;
    texdesc_depth_dbg.format = kGPUTextureFormatR32F;

    NXOGLTexturePtr_t tex_depth, tex_depth_dbg;
    tex_depth = NXOGLTexture::create(texdesc_depth);
    tex_depth_dbg = NXOGLTexture::create(texdesc_depth_dbg);

    if (!tex_depth || !tex_depth_dbg)
    {
        NXLogError("Failed to create RT textures");
        return false;
    }

    // create render target
    NXOGLRenderTargetDesc rt_desc;
    rt_desc.attachments[kGPURTAttachmentDepth].format = kGPUTextureFormatDepth32F;
    rt_desc.attachments[kGPURTAttachmentDepth].tex = tex_depth;
    rt_desc.attachments[kGPURTAttachmentColor0].format = kGPUTextureFormatR32F;
    rt_desc.attachments[kGPURTAttachmentColor0].tex = tex_depth_dbg;

    _rt = NXOGLRenderTarget::create(rt_desc);

    if (!_rt)
    {
        NXLogError("Failed to create RT");
        return false;
    }

    _tex = tex_depth;
    _texdbg = tex_depth_dbg;

    return true;
}

bool
GLShadows::handleInputEvent(const NXInputEvent& pEvtData)
{

    if (pEvtData.type == kInputEventTypeKey)
    {
        if (pEvtData.evt.keyboard.key & kInputButtonStateDown)
        {
            InputKey key = NXInputKey(pEvtData.evt.keyboard);
            if (key == kInputKeyT)
            {
                _renderDepth = !_renderDepth;
            }

            if (key == kInputKeyP)
            {
                _paused = !_paused;
            }
        }
    }
    return false;
}



GLTUTAPP(GLShadows)

