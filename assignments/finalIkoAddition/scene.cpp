#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

// batteries
#include "batteries/opengl.h"
#include "batteries/lights.h"

constexpr int kFramebufferWidth = 800;
constexpr int kFramebufferHeight = 600;

struct {
    int indexEffect = 0;

    glm::vec3 modelPos = glm::vec3(0.0);

    glm::vec3 palette1 = glm::vec3(1.0);
    glm::vec3 palette2 = {0.3, 0.3, 0.3};
    glm::vec3 floor = {1.0, 0.0, 0.0};
    float lerpFactor = 0.0f;

    glm::vec3 backgroundColor = {0.2, 0.3, 0.3};

    float fogHeight = 1.0;
    float fogRange = 0.5;
    bool fogtoggle = false;
    float fogR = 0.5;
    float fogG = 0.5;
    float fogB = 0.5;

} debug;

// Palettes, think of a better way to do this later
struct {
    glm::vec3 floor = {0.53, 0.35, 0.14};
    glm::vec3 accent1 = {0.89, 0.89, 0.55};
    glm::vec3 accent2 = {0.91, 0.65, 0.33};
} dayPalette;

struct {
    glm::vec3 floor = {0.82, 0.28, 0.28};
    glm::vec3 accent1 = {0.49, 0.29, 0.41};
    glm::vec3 accent2 = {0.16, 0.16, 0.4};
} nightPalette;

struct {
    glm::vec3 floor = {0.68, 0.91, 0.96};
    glm::vec3 accent1 = {0.28, 0.79, 0.89};
    glm::vec3 accent2 = {0.01, 0.24, 0.54};
} oceanPalette;

struct {
    glm::vec3 floor = {0.64, 0.69, 0.54};
    glm::vec3 accent1 = {0.35, 0.51, 0.34};
    glm::vec3 accent2 = {0.20, 0.31, 0.25};
} earthPalette;

// To transition between values!
glm::vec3 Scene::lerp(glm::vec3 a, glm::vec3 b, float t) {
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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
        printf("It's not complete for depth :(\n");
    }

    // Has to unbind or else it will create a black screen
    // All functions will be operating on Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::createHeightBuffer() {
    glCreateFramebuffers(1, &elevation_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, elevation_fbo);

    { // Create Depth
        glGenTextures(1, &elevation_texture);
        glBindTexture(GL_TEXTURE_2D,elevation_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,elevation_texture,0);

        glGenTextures(1, &elevation_depth);
        glBindTexture(GL_TEXTURE_2D, elevation_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, elevation_depth, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
        printf("It's not complete for height :(\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::createIsolationBuffer() {
    glCreateFramebuffers(1, &isolation_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, isolation_fbo);

    { // Create Depth

        glGenTextures(1, &isolation_texture);
        glBindTexture(GL_TEXTURE_2D,isolation_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,isolation_texture,0);

        glGenTextures(1, &isolation_depth);
        glBindTexture(GL_TEXTURE_2D, isolation_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, isolation_depth, 0);

    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
        printf("It's not complete for height :(\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


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

// Assigns effect, currently is the Vignette
void Scene::assignEffect(ew::Shader* shader) {

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDisable(GL_DEPTH_TEST);

    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); Does nothing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->setVec3("backcolor", debug.backgroundColor);

    // no fullscreen
    glBindVertexArray(fullQuad.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

Scene::Scene()
{
    monument = std::make_unique<ew::Model>("assets/models/MonumentValley-compressed.obj");
    ambient = std::make_unique<ew::Shader>("assets/shaders/cocoa/ambient.vs", "assets/shaders/cocoa/ambient.fs");
    elevation = std::make_unique<ew::Shader>("assets/shaders/IkoFinalShaders/elevation.vs", "assets/shaders/IkoFinalShaders/elevation.fs");
    elevationFade = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/IkoFinalShaders/mistfade.fs");
    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");

    // Vignette! This can be changed
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/cocoa/vignette.vs", "assets/shaders/cocoa/vignette.fs"));

    light = {
        .color = {1.0f, 1.0f, 1.0f},
        .position = {5.0f, 5.0f, -8.0f},  
    };

    fullQuad.Init();

    createFrameBuffer();
    createHeightBuffer();
    createIsolationBuffer();
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
    
    // Vignette, I'm too lazy to change from the previous system
    // But let me know if it's a problem and I'll fix it
    assignEffect(postProcessingEffects[debug.indexEffect].get());

    // Actual Beginning of Pipeline
    auto newModelMatrix = 
        glm::translate(glm::mat4(1.0f), debug.modelPos)
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

    const auto light_proj = glm::ortho(-10.0f, +10.0f, -10.0f, +10.0f, 0.1f, 100.0f);
    const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    const auto light_view_proj = light_proj * light_view;

    // Model Pipeline
    const auto view_proj = camera.Projection() * camera.View();

    // Will Clear Vignette
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // See through vignette
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, shadow_depth);

    ambient->use();

    // scene matrices
    ambient->setMat4("view_proj", view_proj);
    ambient->setVec3("camera_position", camera.position);
    ambient->setMat4("light_view_proj", light_view_proj);

    // Set uniforms
    ambient->setVec3("camera", camera.position);
    ambient->setVec3("light.position", light.position);
    ambient->setVec3("light.color", light.color);
    ambient->setVec3("floorColor", debug.floor);

    ambient->setVec3("pal.lit", debug.palette1);
    ambient->setVec3("pal.unlit", debug.palette2);

    // Scaling down the giant model!
    auto scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)); 
    ambient->setMat4("model", newModelMatrix);
    monument->draw();

    // Cleaning up, just in case
    glDisable(GL_BLEND);

    // Blitzing the vignette with the model
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, kFramebufferWidth, kFramebufferHeight, 0, 0, kFramebufferWidth, kFramebufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(debug.fogtoggle)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, elevation_fbo);
    {
        const auto view_proj = camera.Projection() * camera.View();

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        elevation->use();

        elevation->setMat4("model", newModelMatrix);
        elevation->setMat4("view_proj", view_proj);

        monument->draw();
        //const auto plane_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
        //elevation->setMat4("model", plane_matrix);
        //plane.draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, elevation_texture);
    {
        const auto view_proj = camera.Projection() * camera.View();
        // local scope
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        elevation->use();

        // scene matrices
        elevation->setFloat("fog_range", debug.fogRange);
        elevation->setFloat("fog_height", debug.fogHeight);
        elevation->setMat4("model", newModelMatrix);
        elevation->setMat4("view_proj", view_proj);

        monument->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, isolation_texture);
    {
        glClearColor(debug.fogR,debug.fogG,debug.fogB,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        glBindVertexArray(fullQuad.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, elevation_texture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        elevationFade->use();
        elevationFade->setInt("screen", 0);
        elevationFade->setInt("mist_effect", 1);
        elevationFade->setInt("background", 2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    

    //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fullQuad.vao);
    //glBlitFramebuffer(0, 0, kFramebufferWidth, kFramebufferHeight, 0, 0, kFramebufferWidth, kFramebufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Scene::Debug(void)
{
    const auto view = camera.View();
    const auto proj = camera.Projection();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    //ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], glm::value_ptr(identity), 10.0f);

    ImGuizmo::SetID(1);
    auto light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(light_matrix));

    if (ImGuizmo::IsUsing()){
        light.position = glm::vec3(light_matrix[3]);
    }

    ImGuizmo::SetID(2);
    auto suzanne_matrix = glm::translate(glm::mat4(1.0f), debug.modelPos);
    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(suzanne_matrix));

    if (ImGuizmo::IsUsing()){
        debug.modelPos = glm::vec3(suzanne_matrix[3]);
    }    

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    //ImGui::Checkbox("Paused", &time.paused);
    //ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */
    ImGui::SeparatorText("Ambient");
    ImGui::ColorEdit3("BackGround Color", &debug.backgroundColor[0]);
    ImGui::ColorEdit3("Floor Color", &debug.floor[0]);
    ImGui::ColorEdit3("Accent 1", &debug.palette1[0]);
    ImGui::ColorEdit3("Accent 2", &debug.palette2[0]);

    ImGui::SeparatorText("Presets");
    // Day Night
    if (ImGui::Button("Day")) {
        debug.floor = dayPalette.floor;
        debug.palette1 = dayPalette.accent1;
        debug.palette2 = dayPalette.accent2;
    }
    ImGui::SameLine();
    if (ImGui::Button("Night")) {
        debug.floor = nightPalette.floor;
        debug.palette1 = nightPalette.accent1;
        debug.palette2 = nightPalette.accent2;
    }
    ImGui::SameLine();
    if (ImGui::Button("Ocean")) {
        debug.floor = oceanPalette.floor;
        debug.palette1 = oceanPalette.accent1;
        debug.palette2 = oceanPalette.accent2;
    }
    ImGui::SameLine();
    if (ImGui::Button("Earth")) {
        debug.floor = earthPalette.floor;
        debug.palette1 = earthPalette.accent1;
        debug.palette2 = earthPalette.accent2;
    }

    if (ImGui::SliderFloat("Day/Night Slider", &debug.lerpFactor, 0.0f, 1.0f)) {
        debug.floor = lerp(dayPalette.floor, nightPalette.floor, debug.lerpFactor);
        debug.palette1 = lerp(dayPalette.accent1, nightPalette.accent1, debug.lerpFactor);
        debug.palette2 = lerp(dayPalette.accent2, nightPalette.accent2, debug.lerpFactor);
    }
    if (ImGui::CollapsingHeader("Fog"))
    {
        ImVec2 uv_min(0.0f, 1.0f);
        ImVec2 uv_max(1.0f, 0.0f);

        debug.fogtoggle = true;
        ImGui::SliderFloat("Fog Height", &debug.fogHeight, -15.0f, 8.0f);
        ImGui::SliderFloat("Fog Fade Range", &debug.fogRange, 0.1f, 3.0f);
        ImGui::SliderFloat("Fog Red", &debug.fogR, 0.0f, 1.0f);
        ImGui::SliderFloat("Fog Green", &debug.fogG, 0.0f, 1.0f);
        ImGui::SliderFloat("Fog Blue", &debug.fogB, 0.0f, 1.0f);

        ImGui::Text("Elevation:");
        ImGui::Image((ImTextureID)(intptr_t) elevation_texture, ImVec2(200, 150), uv_min, uv_max);

        //ImGui::Text("Isolation:");
        //ImGui::Image((ImTextureID)(intptr_t) isolation_texture, ImVec2(200, 150), uv_min, uv_max);
    }
    else{
        debug.fogtoggle = false;
    }
    // Anything else that's cute?
    
    if (ImGui::CollapsingHeader("Framebuffer Images")) {
        ImGui::Image((void*)(intptr_t)fbo_texture, ImVec2(400, 300), ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
}