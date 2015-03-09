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
#include "nx/ogl/nxgpuinterfaceogl.h"
#include "nx/gpu/nxgpuprogramsource.h"
#include "nx/resource/nxgpuprogramresource.h"
#include "nx/gpu/nxgpuinterface.h"
#include "nx/gpu/nxgpushaderinput.h"
#include "nx/font/nxfont.h"
#include "nx/gpu/nxgpubuffermanagerinterface.h"
using namespace nx;

enum
{
    kActiveTetureIdx = 1,
    kUniformLoc = 1
};

class GLFonts : public GLTutApp
{
public:
    GLFonts():
        GLTutApp("fonts", "glfonts"),
        _program(),
        _vao1(),
        _vao2(),
        _fontTex(),
        _font()
    {
    }


    void doInit() NX_CPP_OVERRIDE
    {
        _program = _mediaManager.createAndLoad("prog", "programs/font.nxprog");
        if (!_mediaManager.isLoaded(_program))
        {
            quit();
            return;
        }

        std::unique_ptr<NXIOBase> io (_fileManager.open("fonts/DroidSansMono.nxfont", kIOAccessModeReadBit));
        if (!io)
        {
            quit();
            return;
        }

        if (!_font.load(io.get()))
        {
            quit();
            return;
        }

        /*
        FILE* fp = fopen ("font.meta", "w");
        if (!fp)
        {
            nx::NXLogError("Failed to open '%s' for writing", "font.meta");
            quit();
            return;
        }

        // comment
        fprintf (fp, "// ascii_code xMin width yMin height yOffset\n");

        // write a line for each regular character
        for (int i = 32; i < 256; i++)
        {
            const nx::NXFontMetaData* meta = _font.metaData(i);
            fprintf (fp, "%i %f %f %f %f %f\n",
                     i, meta->xMin, meta->width, meta->yMin, meta->height,
                     meta->yOffset);
        }
        fclose (fp);
        */

        NXGPUTextureDesc font_tex_desc;
        _font.gpuTextureDesc(font_tex_desc);

        NXGPUInterface* gpu_interface = _mediaManager.gpuInterface();
        NXGPUInterfaceOGL* ogl_gpuinterface = static_cast<NXGPUInterfaceOGL*>(gpu_interface);

        _fontTex = ogl_gpuinterface->allocTexture(font_tex_desc);
        if (!_fontTex)
        {
            NXLogError("Failed to create font texture");
            quit();
            return;
        }

        NXOGLTexturePtr_t tex_ptr = ogl_gpuinterface->getTexture(_fontTex);
        if (!tex_ptr->upload(_font.imgBuffer(), _font.imgBufferSize(),
                             _font.hdr().atlasDimensionsPx,
                             _font.hdr().atlasDimensionsPx, 1, 0))
        {
            NXLogError("Failed to upload font texture");
            quit();
            return;
        }


        float x_pos = -0.75f;
        float y_pos = 0.2f;
        float pixel_scale = 190.0f;
        const char first_str[] = "abcdefghijklmnopqrstuvwxyz";

        _nPointsTxt1 = buildText(first_str, x_pos, y_pos, pixel_scale, _vao1);
        if (!_nPointsTxt1)
        {
            quit();
            return;
        }

        x_pos = -1.0f;
        y_pos = 1.0f;
        pixel_scale = 70.0f;
        const char second_str[] = "The human torch was denied a bank loan!";
        _nPointsTxt2 = buildText(second_str, x_pos, y_pos, pixel_scale, _vao2);
        if (!_nPointsTxt2)
        {
            quit();
            return;
        }

        glDisable (GL_DEPTH_TEST);
        glEnable (GL_BLEND);
        glCullFace (GL_BACK); // cull back face
        glFrontFace (GL_CCW); // GL_CCW for counter clock-wise
        glEnable (GL_CULL_FACE); // cull face
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // partial transparency
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
        NXGPUInterface* gpu_interface = _mediaManager.gpuInterface();

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor (0.2, 0.2, 0.6, 1.0);
        glViewport(0, 0, windowWidth(), windowHeight());

        // bind texture
        gpu_interface->bindTexture(_fontTex, kActiveTetureIdx);

        // bind program
        NXGPUProgramResourcePtr_t prog_ptr = _mediaManager.get(_program);
        gpu_interface->bindProgram(prog_ptr->gpuHdl());

        // draw first text
        gpu_interface->bindShaderInput(_vao1);
        glUniform4f(kUniformLoc, 1.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, _nPointsTxt1);

        // draw second text
        gpu_interface->bindShaderInput(_vao2);
        glUniform4f(kUniformLoc, 1.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, _nPointsTxt2);
    }

    // build text into vbo and return number of points
    int buildText(const char* text,
                  const float xloc,
                  const float yloc,
                  const float textScale,
                  NXHdl& vao)
    {
        const NXFontHdr hdr = _font.hdr();
        const int len = strlen(text);

        const int window_width = windowWidth();
        const int window_height = windowHeight();

        float cur_xloc = xloc;
        float cur_yloc = yloc;

        // allocate temporary buffer
        std::unique_ptr<float> points_ptr (new float[len * 24]); // 2 * 12 (vec2 * 6)
        float* points = points_ptr.get();

        // FIXME: add rows to font hdr
#define ATLAS_ROWS hdr.atlasColumns
#define ATLAS_COLS hdr.atlasColumns

        for (int i = 0; i < len; ++i)
        {
            int ascii_code = text[i];
            const NXFontMetaData* p_meta = _font.metaData(ascii_code);
            NX_ASSERT(p_meta);

            // row and column in atlas
            int atlas_col = (ascii_code - ' ') % hdr.atlasColumns;
            int atlas_row = (ascii_code - ' ') / hdr.atlasColumns;

            // texture coordinate in atlas, altas is squared so columns == rows
            float s = atlas_col * (1.0 / ATLAS_COLS);
            float t = (atlas_row + 1) * (1.0 / ATLAS_ROWS);

            // location of the triangle
            float x_pos = cur_xloc;

            float y_offset = 1.0 - p_meta->height - p_meta->yOffset;
            float y_pos = cur_yloc - textScale / window_height * y_offset;

            // update location
            if (i + 1 < len)
            {
                cur_xloc += p_meta->width * textScale / window_width;
            }

            // write vertex and texcoord
            // point 1
            points[i * 24 + 0] = x_pos;
            points[i * 24 + 1] = y_pos;
            points[i * 24 + 2] = s;
            points[i * 24 + 3] = 1.0 - t + 1.0 / ATLAS_ROWS;

            // point 2
            points[i * 24 + 4] = x_pos;
            points[i * 24 + 5] = y_pos - textScale / window_height;
            points[i * 24 + 6] = s;
            points[i * 24 + 7] = 1.0 - t;

            // point 3
            points[i * 24 + 8] = x_pos + textScale / window_width;
            points[i * 24 + 9] = y_pos - textScale / window_height;
            points[i * 24 + 10] = s + 1.0 / ATLAS_COLS;
            points[i * 24 + 11] = 1.0 - t;
            // point 4
            points[i * 24 + 12] = x_pos + textScale / window_width;
            points[i * 24 + 13] = y_pos - textScale / window_height;
            points[i * 24 + 14] = s + 1.0 / ATLAS_COLS;
            points[i * 24 + 15] = 1.0 - t;

            // point 5
            points[i * 24 + 16] = x_pos + textScale / window_width;
            points[i * 24 + 17] = y_pos;
            points[i * 24 + 18] = s + 1.0 / ATLAS_COLS;
            points[i * 24 + 19] = 1.0 - t + 1.0 / ATLAS_ROWS;

            // point 6
            points[i * 24 + 20] = x_pos;
            points[i * 24 + 21] = y_pos;
            points[i * 24 + 22] = s;
            points[i * 24 + 23] = 1.0 - t + 1.0 / ATLAS_ROWS;
        }

        NXGPUShaderInput shader_input;

        // vertex input
        NXGPUShaderInputDesc input_desc_vert;
        input_desc_vert.binding_idx = 0;
        input_desc_vert.data_count = 2;
        input_desc_vert.data_idx = kGPUShaderInputIdxVertices;
        input_desc_vert.data_offset = 0;
        input_desc_vert.data_type = kGPUDataTypeFloat;

        // texcoord input
        NXGPUShaderInputDesc input_desc_frag;
        input_desc_frag.binding_idx = 0;
        input_desc_frag.data_count = 2;
        input_desc_frag.data_idx = kGPUShaderInputIdxTexCoord0;
        input_desc_frag.data_offset = 2 * sizeof(float);
        input_desc_frag.data_type = kGPUDataTypeFloat;


        // create gpu buffer
        NXGPUBufferDesc buffer_desc;
        buffer_desc.flags = kGPUBufferAccessStaticBit;
        buffer_desc.mode = 0;
        buffer_desc.size = sizeof(float) * 24 * len;
        buffer_desc.type = kGPUBufferTypeData;
        buffer_desc.data = points;

        NXGPUInterface* gpu_interface = _mediaManager.gpuInterface();

        NXGPUBufferHdl vbo;
        vbo.gpuhdl = gpu_interface->allocBuffer(buffer_desc);
        vbo.offset = 0;
        if (!vbo.gpuhdl)
        {
            NXLogError("Failed to create buffer");
            return 0;
        }

        if (!shader_input.addInput(input_desc_vert))
        {
            NXLogError("Failed to set shader input vertices");
            return 0;
        }

        if (!shader_input.addInput(input_desc_frag))
        {
            NXLogError("Failed to set shader input vertices");
            return 0;
        }

        if (!shader_input.addBuffer(vbo, 0))
        {
            NXLogError("Failed to set shader input buffer");
            return 0;
        }

        vao = gpu_interface->allocShaderInput(shader_input);
        if (!vao)
        {
            NXLogError("Failed to create vao");
            return 0;
        }

        return len * 6;
    }

protected:
    NXHdl _program;
    NXHdl _vao1;
    NXHdl _vao2;
    NXHdl _fontTex;
    NXFont _font;
    int _nPointsTxt1;
    int _nPointsTxt2;
};

GLTUTAPP(GLFonts)
