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

struct FrameBuffer{
  GLuint fbo;
  GLuint color0;
  GLuint depth;

  void Initialize();
};

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    void ReflectionPass(const glm::mat4x4 view_proj, ew::Model* model, glm::vec4 clipPlane);
    void RefractionPass(const glm::mat4x4 view_proj, ew::Model* model, glm::vec4 clipPlane);

    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Model> monument;
    std::unique_ptr<ew::Shader> ambient;

    std::unique_ptr<ew::Shader> depth;
    
    //water
    std::unique_ptr<ew::Shader> water;
    std::unique_ptr<ew::Texture> waveWarp;
    std::unique_ptr<ew::Texture> waveSpec;

    // Post process
    std::vector<std::unique_ptr<ew::Shader>> postProcessingEffects;

    batteries::light_t light;
    batteries::material_t material;

    GLuint fbo; // frame buffer object
    GLuint fbo_texture;
    GLuint fbo_depth;

    // Depth
    GLuint shadow_fbo; // frame buffer object
    GLuint shadow_depth;

    //water framebuffers
    FrameBuffer reflection;
    FrameBuffer refraction; 
    FrameBuffer waterBuffer;
    ew::Mesh plane;

    void createFrameBuffer();
    void createDepthBuffer();
    void assignEffect(ew::Shader* shader);
    glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
};
