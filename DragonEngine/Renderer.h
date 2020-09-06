#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace dgn
{
    enum class DepthTest
    {
       Never = 0,
       Less,
       Equal,
       LEqual,
       Greater,
       NotEqual,
       GEqaul,
       Always
    };

    enum class DrawMode
    {
        Points = 0,
        Lines,
        LineLoop,
        LineStrip,
        Triangles,
        TriangleStrip,
        TriangleFan
    };

    enum class Face
    {
        Front = 0,
        Back,
        FrontBack = 4
    };

    enum class Winding
    {
        ClockWise = 0,
        CounterClockWise
    };

    enum class AlphaFactor
    {
        Zero                = 0x0000,
        One                 = 0x0001,
        SourceColor         = 0x0300,
        OneMinusSourceColor = 0x0301,
        DestColor           = 0x0306,
        OneMinusDestColor   = 0x0307,
        SourceAlpha         = 0x0302,
        OneMinusSourceAlpha = 0x0303,
        DestAlpha           = 0x0304,
        OneMinusDestAlpha   = 0x0305,
        ConstColor          = 0x8001,
        OneMinusConstColor  = 0x8002,
        ConstAlpha          = 0x8003,
        OneMinusConstAlpha  = 0x8004
    };

    enum class ClearFlag
    {
        color   = 0x4000,
        depth   = 0x0100,
        stencil = 0x0400
    };

    enum class RenderFlag
    {
        AlphaBlend         = 0x0BE2,
        CullFace           = 0x0B44,
        DepthTest          = 0x0B71,
        MultiSampling      = 0x809D,
        ScissorTest        = 0x0C11,
        StencilTest        = 0x0B90,
        SeamlessCubemaps   = 0x884F,
        LineSmoothing      = 0x0B20
    };

    enum class ClipMode
    {
        NegativeOneToOne = 0x935e,
        ZeroToOne
    };

    class Renderer
    {
    private:
        unsigned clear_flags = 0;
        DrawMode draw_mode = DrawMode::Triangles;
        unsigned bound_mesh_size = 0;

    public:
        bool initialize();
        void terminate();

        void clear();

        void bindMesh(const Mesh& mesh);
        void bindShader(const Shader& shader);
        void bindTexture(const Texture& texture, unsigned slot);
        void bindFramebuffer(const Framebuffer& framebuffer);

        void unbindMesh();
        void unbindShader();
        void unbindTexture(unsigned slot);
        void unbindFramebuffer();

        void drawBoundMesh() const;

        void setDepthTest(DepthTest func);
        void setClearColor(float red, float green, float blue);
        void setDrawMode(DrawMode mode);
        void setLineWidth(float width);
        void setViewport(unsigned x, unsigned y, unsigned width, unsigned height);
        void setCullFace(Face face);
        void setWinding(Winding winding);
        void setAlphaBlend(AlphaFactor source_factor, AlphaFactor dest_factor);
        void setClipMode(ClipMode mode);

        void enableClearFlag(ClearFlag flag);
        void disableClearFlag(ClearFlag flag);

        void enableFlag(RenderFlag value);
        void disableFlag(RenderFlag value);
    };
}
