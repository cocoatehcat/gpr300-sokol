#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

// batteries
#include "batteries/opengl.h"
#include "batteries/lights.h"

struct {
    float alpha = 1.0f;
    glm::vec3 ambient = {0.3, 0.3, 0.3};
    int selectedIndex = 0;
    int indexEffect = 0;
    float strength = 10.0f;
    int textureChoice = 0;
    float biasMax = 0.005f;

    glm::vec3 suzannePos = glm::vec3(1.0);

    glm::vec3 palette1 = glm::vec3(1.0);
    glm::vec3 palette2 = {0.3, 0.3, 0.3};
    glm::vec3 floor = {1.0, 0.0, 0.0};
    float lerpFactor = 0.0f;

} debug;

// Palettes, think of a better way to do this
struct {
    glm::vec3 floor = {1.0, 0.0, 0.0};
    glm::vec3 accent1 = {0.89, 0.89, 0.55};
    glm::vec3 accent2 = {0.91, 0.65, 0.33};
} dayPalette;

struct {
    glm::vec3 floor = {1.0, 0.0, 0.0};
    glm::vec3 accent1 = {0.49, 0.29, 0.41};
    glm::vec3 accent2 = {0.16, 0.16, 0.4};
} nightPalette;

// To transition between values!
glm::vec3 Scene::lerp(glm::vec3 a, glm::vec3 b, float t) {
    //debug.floor = (1 - debug.lerpFactor) * dayPalette.floor + debug.lerpFactor * nightPalette.floor;
    return (1 - t) * a + t * b;
}

glm::mat4 identity(1.0f);

struct {
    GLuint fbo;
    GLuint depth;

    void init() {
        glCreateFramebuffers(1, &fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        { // Create Depth
            glGenTextures(1, &depth);
            glBindTexture(GL_TEXTURE_2D, depth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 800, 600, 0, GL_DEPTH, GL_UNSIGNED_SHORT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

            // clean up
            //glBindTexture(GL_TEXTURE_2D, 0);
            glDrawBuffers(0, nullptr);
            glReadBuffer(GL_NONE);
        }
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, shadow_depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
            printf("It's not complete for depth :(\n");
        }

        // Has to unbind or else it will create a black screen
        // All functions will be operating on Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} shadowBuffer;

void Scene::createDepthBuffer() {
    glCreateFramebuffers(1, &shadow_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

    { // Create Depth
        glGenTextures(1, &shadow_depth);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth, 0);

        // clean up
        //glBindTexture(GL_TEXTURE_2D, 0);
        glDrawBuffers(0, nullptr);
        glReadBuffer(GL_NONE);
    }
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, shadow_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
        printf("It's not complete for depth :(\n");
    }

    // Has to unbind or else it will create a black screen
    // All functions will be operating on Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

struct Framebuffer {

    GLuint framefbo; // frame buffer object
    GLuint framefbo_texture;
    GLuint framefbo_depth;
    
    void init() {
        glCreateFramebuffers(1, &framefbo);

        glBindFramebuffer(GL_FRAMEBUFFER, framefbo);

        { // Create Texture
            glGenTextures(1, &framefbo_texture);
            glBindTexture(GL_TEXTURE_2D, framefbo_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framefbo_texture, 0);

            glGenTextures(1, &framefbo_depth);
            glBindTexture(GL_TEXTURE_2D, framefbo_depth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framefbo_depth, 0);

            // clean up
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
            printf("It's not complete :(\n");
        }

        // Has to unbind or else it will create a black screen
        // All functions will be operating on Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} framebuff;

void Scene::createFrameBuffer() {
    glCreateFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    { // Create Texture
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

        glGenTextures(1, &fbo_depth);
        glBindTexture(GL_TEXTURE_2D, fbo_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);

        // clean up
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
        printf("It's not complete :(\n");
    }

    // Has to unbind or else it will create a black screen
    // All functions will be operating on Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
    

struct fullscreenQuad
{
    GLuint vao;
    GLuint vbo;

    void Init() {
        float vertices[] = {
            // pos (x, y), texcoord (u, v)
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        // Always last
        glBindVertexArray(0);
    }
} fullQuad;

void Scene::assignEffect(ew::Shader* shader) {
    shader->use();
    shader->setInt("screen", 0);

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw fullscreen
    glBindVertexArray(fullQuad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    monument = std::make_unique<ew::Model>("assets/models/MonumentValley-compressed.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/cocoa/ambient.vs", "assets/shaders/cocoa/ambient.fs");

    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");

    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/cocoa/vignette.vs", "assets/shaders/cocoa/vignette.fs"));

    light = {
        .color = {1.0f, 1.0f, 1.0f},
        .position = {5.0f, 5.0f, -5.0f},  
    };

    fullQuad.Init();

    createFrameBuffer();
    //createDepthBuffer();
}

Scene::~Scene()
{
    glDeleteBuffers(1, &fbo);
    glDeleteBuffers(1, &shadow_fbo);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glm::mat4 transMatrix = glm::translate(
        glm::mat4( 1.0f ),
        glm::vec3( 0.0f, 0.0f, 0.0f )
        );

    auto scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)); // Get rid of this and replace with Suzanne!
    auto suzanne_matrix = glm::translate(glm::mat4(1.0f), debug.suzannePos);

    auto newSuzanneMatrix = 
        glm::translate(glm::mat4(1.0f), debug.suzannePos)
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

    const auto light_proj = glm::ortho(-10.0f, +10.0f, -10.0f, +10.0f, 0.1f, 100.0f);
    const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    const auto light_view_proj = light_proj * light_view;

    // Suzanne Pipeline
    {
        const auto view_proj = camera.Projection() * camera.View();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);

        blinnphong->use();

        // scene matrices
        blinnphong->setMat4("model", suzanne_matrix);
        blinnphong->setMat4("view_proj", view_proj);
        blinnphong->setVec3("camera_position", camera.position);
        blinnphong->setMat4("light_view_proj", light_view_proj);

        // Set uniforms
        blinnphong->setVec3("camera", camera.position);
        blinnphong->setVec3("light.position", light.position);
        blinnphong->setVec3("light.color", light.color);
        blinnphong->setVec3("floorColor", debug.floor);

        blinnphong->setVec3("pal.lit", debug.palette1);
        blinnphong->setVec3("pal.unlit", debug.palette2);

        // draw suzanne
        // rip Suzanne </3
        //suzanne->draw();

        auto scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)); // Get rid of this and replace with Suzanne!
        blinnphong->setMat4("model", newSuzanneMatrix);
        monument->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    assignEffect(postProcessingEffects[debug.indexEffect].get());
}

void Scene::Debug(void)
{
    const auto view = camera.View();
    const auto proj = camera.Projection();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], glm::value_ptr(identity), 10.0f);

    ImGuizmo::SetID(1);
    auto light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(light_matrix));

    if (ImGuizmo::IsUsing()){
        light.position = glm::vec3(light_matrix[3]);
    }

    ImGuizmo::SetID(2);
    auto suzanne_matrix = glm::translate(glm::mat4(1.0f), debug.suzannePos);
    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(suzanne_matrix));

    if (ImGuizmo::IsUsing()){
        debug.suzannePos = glm::vec3(suzanne_matrix[3]);
    }    

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    //ImGui::Checkbox("Paused", &time.paused);
    //ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */
    ImGui::SeparatorText("Ambient");
    ImGui::ColorEdit3("Floor Color", &debug.floor[0]);

    ImGui::ColorEdit3("Accent 1", &debug.palette1[0]);
    ImGui::ColorEdit3("Accent 2", &debug.palette2[0]);

    ImGui::SeparatorText("Presets");
    // Day Night
    if (ImGui::Button("Day")) {
        debug.palette1 = {0.89, 0.89, 0.55};
        debug.palette2 = {0.91, 0.65, 0.33};
    }
    if (ImGui::Button("Night")) {
        debug.palette1 = {0.49, 0.29, 0.41};
        debug.palette2 = {0.16, 0.16, 0.4};
    }

    if (ImGui::SliderFloat("Day/Night", &debug.lerpFactor, 0.0f, 1.0f)) {
        debug.floor = lerp(dayPalette.floor, nightPalette.floor, debug.lerpFactor);
        debug.palette1 = lerp(dayPalette.accent1, nightPalette.accent1, debug.lerpFactor);
        debug.palette2 = lerp(dayPalette.accent2, nightPalette.accent2, debug.lerpFactor);
    }
    // Anything else that's cute?
    
    if (ImGui::CollapsingHeader("Framebuffer Images")) {
        ImGui::Image((void*)(intptr_t)fbo_texture, ImVec2(400, 300), ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
}