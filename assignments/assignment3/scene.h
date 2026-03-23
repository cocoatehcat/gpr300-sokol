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

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);
    void CacheInstance();

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> blinnphong;

    // Post process
    std::unique_ptr<ew::Shader> postProcess;
    std::vector<std::unique_ptr<ew::Shader>> postProcessingEffects;

    std::unique_ptr<ew::Texture> colorblind;
    std::unique_ptr<ew::Texture> ornament;
    std::unique_ptr<ew::Texture> leaf;

    batteries::light_t light;
    batteries::material_t material;

    std::vector<glm::mat4> instanceData;

    GLuint fbo; // frame buffer object
    GLuint fbo_texture;
    GLuint fbo_depth;
};
