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
    std::unique_ptr<ew::Shader> blinnphong;

    std::unique_ptr<ew::Shader> depth;
    ew::Mesh plane;

    // Post process
    std::unique_ptr<ew::Shader> postProcess;
    std::vector<std::unique_ptr<ew::Shader>> postProcessingEffects;

    std::unique_ptr<ew::Texture> colorblind;
    std::unique_ptr<ew::Texture> ornament;
    std::unique_ptr<ew::Texture> normalMap;

    batteries::light_t light;
    batteries::material_t material;

    GLuint fbo; // frame buffer object
    GLuint fbo_texture;
    GLuint fbo_depth;

    // Depth
    GLuint shadow_fbo; // frame buffer object
    GLuint shadow_depth;

    void createFrameBuffer();
    void createDepthBuffer();
    void assignEffect(ew::Shader* shader);
};
