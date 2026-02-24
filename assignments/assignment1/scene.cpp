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
    int textureChoice = 0;

} debug;

struct {
    float kernelStrength = 10.0f;
    float gamma = 2.2f;
    float distortion = 0.75f;
    float filmStrength = 1.0f;
    float watercolorStrength = 50.0f;

} ppDebug;

glm::mat4 identity(1.0f);

typedef struct // making new type of struct instead of unordered map
{
    const std::string name;
    const batteries::material_t material;
} mats;

static std::vector<mats> matList = {
    {"default", {{0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, 1.0f}},

    // http://devernay.free.fr/cours/opengl/materials.html
    // Since my file reader imploded we will just do this the hideous way
    {"emerald", {{0.0215f, 0.1745f, 0.0215f}, {0.07568f, 0.61424f, 0.07568f}, {0.633f, 0.727811f, 0.633f}, 0.6f}},
    {"jade", {{0.135f, 0.2225f, 0.1575f}, {0.54f, 0.89f, 0.63f}, {0.316228f, 0.316228f, 0.316228f}, 0.1}},
    {"obsidian", {{0.05375f, 0.05f, 0.06625f}, {0.18275f, 0.17f, 0.22525f}, {0.332741f, 0.328634f, 0.346435f}, 0.3f}},
    {"pearl", {{0.25f, 0.20725f, 0.20725f}, {1.0f, 0.829f, 0.829f}, {0.296648f, 0.296648f, 0.296648f}, 0.088f}},
    {"ruby", {{0.1745f, 0.01175f, 0.01175f}, {0.61424f, 0.04136f, 0.04136f}, {0.727811f, 0.626959f, 0.626959f}, 0.6f}},
    {"turquoise", {{0.1f, 0.18725f, 0.1745f}, {0.396f, 0.74151f, 0.69102f}, {0.297254f, 0.30829f, 0.306678f}, 0.1f}},
    {"brass", {{0.329412f, 0.223529f, 0.027451f}, {0.780392f, 0.568627f, 0.113725f}, {0.992157f, 0.941176f, 0.807843f}, 0.21794872f}},
    {"bronze", {{0.2125f, 0.1275f, 0.054f}, {0.714f, 0.4284f, 0.18144f}, {0.393548f, 0.271906f, 0.166721f}, 0.2f}},
    {"chrome", {{0.25f, 0.25f, 0.25f}, {0.4f, 0.4f, 0.4f}, {0.774597f, 0.774597f, 0.774597f}, 0.6f}},
    {"copper", {{0.19125f, 0.0735f, 0.0225f}, {0.7038f, 0.27048f, 0.0828f}, {0.256777f, 0.137622f, 0.086014f}, 0.1f}},
    {"gold", {{0.24725f, 0.1995f, 0.0745f}, {0.75164f, 0.60648f, 0.22648f}, {0.628281f, 0.555802f, 0.366065f}, 0.4f}},
    {"silver", {{0.19225f, 0.19225f, 0.19225f}, {0.50754f, 0.50754f, 0.50754f}, {0.508273f, 0.508273f, 0.508273f}, 0.4f}},
    {"black plastic", {{0.0f, 0.0f, 0.0f}, {0.01f, 0.01f, 0.01f}, {0.50f, 0.50f, 0.50f}, 0.25f}},
    {"cyan plastic", {{0.0f, 0.1f, 0.06f}, {0.0f, 0.50980392f, 0.50980392f}, {0.50196078f, 0.50196078f, 0.50196078f}, 0.25f}},
    {"green plastic", {{0.0f, 0.0f, 0.0f}, {0.1f, 0.35f, 0.1f}, {0.45f, 0.55f, 0.45f}, 0.25f}},
    {"red plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.7f, 0.6f, 0.6f}, 0.25f}},
    {"white plastic", {{0.0f, 0.0f, 0.0f}, {0.55f, 0.55f, 0.55f}, {0.70f, 0.70f, 0.70f}, 0.25f}},
    {"yellow plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.60f, 0.60f, 0.50f}, 0.25f}},
    {"black rubber", {{0.02f, 0.02f, 0.02f}, {0.01f, 0.01f, 0.01f}, {0.4f, 0.4f, 0.4f}, 0.078125f}},
    {"cyan rubber", {{0.0f, 0.05f, 0.05f}, {0.4f, 0.5f, 0.5f}, {0.04f, 0.7f, 0.7f}, 0.078125f}},
    {"green rubber", {{0.0f, 0.05f, 0.0f}, {0.4f, 0.5f, 0.4f}, {0.04f, 0.7f, 0.04f}, 0.078125f}},
    {"red rubber", {{0.05f, 0.0f, 0.0f}, {0.5f, 0.4f, 0.4f}, {0.7f, 0.04f, 0.04f}, 0.078125f}},
    {"white rubber", {{0.05f, 0.05f, 0.05f}, {0.5f, 0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, 0.078125f}},
    {"yellow rubber", {{0.05f, 0.05f, 0.0f}, {0.5f, 0.5f, 0.4f}, {0.7f, 0.7f, 0.04f}, 0.078125f}},
};

enum EFFECT_NAMES {
    NONE = 0,
    BLUR = 1,
    GREYSCALE = 2,
    EDGE = 3,
    SHARPEN = 4,
    GAMMA = 5,
    CHROMATIC = 6,
    FILM = 7,
    RED_GREEN = 8,
    BLUE_YELLOW = 9,
    WATERCOLOR = 10,
} effectType;

static std::vector<std::string> processingNames = {
    "None",
    "Blur",
    "Greyscale",
    "Edge Detection",
    "Sharpen",
    "Gamma Correction",
    "Chromatic Abberation",
    "Film Grain",
    "Red-Green Colorblindness",
    "Blue-Yellow Colorblindness",
    "Watercolor",
};

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

void assignEffect(ew::Shader* shader) {
    shader->use();
    shader->setInt("screen", 0);

    switch(debug.indexEffect) {
        case BLUR:
            shader->setFloat("strength", ppDebug.kernelStrength);
            break;
        case GREYSCALE:
            break;
        case EDGE:
            break;
        case SHARPEN:
            break;
        case GAMMA:
            shader->setFloat("gamma", ppDebug.gamma);
            break;
        case CHROMATIC:
            shader->setFloat("distortion", ppDebug.distortion);
            break;
        case FILM:
            shader->setFloat("strength", ppDebug.filmStrength);
            break;
        case RED_GREEN:
            break;
        case BLUE_YELLOW:
            break;
        case WATERCOLOR:
            shader->setFloat("strength", ppDebug.watercolorStrength);
            break;
        default:
            break;
    }

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw fullscreen
    glBindVertexArray(fullQuad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuff.framefbo_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");
    colorblind = std::make_unique<ew::Texture>("assets/textures/colorblind.png");
    ornament = std::make_unique<ew::Texture>("assets/textures/CTO_Color.jpg");
    leaf = std::make_unique<ew::Texture>("assets/textures/leaf.png");

    postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/blur.fs");

    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/fullscreen.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/blur.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/greyscale.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/edgeDetect.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/sharpen.fs"));  
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/gamma.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/chromatic.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/filmgrain.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/redgreen.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/tritanopia.fs"));
    postProcessingEffects.push_back(std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocessing/watercolor.fs"));

    light = {
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},  
    };

    fullQuad.Init();

    framebuff.init();
}

Scene::~Scene()
{
    glDeleteBuffers(1, &fbo);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuff.framefbo);

    // Suzanne Pipeline
    {
        const auto view_proj = camera.Projection() * camera.View();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorblind->getID());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ornament->getID());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, leaf->getID());

        blinnphong->use();

        // scene matrices
        blinnphong->setMat4("model", glm::mat4(1.0f));
        blinnphong->setMat4("view_proj", view_proj);
        blinnphong->setVec3("camera_position", camera.position);

        // Set uniforms
        blinnphong->setVec3("camera", camera.position);
        blinnphong->setVec3("light.position", light.position);
        blinnphong->setVec3("light.color", light.color);
        blinnphong->setFloat("alpha", debug.alpha);
        blinnphong->setVec3("ambientColor", debug.ambient);

        // Updating uniforms for lighting
        auto material = matList[debug.selectedIndex].material;
        blinnphong->setVec3("material.ambient", material.ambient);
        blinnphong->setVec3("material.diffuse", material.diffuse);
        blinnphong->setVec3("material.specular", material.specular);
        blinnphong->setFloat("material.shininess", material.shininess);

        // Texture test
        blinnphong->setInt("mainTexture", debug.textureChoice);

        // draw suzanne
        suzanne->draw();
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

    auto light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(light_matrix));

    if (ImGuizmo::IsUsing()){
        light.position = glm::vec3(light_matrix[3]);
    }

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */
    ImGui::SeparatorText("Material/Ambient");
    ImGui::SliderInt("Texture Choice", &debug.textureChoice, 0, 2);
    ImGui::SliderFloat("Intensity", &debug.alpha, 0.0f, 1.0f);
    ImGui::ColorEdit3("Color", &light.color[0]);

    ImGui::SeparatorText("Material");
    if (ImGui::BeginCombo("Presets", matList[debug.selectedIndex].name.c_str())) {
        for (int i = 0; i < matList.size(); i++) {
            const bool isSelected = (matList[debug.selectedIndex].name == matList[i].name);
            if (ImGui::Selectable(matList[i].name.c_str(), isSelected)) {
                debug.selectedIndex = i;
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }

        }
        ImGui::EndCombo();
    }
    auto material = matList[debug.selectedIndex].material; // Updating the values
    if (ImGui::CollapsingHeader("Material Details")) {
        ImGui::SliderFloat3("Ambient", &material.ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse", &material.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular", &material.specular[0], 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &material.shininess, 2.0f, 128.0f);
    }

    ImGui::SeparatorText("Post Processing Effects");
    if (ImGui::BeginCombo("Effect", processingNames[debug.indexEffect].c_str())) {
        for (int i = 0; i < processingNames.size(); i++) {
            const bool isSelected = (processingNames[debug.indexEffect] == processingNames[i]);
            if (ImGui::Selectable(processingNames[i].c_str(), isSelected)) {
                debug.indexEffect = i;
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    if (debug.indexEffect == BLUR) {
        ImGui::SliderFloat("Kernel Strength", &ppDebug.kernelStrength, 0.0f, 300.0f);
    }
    if (debug.indexEffect == GAMMA) {
        ImGui::SliderFloat("Gamma", &ppDebug.gamma, 0.0f, 10.0f);
    }
    if (debug.indexEffect == CHROMATIC) {
        ImGui::SliderFloat("Distortion", &ppDebug.distortion, 0.0f, 1.0f);
    }
    if (debug.indexEffect == FILM) {
        ImGui::SliderFloat("Strength", &ppDebug.filmStrength, 0.0f, 1.0f);
    }
    if (debug.indexEffect == WATERCOLOR) {
        ImGui::SliderFloat("Wiggle", &ppDebug.watercolorStrength, 0.0f, 60.0f);
    }
  
    if (ImGui::CollapsingHeader("Framebuffer Images")) {
        ImGui::Image((void*)(intptr_t)framebuff.framefbo_texture, ImVec2(400, 300), ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
}