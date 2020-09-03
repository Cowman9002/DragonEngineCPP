#include "src/Window.h"
#include "src/Camera.h"
#include "src/ShadowMap.h"
#include "src/BoundingBox.h"
#include "src/BoundingSphere.h"
#include "src/Plane.h"
#include "src/Triangle.h"

#include <stdio.h>
#include <algorithm>
#include <cmath>

#include <m3d/math1D.h>
#include <m3d/vec3.h>
#include <m3d/vec4.h>
#include <m3d/mat3x3.h>
#include <m3d/mat4x4.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define WINDOW_TITLE "DRAGON ENGINE"

#define SHADOW_SIZE 1024
#define SHADOW_CASCADES 3
#define SHADOW_NEAR 0.1
#define SHADOW_FAR 35
#define CASCADE_SPLIT_BLEND 0.4

#include "src/d_internal.h"
#include <glad/glad.h>

void updateCamera(dgn::Camera *camera, dgn::Window *window, float delta, bool controller);

void drawLineBox(const dgn::BoundingBox& box, int uniforms[], const dgn::Renderer& renderer);
void drawLineSphere(const dgn::BoundingSphere& sphere, int uniforms[], const dgn::Renderer& renderer);
void drawPlane(const dgn::Plane& plane, int uniforms[], const dgn::Renderer& renderer);
void drawPoint(const m3d::vec3& point, int uniforms[], const dgn::Renderer& renderer);
void drawTriangle(const dgn::Triangle& tri, int uniforms[], const dgn::Renderer& renderer);

int main(int argc, const char* argv[])
{
    dgn::Window main_window;

    main_window.initialize(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    main_window.setVsync(dgn::VsyncMode::Single);

    main_window.makeCurrent();

    main_window.getRenderer().setClearColor(0.0f, 0.0f, 0.0f);
    main_window.getRenderer().enableClearFlag(dgn::ClearFlag::color);
    main_window.getRenderer().enableClearFlag(dgn::ClearFlag::depth);
    main_window.getRenderer().enableFlag(dgn::RenderFlag::DepthTest);
    main_window.getRenderer().enableFlag(dgn::RenderFlag::SeamlessCubemaps);
    main_window.getRenderer().enableFlag(dgn::RenderFlag::CullFace);

    std::vector<float> screen_vertices =
    {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    std::vector<unsigned> screen_indices =
    {
        0, 2, 1,
        0, 3, 2
    };

    dgn::Mesh screen_mesh;
    screen_mesh.createFromData(screen_vertices, screen_indices);
    screen_mesh.setVertexSize(5);
    screen_mesh.addVertexAttrib(0, 3).addVertexAttrib(1, 2);
    screen_mesh.complete();

    dgn::Texture screen_texture;
    screen_texture.createFromData(nullptr, dgn::TextureData::Float, WINDOW_WIDTH, WINDOW_HEIGHT,
                                  dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Nearest,
                                  dgn::TextureStorage::RGBA16F, dgn::TextureStorage::RGB);

    dgn::Shader screen_shader;
    screen_shader.loadFromFiles("res/shaders/screen.vert", "", "res/shaders/screen.frag");

    dgn::Framebuffer screen_fb;
    screen_fb.create();
    screen_fb.setColorAttachment(screen_texture, 0);
    screen_fb.createDepthBit(WINDOW_WIDTH, WINDOW_HEIGHT);
    screen_fb.complete();

    /////////////////////////////////////////////////////

    dgn::ShadowMap shadowmap[SHADOW_CASCADES];

    for(int i = 0; i < SHADOW_CASCADES; i++)
    {
        shadowmap[i].initialize(SHADOW_SIZE, SHADOW_SIZE);
    }

    int cascade_depths_count = SHADOW_CASCADES + 1;
    float cascade_distances[cascade_depths_count];

    for(int i = 0; i < cascade_depths_count; i++)
    {
        float ioverm = (float)i / (float)SHADOW_CASCADES;
        float dist_uni = SHADOW_NEAR + (SHADOW_FAR - SHADOW_NEAR) * ioverm;
        float dist_log = SHADOW_NEAR * std::pow(SHADOW_FAR / SHADOW_NEAR, ioverm);

        cascade_distances[i] = m3d::lerp(dist_log, dist_uni, CASCADE_SPLIT_BLEND);
    }

    dgn::Shader shadow_shader;
    shadow_shader.loadFromFiles("res/shaders/shadow.vert", "", "");

    int shadow_u_light = shadow_shader.getUniformLocation("uLight");
    int shadow_u_model = shadow_shader.getUniformLocation("uModel");

    dgn::Shader::setEconst("CASCADES", SHADOW_CASCADES);

    /////////////////////////////////////////////////////

    dgn::Mesh skybox_mesh;
    skybox_mesh = dgn::Mesh::loadFromFile("res/models/skybox.obj")[0];

    dgn::Shader skybox_shader;
    skybox_shader.loadFromFiles("res/shaders/skybox.vert", "", "res/shaders/skybox.frag");

    int skybox_u_vp        = skybox_shader.getUniformLocation("uVP");
    int skybox_u_texture   = skybox_shader.getUniformLocation("uTexture");
    int skybox_u_sun_dir   = skybox_shader.getUniformLocation("uSunDir");

    /////////////////////////////////////////////////////

    std::vector<dgn::Mesh> scene;
    dgn::Mesh ball;

    dgn::Texture bricks_texture[5];
    dgn::Texture concrete_texture[5];
    dgn::Texture grass_texture[5];
    dgn::Texture plaster_texture[5];
    dgn::Texture paint_wood_texture[5];
    dgn::Texture planks_texture[5];
    dgn::Texture wood_texture[5];
    dgn::Texture metal_plates_texture[5];

    dgn::Cubemap skybox;
    dgn::Shader shader;
    dgn::Shader skin_shader;

    scene = dgn::Mesh::loadFromFile("res/models/forest_level.obj");
    ball = dgn::Mesh::loadFromFile("res/models/ball.obj")[0];

    dgn::Texture white_texture;
    white_texture.loadFromFile("res/textures/white.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    dgn::Texture black_texture;
    black_texture.loadFromFile("res/textures/black.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);

    dgn::Texture irrad_texture[2];
    irrad_texture[0].loadFromFile("res/textures/irrad_1.png", dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::SRGB);
    irrad_texture[1].loadFromFile("res/textures/irrad_2.png", dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::SRGB);

    bricks_texture[0].loadFromFile("res/textures/bricks_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    concrete_texture[0].loadFromFile("res/textures/concrete_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    grass_texture[0].loadFromFile("res/textures/ground_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    plaster_texture[0].loadFromFile("res/textures/paint_plaster_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    paint_wood_texture[0].loadFromFile("res/textures/paint_wood_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    planks_texture[0].loadFromFile("res/textures/planks_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    wood_texture[0].loadFromFile("res/textures/wood_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    metal_plates_texture[0].loadFromFile("res/textures/metal_plates_1/color.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);

    bricks_texture[1].loadFromFile("res/textures/bricks_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    concrete_texture[1].loadFromFile("res/textures/concrete_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    grass_texture[1].loadFromFile("res/textures/ground_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    plaster_texture[1].loadFromFile("res/textures/paint_plaster_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    paint_wood_texture[1].loadFromFile("res/textures/paint_wood_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    planks_texture[1].loadFromFile("res/textures/planks_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    wood_texture[1].loadFromFile("res/textures/wood_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    metal_plates_texture[1].loadFromFile("res/textures/metal_plates_1/rough.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);

    bricks_texture[2]       = black_texture;
    concrete_texture[2]     = black_texture;
    grass_texture[2]        = black_texture;
    plaster_texture[2]      = black_texture;
    paint_wood_texture[2]   = black_texture;
    planks_texture[2].loadFromFile("res/textures/planks_1/metal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    wood_texture[2]         = black_texture;
    metal_plates_texture[2] = white_texture;

    bricks_texture[3].loadFromFile("res/textures/bricks_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    concrete_texture[3].loadFromFile("res/textures/concrete_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    grass_texture[3].loadFromFile("res/textures/ground_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    plaster_texture[3].loadFromFile("res/textures/paint_plaster_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    paint_wood_texture[3].loadFromFile("res/textures/paint_wood_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    planks_texture[3].loadFromFile("res/textures/planks_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    wood_texture[3].loadFromFile("res/textures/wood_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    metal_plates_texture[3].loadFromFile("res/textures/metal_plates_1/normal.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);

    bricks_texture[4].loadFromFile("res/textures/bricks_1/ao.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    concrete_texture[4] = white_texture;
    grass_texture[4].loadFromFile("res/textures/ground_1/ao.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    plaster_texture[4].loadFromFile("res/textures/paint_plaster_1/ao.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    paint_wood_texture[4] = white_texture;
    planks_texture[4].loadFromFile("res/textures/planks_1/ao.png", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGB);
    wood_texture[4] = white_texture;
    metal_plates_texture[4] = white_texture;

    skybox.loadFromDirectory("res/textures/skyboxday/", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);
    //skybox.loadFromDirectory("res/textures/skyboxnight/", dgn::TextureWrap::Repeat, dgn::TextureFilter::Trilinear, dgn::TextureStorage::SRGB);

    shader.loadFromFiles("res/shaders/pbr_lite.vert", "", "res/shaders/pbr_lite.frag");
    skin_shader.loadFromFiles("res/shaders/skin.vert", "", "res/shaders/skin.frag");

    int shader_u_mvp        = shader.getUniformLocation("uMVP");
    //int shader_u_norm_mat   = shader.getUniformLocation("uNormMat");
    int shader_u_model_mat   = shader.getUniformLocation("uModelMat");
    int shader_u_skybox   = shader.getUniformLocation("uSkybox");
    int shader_u_irrad[2]   = {shader.getUniformLocation("uIrrad[0]"), shader.getUniformLocation("uIrrad[1]")};
    int shader_u_texture   = shader.getUniformLocation("uTexture");
    int shader_u_rough   = shader.getUniformLocation("uRough");
    int shader_u_metal   = shader.getUniformLocation("uMetalness");
    int shader_u_norm   = shader.getUniformLocation("uNorm");
    int shader_u_ao        = shader.getUniformLocation("uAO");
    int shader_u_cam_pos   = shader.getUniformLocation("uCamPos");

    int shader_u_light_mat[SHADOW_CASCADES];
    int shader_u_shadowmap[SHADOW_CASCADES];
    int shader_u_cascade_depths[SHADOW_CASCADES];

    for(int i = 0; i < SHADOW_CASCADES; i++)
    {
        shader_u_light_mat[i] = shader.getUniformLocation("uLightMat[" + std::to_string(i) + "]");
        shader_u_shadowmap[i] = shader.getUniformLocation("uShadowMap[" + std::to_string(i) + "]");
        shader_u_cascade_depths[i] = shader.getUniformLocation("uCascadeDepths[" + std::to_string(i) + "]");
    }

    int skin_u_mvp        = skin_shader.getUniformLocation("uMVP");
    int skin_u_norm_mat   = skin_shader.getUniformLocation("uNormMat");
    int skin_u_lut        = skin_shader.getUniformLocation("uLUT");


    std::vector<dgn::Texture*> textures = std::vector<dgn::Texture*>(scene.size());

    textures[0] = bricks_texture;
    textures[1] = plaster_texture;
    textures[2] = paint_wood_texture;
    textures[3] = wood_texture;
    textures[4] = grass_texture;
    textures[5] = concrete_texture;
    textures[6] = planks_texture;
    textures[7] = bricks_texture;

    dgn::Texture skin_lut;
    skin_lut.loadFromFile("res/textures/skin_lut.png", dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::SRGB);

    m3d::vec3 sun_dir = m3d::vec3(1.0, -1.0, -1.0).normalized();

    // Rendering to a cubemap ----- reflection probe stuffs

    ////////////////////////////////////////////////
    //              Reflection Probe              //
    ////////////////////////////////////////////////

    dgn::Camera camera;

    #define R_PROBE_SIZE 256
    dgn::Cubemap reflection_probe_base;
    reflection_probe_base.createFromData(nullptr, dgn::TextureData::Ubyte, R_PROBE_SIZE, R_PROBE_SIZE, dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::RGBA, dgn::TextureStorage::RGB);

    camera.width = R_PROBE_SIZE;
    camera.height = R_PROBE_SIZE;

    dgn::Framebuffer reflection_buffer;
    reflection_buffer.create();
    reflection_buffer.createDepthBit(R_PROBE_SIZE, R_PROBE_SIZE);

    /*
    East
    West
    Up
    Down
    North
    South
    */
    m3d::quat cam_rotations[] =
    {
        m3d::quat(180.0f * TO_RADS, m3d::vec3(1.0f, 0.0f, 0.0f)) *
            m3d::quat( 90.0f * TO_RADS, m3d::vec3(0.0f, -1.0f, 0.0f)),
        m3d::quat(180.0f * TO_RADS, m3d::vec3(1.0f, 0.0f, 0.0f)) *
            m3d::quat( 90.0f * TO_RADS, m3d::vec3(0.0f, 1.0f, 0.0f)),
        m3d::quat( 90.0f * TO_RADS, m3d::vec3(1.0f, 0.0f, 0.0f)),
        m3d::quat( 90.0f * TO_RADS, m3d::vec3(-1.0f, 0.0f, 0.0f)),
        m3d::quat(180.0f * TO_RADS, m3d::vec3(0.0f, 0.0f, 1.0f)) *
            m3d::quat(180.0f * TO_RADS, m3d::vec3(0.0f, 1.0f, 0.0f)),
        m3d::quat(180.0f * TO_RADS, m3d::vec3(0.0f, 0.0f, 1.0f)) *
            m3d::quat(  0.0f * TO_RADS, m3d::vec3(0.0f, 1.0f, 0.0f))
    };
    main_window.getRenderer().setViewport(0, 0, R_PROBE_SIZE, R_PROBE_SIZE);

    camera.position = m3d::vec3(-1.0f, 3.0f, 0.0f);
    //camera.position = m3d::vec3(8.0f, 1.5f, -6.0f);
    camera.fov = 90 * TO_RADS;

    for(unsigned i = 0; i < 6; i++)
    {
        camera.rotation = cam_rotations[i];

        reflection_buffer.setColorAttachment(reflection_probe_base, i, 0, 0);
        main_window.getRenderer().clear();

        main_window.getRenderer().setClipMode(dgn::ClipMode::ZeroToOne);
        main_window.getRenderer().setDepthTest(dgn::DepthTest::Less);
        main_window.getRenderer().setCullFace(dgn::Face::Back);

        main_window.getRenderer().bindShader(shader);

        dgn::Shader::uniform(shader_u_mvp, camera.getProjection() * camera.getView());
        //dgn::Shader::uniform(shader_u_norm_mat, m3d::mat3x3(1.0f));
        dgn::Shader::uniform(shader_u_model_mat, m3d::mat4x4(1.0f));
        dgn::Shader::uniform(shader_u_cam_pos, camera.position);
        dgn::Shader::uniform(shader_u_texture, 0);
        dgn::Shader::uniform(shader_u_rough, 1);
        dgn::Shader::uniform(shader_u_metal, 2);
        dgn::Shader::uniform(shader_u_norm, 3);
        dgn::Shader::uniform(shader_u_ao, 4);
        dgn::Shader::uniform(shader_u_skybox, 20);
        dgn::Shader::uniform(shader_u_irrad[0], 18);
        dgn::Shader::uniform(shader_u_irrad[1], 19);

        main_window.getRenderer().bindCubemap(skybox, 20);
        main_window.getRenderer().bindTexture(irrad_texture[0], 18);
        main_window.getRenderer().bindTexture(irrad_texture[1], 19);

        int k = 0;
        for(const dgn::Mesh& m : scene)
        {
            for(int j = 0; j < 5; j++)
            {
                main_window.getRenderer().bindTexture(textures[k][j], j);

            }
            k++;
            main_window.getRenderer().bindMesh(m);
            main_window.getRenderer().drawBoundMesh();
        }

        main_window.getRenderer().setDepthTest(dgn::DepthTest::LEqual);

        main_window.getRenderer().bindMesh(skybox_mesh);
        main_window.getRenderer().bindShader(skybox_shader);
        main_window.getRenderer().bindCubemap(skybox, 0);

        dgn::Shader::uniform(skybox_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());
        dgn::Shader::uniform(skybox_u_texture, 0);
        dgn::Shader::uniform(skybox_u_sun_dir, m3d::vec3());

        main_window.getRenderer().drawBoundMesh();

        m3d::mat4x4 ball_model2 = m3d::mat4x4(1.0f);
        ball_model2.translate(m3d::vec3(1.0f, 3.0f, 0.0f));
        main_window.getRenderer().bindShader(skin_shader);

        dgn::Shader::uniform(skin_u_mvp, camera.getProjection() * camera.getView() * ball_model2);
        //dgn::Shader::uniform(skin_u_model, ball_model2);
        dgn::Shader::uniform(skin_u_norm_mat, m3d::mat4x4(1.0f).toMat3x3());
        dgn::Shader::uniform(skin_u_lut, 0);

        main_window.getRenderer().bindTexture(skin_lut, 0);

        main_window.getRenderer().bindMesh(ball);
        main_window.getRenderer().drawBoundMesh();
    }


    dgn::Shader reflection_probe_shader;
    reflection_probe_shader.loadFromFiles("res/shaders/skybox.vert", "", "res/shaders/reflectionProbe.frag");

    int reflection_probe_u_vp        = reflection_probe_shader.getUniformLocation("uVP");
    int reflection_probe_u_texture   = reflection_probe_shader.getUniformLocation("uTexture");
    int reflection_probe_u_roughness = reflection_probe_shader.getUniformLocation("uRoughness");

    main_window.getRenderer().setDepthTest(dgn::DepthTest::LEqual);
    main_window.getRenderer().bindMesh(skybox_mesh);
    main_window.getRenderer().bindShader(reflection_probe_shader);
    dgn::Shader::uniform(reflection_probe_u_texture, 0);

    const unsigned maxNumMips = 6;

    dgn::Cubemap reflection_probe;
    reflection_probe.createFromData(nullptr, dgn::TextureData::Ubyte, R_PROBE_SIZE, R_PROBE_SIZE, dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGBA, dgn::TextureStorage::RGB);
    main_window.getRenderer().bindCubemap(reflection_probe_base, 0);
    for(unsigned mip = 0; mip < maxNumMips; mip++)
    {
        unsigned mip_size = R_PROBE_SIZE / std::pow(2, mip);

        main_window.getRenderer().setViewport(0, 0, mip_size, mip_size);
        reflection_buffer.createDepthBit(mip_size, mip_size);

        float roughness = (float)mip / (maxNumMips - 1);
        dgn::Shader::uniform(reflection_probe_u_roughness, roughness);

        for(unsigned i = 0; i < 6; i++)
        {
            camera.rotation = cam_rotations[i];

            reflection_buffer.setColorAttachment(reflection_probe, i, 0, mip);
            main_window.getRenderer().clear();

            dgn::Shader::uniform(reflection_probe_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());

            main_window.getRenderer().drawBoundMesh();
        }
    }

    dgn::Cubemap skybox_probe;
    skybox_probe.createFromData(nullptr, dgn::TextureData::Ubyte, skybox.getWidth(), skybox.getHeight(), dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Trilinear, dgn::TextureStorage::RGBA, dgn::TextureStorage::RGB);
    main_window.getRenderer().bindCubemap(skybox, 0);
    for(unsigned mip = 0; mip < maxNumMips; mip++)
    {
        unsigned mip_size = skybox.getWidth() / std::pow(2, mip);

        main_window.getRenderer().setViewport(0, 0, mip_size, mip_size);
        reflection_buffer.createDepthBit(mip_size, mip_size);

        float roughness = (float)mip / (maxNumMips - 1);
        dgn::Shader::uniform(reflection_probe_u_roughness, roughness);

        for(unsigned i = 0; i < 6; i++)
        {
            camera.rotation = cam_rotations[i];

            reflection_buffer.setColorAttachment(skybox_probe, i, 0, mip);
            main_window.getRenderer().clear();

            dgn::Shader::uniform(reflection_probe_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());

            main_window.getRenderer().drawBoundMesh();
        }
    }

    main_window.getRenderer().unbindFramebuffer();
    main_window.getRenderer().unbindCubemap(0);
    reflection_buffer.dispose();
    reflection_probe_base.dispose();

    main_window.getRenderer().enableClearFlag(dgn::ClearFlag::depth);
    camera.position = m3d::vec3(0.0f, 2.0f, 3.0f);
    camera.rotation = m3d::quat();
    camera.width = WINDOW_WIDTH;
    camera.height = WINDOW_HEIGHT;

    std::vector<unsigned> line_indices =
    {
        0, 1
    };

    dgn::Mesh line_mesh;
    line_mesh.createFromData(line_indices);
    line_mesh.complete();

    dgn::Shader line_shader;
    line_shader.loadFromFiles("res/shaders/line.vert", "", "res/shaders/line.frag");

    int line_u_mvp = line_shader.getUniformLocation("uMVP");
    int line_u_points[2] = {line_shader.getUniformLocation("uPoints[0]"), line_shader.getUniformLocation("uPoints[1]")};
    int line_u_color = line_shader.getUniformLocation("uColor");

    /////////////////////////////////////////////////////////
    //                      MAIN LOOP                      //
    /////////////////////////////////////////////////////////

    while(!main_window.shouldClose())
    {
        main_window.getInput().pollEvents();

        updateCamera(&camera, &main_window, 1.0f / 60.0f, true);

        if(main_window.getInput().getKeyDown(dgn::Key::R))
        {
            shader.loadFromFiles("res/shaders/pbr_lite.vert", "", "res/shaders/pbr_lite.frag");
            skin_shader.loadFromFiles("res/shaders/skin.vert", "", "res/shaders/skin.frag");
            screen_shader.loadFromFiles("res/shaders/screen.vert", "", "res/shaders/screen.frag");
            skybox_shader.loadFromFiles("res/shaders/skybox.vert", "", "res/shaders/skybox.frag");
            skin_lut.loadFromFile("res/textures/skin_lut.png", dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::SRGB);
        }


        if(main_window.getInput().getKeyDown(dgn::Key::P))
        {
            logMessagef("%f, %f, %f\n", camera.position.x, camera.position.y, camera.position.z);
        }

        for(int i = 0; i < SHADOW_CASCADES; i++)
        {
            shadowmap[i].updateProjectionMatFitted(camera, cascade_distances[i], cascade_distances[i+1], 10.0f, 1.0f / PI);
            shadowmap[i].updateViewMat(sun_dir);
        }

        m3d::mat4x4 mvp = camera.getProjection() * camera.getView();

        m3d::mat4x4 ball_model = m3d::mat4x4(1.0f);
        ball_model.translate(m3d::vec3(-1.0f, 3.0f, 0.0f));

        m3d::mat4x4 ball_model2 = m3d::mat4x4(1.0f);
        ball_model2.translate(m3d::vec3(1.0f, 3.0f, 0.0f));

        ///////////////////////////////////////////////////////
        //                  RENDER SHADOWS                   //
        ///////////////////////////////////////////////////////

        //TODO: make shadows work in zero to one depth space
        main_window.getRenderer().setClipMode(dgn::ClipMode::NegativeOneToOne);
        main_window.getRenderer().setDepthTest(dgn::DepthTest::Less);
        main_window.getRenderer().setCullFace(dgn::Face::Front);
        main_window.getRenderer().setViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);

        main_window.getRenderer().bindShader(shadow_shader);

        for(int i = 0; i < SHADOW_CASCADES; i++)
        {
            main_window.getRenderer().bindFramebuffer(shadowmap[i].getFramebuffer());
            main_window.getRenderer().clear();

            dgn::Shader::uniform(shadow_u_light, shadowmap[i].getLightMat());
            dgn::Shader::uniform(shadow_u_model, m3d::mat4x4(1.0f));

            for(const dgn::Mesh& m : scene)
            {
                main_window.getRenderer().bindMesh(m);
                main_window.getRenderer().drawBoundMesh();
            }

            dgn::Shader::uniform(shadow_u_model, ball_model);

            main_window.getRenderer().bindMesh(ball);
            main_window.getRenderer().drawBoundMesh();
            dgn::Shader::uniform(shadow_u_model, ball_model2);
            main_window.getRenderer().drawBoundMesh();
        }

        main_window.getRenderer().unbindFramebuffer();

        /////////////////////////////////////////////////////////
        //                  RENDER SCENE                       //
        /////////////////////////////////////////////////////////

        main_window.getRenderer().setClipMode(dgn::ClipMode::ZeroToOne);
        main_window.getRenderer().setDepthTest(dgn::DepthTest::Less);
        main_window.getRenderer().setCullFace(dgn::Face::Back);
        main_window.getRenderer().bindFramebuffer(screen_fb);
        main_window.getRenderer().setViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        main_window.getRenderer().clear();

        main_window.getRenderer().bindShader(shader);

        dgn::Shader::uniform(shader_u_mvp, mvp);
        //dgn::Shader::uniform(shader_u_norm_mat, m3d::mat3x3(1.0f));
        dgn::Shader::uniform(shader_u_model_mat, m3d::mat4x4(1.0f));
        dgn::Shader::uniform(shader_u_cam_pos, camera.position);
        dgn::Shader::uniform(shader_u_texture, 0);
        dgn::Shader::uniform(shader_u_rough, 1);
        dgn::Shader::uniform(shader_u_metal, 2);
        dgn::Shader::uniform(shader_u_norm, 3);
        dgn::Shader::uniform(shader_u_ao, 4);
        dgn::Shader::uniform(shader_u_skybox, 20);
        dgn::Shader::uniform(shader_u_irrad[0], 18);
        dgn::Shader::uniform(shader_u_irrad[1], 19);

        for(int i = 0; i < SHADOW_CASCADES; i++)
        {
            dgn::Shader::uniform(shader_u_light_mat[i], shadowmap[i].getLightMat());
            dgn::Shader::uniform(shader_u_shadowmap[i], 21 + i);
            main_window.getRenderer().bindTexture(shadowmap[i].getTexture(), 21 + i);

            m3d::vec4 v = m3d::vec4(0.0f, 0.0f, -cascade_distances[i + 1], 1.0f);
            float clip = (camera.getProjection() * v).z;
            dgn::Shader::uniform(shader_u_cascade_depths[i], clip);
        }

        main_window.getRenderer().bindCubemap(skybox_probe, 20);
        //main_window.getRenderer().bindCubemap(reflection_probe, 20);
        main_window.getRenderer().bindTexture(irrad_texture[0], 18);
        main_window.getRenderer().bindTexture(irrad_texture[1], 19);

        int i = 0;
        for(const dgn::Mesh& m : scene)
        {
            for(int j = 0; j < 5; j++)
            {
                main_window.getRenderer().bindTexture(textures[i][j], j);

            }
            i++;
            main_window.getRenderer().bindMesh(m);
            main_window.getRenderer().drawBoundMesh();
        }

        for(int j = 0; j < 5; j++)
        {
            main_window.getRenderer().bindTexture(metal_plates_texture[j], j);
        }

        dgn::Shader::uniform(shader_u_mvp, mvp * ball_model);
        dgn::Shader::uniform(shader_u_model_mat, ball_model);
        //main_window.getRenderer().bindTexture(black_texture, 2);
        //main_window.getRenderer().bindTexture(white_texture, 1);
        //main_window.getRenderer().bindTexture(white_texture, 0);

        main_window.getRenderer().bindCubemap(reflection_probe, 20);
        main_window.getRenderer().bindMesh(ball);
        main_window.getRenderer().drawBoundMesh();

        main_window.getRenderer().bindShader(skin_shader);

        dgn::Shader::uniform(skin_u_mvp, mvp * ball_model2);
        //dgn::Shader::uniform(skin_u_model, ball_model2);
        dgn::Shader::uniform(skin_u_norm_mat, m3d::mat4x4(1.0f).toMat3x3());
        dgn::Shader::uniform(skin_u_lut, 0);

        main_window.getRenderer().bindTexture(skin_lut, 0);

        main_window.getRenderer().drawBoundMesh();

        /////////////////////////////////////////////
        //              RENDER CUBEMAP             //
        /////////////////////////////////////////////

        main_window.getRenderer().setDepthTest(dgn::DepthTest::LEqual);

        main_window.getRenderer().bindMesh(skybox_mesh);
        main_window.getRenderer().bindShader(skybox_shader);
        main_window.getRenderer().bindCubemap(skybox, 0);

        dgn::Shader::uniform(skybox_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());
        dgn::Shader::uniform(skybox_u_texture, 0);
        dgn::Shader::uniform(skybox_u_sun_dir, sun_dir);

        //dgn::Shader::uniform(reflection_probe_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());
        //dgn::Shader::uniform(reflection_probe_u_texture, 0);
        //dgn::Shader::uniform(reflection_probe_u_roughness, main_window.getInput().getGamepadAxis(0, dgn::GamepadAxis::LeftX, 0.1f));

        main_window.getRenderer().drawBoundMesh();

        /////////////////////////////////////////////
        //            RENDER COLLIDERS             //
        /////////////////////////////////////////////

        //main_window.getRenderer().setDepthTest(dgn::DepthTest::Always);

        main_window.getRenderer().setDrawMode(dgn::DrawMode::Lines);
        //main_window.getRenderer().setDrawMode(dgn::DrawMode::Triangles);

        main_window.getRenderer().bindMesh(line_mesh);
        main_window.getRenderer().bindShader(line_shader);
        dgn::Shader::uniform(line_u_mvp, camera.getProjection() * camera.getView());

        std::vector<dgn::Collider*> colliders;

        m3d::vec3 test_collider_pos = camera.position + m3d::vec3(0.0f, 0.0f, -1.0f) * camera.rotation;
        dgn::BoundingSphere test_collider = dgn::BoundingSphere(test_collider_pos, 0.2f);
        //dgn::BoundingBox test_collider = dgn::BoundingBox(test_collider_pos + m3d::vec3(0.2), test_collider_pos - m3d::vec3(0.2));
        //dgn::Plane test_collider = dgn::Plane(m3d::vec3(0.0f, 1.0f, 0.0f), test_collider_pos.y);
//        dgn::Triangle test_collider = dgn::Triangle(test_collider_pos + m3d::vec3(-0.5f, -0.5f, 0.0f),
//                                                    test_collider_pos + m3d::vec3(0.0f, 0.5f, 0.0f),
//                                                    test_collider_pos + m3d::vec3(0.5f, -0.5f, 0.0f));

        dgn::BoundingBox box1 = dgn::BoundingBox(m3d::vec3(0.5f, 2.5f, 5.5f), m3d::vec3(-0.5f, 1.5f, 4.5f)).normalize();
        dgn::BoundingSphere sphere1 = dgn::BoundingSphere(m3d::vec3(1.0f, 3.0f, 0.0f), 0.5f);
        dgn::Plane plane1 = dgn::Plane(m3d::vec3(0.0f, 1.0f, 0.0f), 1.0f);
        dgn::Triangle tri1 = dgn::Triangle(m3d::vec3(0.0f, 1.5f, 2.5f), m3d::vec3(0.0f, 2.5f, 3.0f), m3d::vec3(0.0f, 1.5f, 3.5f));

        colliders.push_back(&box1);
        colliders.push_back(&sphere1);
        colliders.push_back(&plane1);
        colliders.push_back(&tri1);
        colliders.push_back(&test_collider);

        for(unsigned i = 0; i < colliders.size(); i++)
        {
            dgn::Shader::uniform(line_u_color, m3d::vec3(0.0f, 1.0f, 0.0f));

            for(unsigned j = i + 1; j < colliders.size(); j++)
            {
                if(colliders[i]->checkCollision(colliders[j]).hit)
                {
                    dgn::Shader::uniform(line_u_color, m3d::vec3(1.0f, 0.0f, 0.0f));
                }
            }

            switch(colliders[i]->getType())
            {
            case dgn::ColliderType::Box:
                drawLineBox(*(dgn::BoundingBox*)colliders[i], line_u_points, main_window.getRenderer());
                break;
            case dgn::ColliderType::Sphere:
                    drawLineSphere(*(dgn::BoundingSphere*)colliders[i], line_u_points, main_window.getRenderer());
                break;
            case dgn::ColliderType::Plane:
                    drawPlane(*(dgn::Plane*)colliders[i], line_u_points, main_window.getRenderer());
                break;
            case dgn::ColliderType::Triangle:
                    drawTriangle(*(dgn::Triangle*)colliders[i], line_u_points, main_window.getRenderer());
                break;
            default:
                break;
            }
        }

        dgn::Shader::uniform(line_u_color, m3d::vec3(1.0f, 1.0f, 0.0f));
        m3d::vec3 near_point = sphere1.nearestPoint(camera.position);
        drawPoint(near_point, line_u_points, main_window.getRenderer());

        near_point = box1.nearestPoint(camera.position);
        drawPoint(near_point, line_u_points, main_window.getRenderer());

        near_point = plane1.nearestPoint(camera.position);
        drawPoint(near_point, line_u_points, main_window.getRenderer());

        near_point = tri1.nearestPoint(camera.position);
        drawPoint(near_point, line_u_points, main_window.getRenderer());

        main_window.getRenderer().unbindShader();
        main_window.getRenderer().unbindMesh();

        main_window.getRenderer().setDrawMode(dgn::DrawMode::Triangles);

        //////////////////////////////////////////////
        //                RENDER SCREEN             //
        //////////////////////////////////////////////
        main_window.getRenderer().unbindFramebuffer();
        main_window.getRenderer().clear();
        main_window.getRenderer().setDepthTest(dgn::DepthTest::Always);

        main_window.getRenderer().bindShader(screen_shader);

        main_window.getRenderer().bindTexture(screen_texture, 0);
        //main_window.getRenderer().bindTexture(shadowmap.getTexture(), 0);
        main_window.getRenderer().bindMesh(screen_mesh);
        main_window.getRenderer().drawBoundMesh();

        //main_window.getRenderer().setViewport(0, WINDOW_HEIGHT - WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);

        //main_window.getRenderer().bindTexture(shadowmap.getTexture(), 0);
        //main_window.getRenderer().drawBoundMesh();*/

        main_window.swapBuffers();
    }

    for(dgn::Mesh& m : scene)
    {
        m.dispose();
    }

    shader.dispose();
    skybox.dispose();

    main_window.terminate();
}

bool cam_lock = false;

void updateCamera(dgn::Camera *camera, dgn::Window *window, float delta, bool controller)
{
    float camera_rot_speed = PI * delta;
    float camera_move_speed = 2.0f * delta;
    m3d::quat camera_rot_y;
    m3d::quat camera_rot_x;

    if(controller)
    {
        camera_rot_y = m3d::quat(
                        window->getInput().getGamepadAxis(0, dgn::GamepadAxis::RightX, 0.1f) * camera_rot_speed,
                        m3d::vec3(0.0f, -1.0f, 0.0f));
        camera_rot_x = m3d::quat(
                        window->getInput().getGamepadAxis(0, dgn::GamepadAxis::RightY, 0.1f) * camera_rot_speed,
                        m3d::vec3(-1.0f, 0.0f, 0.0f) * camera->rotation);

        camera->rotation = camera_rot_y * camera_rot_x * camera->rotation;

        m3d::vec3 cam_offset = m3d::vec3(window->getInput().getGamepadAxis(0, dgn::GamepadAxis::LeftX, 0.1f),
                                         0.0f,
                                         window->getInput().getGamepadAxis(0, dgn::GamepadAxis::LeftY, 0.1f));
        cam_offset *= camera_move_speed;
        cam_offset *= camera->rotation;

        camera->position += cam_offset;
    }
    else
    {
        camera_rot_speed = PI * 0.004f;

        // Mouse and keyboard
        if(window->getInput().getKeyDown(dgn::Key::Escape))
        {
            cam_lock = !cam_lock;

            if(cam_lock)
            {
                window->setRawCursorMode(true);
                window->setCursorMode(dgn::CursorMode::Disabled);
            }
            else
            {
                window->setRawCursorMode(false);
                window->setCursorMode(dgn::CursorMode::Normal);
            }
        }

        if(cam_lock)
        {
            camera_rot_y = m3d::quat(
                        window->getInput().getMouseXDelta() * camera_rot_speed,
                        m3d::vec3(0.0f, -1.0f, 0.0f));
            camera_rot_x = m3d::quat(
                        window->getInput().getMouseYDelta() * camera_rot_speed,
                        m3d::vec3(-1.0f, 0.0f, 0.0f) * camera->rotation);

            camera->rotation = camera_rot_y * camera_rot_x * camera->rotation;
        }

        float h = window->getInput().getKey(dgn::Key::D) ? 1.0 : 0.0;
        h -= window->getInput().getKey(dgn::Key::A) ? 1.0 : 0.0;

        float v = window->getInput().getKey(dgn::Key::S) ? 1.0 : 0.0;
        v -= window->getInput().getKey(dgn::Key::W) ? 1.0 : 0.0;

        m3d::vec3 cam_offset = m3d::vec3(h, 0.0f, v);
        cam_offset = cam_offset.normalized();
        cam_offset *= camera_move_speed;
        cam_offset *= camera->rotation;

        camera->position += cam_offset;
    }
}

void drawLineBox(const dgn::BoundingBox& box, int uniforms[], const dgn::Renderer& renderer)
{
    m3d::vec3 size = box.max - box.min;

    dgn::Shader::uniform(uniforms[0], box.max);
    dgn::Shader::uniform(uniforms[1], box.max - m3d::vec3(size.x, 0.0f, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.max);
    dgn::Shader::uniform(uniforms[1], box.max - m3d::vec3(0.0f, size.y, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.max);
    dgn::Shader::uniform(uniforms[1], box.max - m3d::vec3(0.0f, 0.0f, size.z));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min);
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(size.x, 0.0f, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min);
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(0.0f, size.y, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min);
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(0.0f, 0.0f, size.z));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(size.x, 0.0f, 0.0f));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(size.x, size.y, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(size.x, 0.0f, 0.0f));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(size.x, 0.0f, size.z));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(0.0f, size.y, 0.0f));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(0.0f, size.y, size.z));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(0.0f, size.y, 0.0f));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(size.x, size.y, 0.0f));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(0.0f, 0.0f, size.z));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(size.x, 0.0f, size.z));
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], box.min + m3d::vec3(0.0f, 0.0f, size.z));
    dgn::Shader::uniform(uniforms[1], box.min + m3d::vec3(0.0f, size.y, size.z));
    renderer.drawBoundMesh();
}

const float sphere_subdivisions = 16;
const float sphere_angle = 2.0f * PI / sphere_subdivisions;

void drawLineSphere(const dgn::BoundingSphere& sphere, int uniforms[], const dgn::Renderer& renderer)
{
    m3d::vec3 start_long = m3d::vec3(1.0f, 0.0f, 0.0f);
    m3d::vec3 end_long;

    m3d::vec3 start_lat_x = m3d::vec3(1.0f, 0.0f, 0.0f);
    m3d::vec3 end_lat_x;

    m3d::vec3 start_lat_z = m3d::vec3(0.0f, 0.0f, 1.0f);
    m3d::vec3 end_lat_z;

    float angle = 0.0f;
    for(int i = 0; i < sphere_subdivisions; i++)
    {
        angle += sphere_angle;
        float sin_theta = std::sin(angle);
        float cos_theta = std::cos(angle);

        end_long = m3d::vec3(cos_theta, 0.0f, sin_theta);

        dgn::Shader::uniform(uniforms[0], start_long * sphere.radius + sphere.position);
        dgn::Shader::uniform(uniforms[1], end_long * sphere.radius + sphere.position);
        renderer.drawBoundMesh();

        end_lat_x = m3d::vec3(cos_theta, sin_theta, 0.0f);

        dgn::Shader::uniform(uniforms[0], start_lat_x * sphere.radius + sphere.position);
        dgn::Shader::uniform(uniforms[1], end_lat_x * sphere.radius + sphere.position);
        renderer.drawBoundMesh();

        end_lat_z = m3d::vec3(0.0f, sin_theta, cos_theta);
        dgn::Shader::uniform(uniforms[0], start_lat_z * sphere.radius + sphere.position);
        dgn::Shader::uniform(uniforms[1], end_lat_z * sphere.radius + sphere.position);
        renderer.drawBoundMesh();

        start_long = end_long;
        start_lat_x = end_lat_x;
        start_lat_z = end_lat_z;
    }
}

const float planeHWidth = 1.5;
const float planeHeight = 1.0;

void drawPlane(const dgn::Plane& plane, int uniforms[], const dgn::Renderer& renderer)
{
    m3d::vec3 center = plane.normal * plane.distance;

    m3d::vec3 tan;
    m3d::vec3 bitan;

    if(std::abs(m3d::vec3::dot(plane.normal, m3d::vec3(0.0f, 1.0f, 0.0f)) == 1))
    {
        tan = m3d::vec3(1.0f, 0.0f, 0.0f);
        bitan = m3d::vec3(0.0f, 0.0f, 1.0f);
    }
    else
    {
        tan = m3d::vec3::cross(plane.normal, m3d::vec3(0.0f, 1.0f, 0.0f)).normalized();
        bitan = m3d::vec3::cross(plane.normal, tan).normalized();
    }

    dgn::Shader::uniform(uniforms[0], center);
    dgn::Shader::uniform(uniforms[1], center + plane.normal * planeHeight);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], center + (tan + bitan) * planeHWidth);
    dgn::Shader::uniform(uniforms[1], center + (tan - bitan) * planeHWidth);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], center + (tan + bitan) * planeHWidth);
    dgn::Shader::uniform(uniforms[1], center + (-tan + bitan) * planeHWidth);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], center + (-tan - bitan) * planeHWidth);
    dgn::Shader::uniform(uniforms[1], center + (tan - bitan) * planeHWidth);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], center + (-tan - bitan) * planeHWidth);
    dgn::Shader::uniform(uniforms[1], center + (-tan + bitan) * planeHWidth);
    renderer.drawBoundMesh();

}

const int point_num_points = 20;
const float point_line_length = 0.1f;
const float gr =(std::sqrt(5.0f) + 1.0f) / 2.0f;  // golden ratio = 1.6180339887498948482
const float ga =(2.0f - gr) * (2.0f * PI);  // golden angle = 2.39996322972865332

void drawPoint(const m3d::vec3& point, int uniforms[], const dgn::Renderer& renderer)
{
    for(int i = 0; i < point_num_points; i++)
    {
        float lat = std::asin(-1.0f + 2.0f * float(i) / (point_num_points + 1));
        float lon = ga * i;

        float x = std::cos(lon)* std::cos(lat);
        float y = std::sin(lon)* std::cos(lat);
        float z = std::sin(lat);

        dgn::Shader::uniform(uniforms[0], point);
        dgn::Shader::uniform(uniforms[1], point + m3d::vec3(x, y, z) * point_line_length);
        renderer.drawBoundMesh();
    }
}

void drawTriangle(const dgn::Triangle& tri, int uniforms[], const dgn::Renderer& renderer)
{
    dgn::Shader::uniform(uniforms[0], tri.p1);
    dgn::Shader::uniform(uniforms[1], tri.p2);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], tri.p2);
    dgn::Shader::uniform(uniforms[1], tri.p3);
    renderer.drawBoundMesh();

    dgn::Shader::uniform(uniforms[0], tri.p3);
    dgn::Shader::uniform(uniforms[1], tri.p1);
    renderer.drawBoundMesh();
}

//TODO: 3d textures

//TODO: make first dll and start demo game
