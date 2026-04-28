#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/opengl.h" //Has to be include in everything Scene is

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"
#include "ew/mesh.h"
#include "ew/procGen.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Model> monument;
    std::unique_ptr<ew::Shader> ambient;
    std::unique_ptr<ew::Shader> elevation;
    std::unique_ptr<ew::Shader> elevationFade;

    std::unique_ptr<ew::Shader> depth;

    // Post process
    std::vector<std::unique_ptr<ew::Shader>> postProcessingEffects;

    batteries::light_t light;
    batteries::material_t material;

    GLuint fbo; // frame buffer object
    GLuint fbo_texture;
    GLuint fbo_depth;

    GLuint elevation_fbo;
    GLuint elevation_texture;
    GLuint elevation_depth;
    GLuint isolation_fbo;
    GLuint isolation_texture;
    GLuint isolation_depth;

    // Depth
    GLuint shadow_fbo; // frame buffer object
    GLuint shadow_depth;

    void createFrameBuffer();
    void createHeightBuffer();
    void createDepthBuffer();
    void createIsolationBuffer();
    void assignEffect(ew::Shader* shader);
    glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
};
