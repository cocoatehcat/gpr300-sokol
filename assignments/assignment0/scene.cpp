#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

// batteries
#include "batteries/opengl.h"

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/default.fs");
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

    // draw suzanne
    suzanne->draw();
}

glm::mat4 identity(1.0f);
glm::mat4 thebourne_identity(1.0f);

void Scene::Debug(void)
{
    const auto view = camera.View();
    const auto proj = camera.Projection();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], glm::value_ptr(identity), 10.0f);

    ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::LOCAL, &thebourne_identity[0][0]);

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */

    ImGui::End();
}