#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

// batteries
#include "batteries/opengl.h"
#include "batteries/lights.h"

//#include "cocoa/materialsReader.h"

struct {
    float alpha = 1.0f;
    glm::vec3 ambient = {0.3, 0.3, 0.3};
    int selectedIndex = 0;

} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");

    light = {
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},  
    };

    material = {
        .ambient = {0.5, 0.5, 0.5},
        .diffuse = {0.7, 0.7, 0.7},
        .specular = {0.3, 0.3, 0.3},
        .shininess = {2},

    };

    //MaterialsReader reader;

    //matList = reader.createMatList("assets/txt/materials.txt");
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

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
    blinnphong->setVec3("material.ambient", material.ambient);
    blinnphong->setVec3("material.diffuse", material.diffuse);
    blinnphong->setVec3("material.specular", material.specular);
    blinnphong->setFloat("material.shininess", material.shininess);

    // draw suzanne
    suzanne->draw();
}

glm::mat4 identity(1.0f);

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
    ImGui::SeparatorText("Ambient");
    ImGui::SliderFloat("Intensity", &debug.alpha, 0.0f, 1.0f);
    ImGui::ColorEdit3("Color", &light.color[0]);

    ImGui::SeparatorText("Material");
    /*if (ImGui::BeginCombo("Materials", "Presets")) {
        for (const auto& i : matList) {
            std::string start = i.first;
            batteries::material_t end = i.second;
            const char* startChar = start.c_str();
            const bool isSelected = (debug.selectedIndex == start);
            if (ImGui::Selectable(startChar, isSelected)) {
                debug.selectedIndex = start;
            }

            // Set the initial focus when opening the combo
            // (scrolling + keyboard navigation focus)
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }

        }
        ImGui::EndCombo();
    } */
    ImGui::SliderFloat3("Ambient", &material.ambient[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &material.diffuse[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", &material.specular[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &material.shininess, 2.0f, 128.0f);

    ImGui::End();
}