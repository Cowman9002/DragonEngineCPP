//TODO: Collision

#include "src/Window.h"
#include "src/Camera.h"
#include "src/ShadowMap.h"

#include <stdio.h>
#include <math.h>

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
#define CASCADE_SPLIT_BLEND 0.5

void updateCamera(dgn::Camera *camera, dgn::Window *window, float delta, bool controller);

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
    screen_fb.addColorAttachment(screen_texture);
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
        float dist_log = SHADOW_NEAR * pow(SHADOW_FAR / SHADOW_NEAR, ioverm);

        cascade_distances[i] = m3d::lerp(dist_log, dist_uni, CASCADE_SPLIT_BLEND);
    }

    dgn::Shader shadow_shader;
    shadow_shader.loadFromFiles("res/shaders/shadow.vert", "", "");

    int shadow_u_light = shadow_shader.getUniformLocation("uLight");
    int shadow_u_model = shadow_shader.getUniformLocation("uModel");

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
    planks_texture[2]       = black_texture;
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

    dgn::Camera camera;
    camera.width = WINDOW_WIDTH;
    camera.height = WINDOW_HEIGHT;

    camera.position.y = 4.0f;

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

    while(!main_window.shouldClose())
    {
        main_window.getInput().pollEvents();

        if(main_window.getInput().getKeyDown(dgn::Key::R))
        {
            shader.loadFromFiles("res/shaders/pbr_lite.vert", "", "res/shaders/pbr_lite.frag");
            skin_shader.loadFromFiles("res/shaders/skin.vert", "", "res/shaders/skin.frag");
            screen_shader.loadFromFiles("res/shaders/screen.vert", "", "res/shaders/screen.frag");
            skin_lut.loadFromFile("res/textures/skin_lut.png", dgn::TextureWrap::ClampToEdge, dgn::TextureFilter::Bilinear, dgn::TextureStorage::SRGB);
        }


        for(int i = 0; i < SHADOW_CASCADES; i++)
        {
            shadowmap[i].updateProjectionMatFitted(camera, cascade_distances[i], cascade_distances[i+1], 10.0f, 1.0f / PI);
            shadowmap[i].updateViewMat(sun_dir);
        }

        updateCamera(&camera, &main_window, 1.0f / 60.0f, true);

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

        for(int i = 0; i < SHADOW_CASCADES; i++)
        {
            dgn::Shader::uniform(shader_u_light_mat[i], shadowmap[i].getLightMat());
            dgn::Shader::uniform(shader_u_shadowmap[i], 21 + i);
            main_window.getRenderer().bindTexture(shadowmap[i].getTexture(), 21 + i);

            m3d::vec4 v = m3d::vec4(0.0f, 0.0f, -cascade_distances[i + 1], 1.0f);
            float clip = (camera.getProjection() * v).z;
            dgn::Shader::uniform(shader_u_cascade_depths[i], clip);
        }

        main_window.getRenderer().bindCubemap(skybox, 20);

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

        dgn::Shader::uniform(skybox_u_vp, camera.getProjection() * camera.getView().toMat3x3().toMat4x4());
        dgn::Shader::uniform(skybox_u_texture, 20);
        dgn::Shader::uniform(skybox_u_sun_dir, sun_dir);

        main_window.getRenderer().drawBoundMesh();

        //////////////////////////////////////////////
        //                RENDER SCREEN             //
        //////////////////////////////////////////////
        main_window.getRenderer().unbindFramebuffer();
        main_window.getRenderer().clear();

        main_window.getRenderer().bindShader(screen_shader);

        main_window.getRenderer().bindTexture(screen_texture, 0);
        //main_window.getRenderer().bindTexture(shadowmap.getTexture(), 0);
        main_window.getRenderer().bindMesh(screen_mesh);
        main_window.getRenderer().drawBoundMesh();

        //main_window.getRenderer().setViewport(0, WINDOW_HEIGHT - WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);

        //main_window.getRenderer().bindTexture(shadowmap.getTexture(), 0);
        //main_window.getRenderer().drawBoundMesh();

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

//TODO: Shader headers and econst values
