#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"

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
    std::unique_ptr<ew::Shader> water;

    std::unique_ptr<ew::Texture> waveSpec;
    std::unique_ptr<ew::Texture> waveWarp;
    std::unique_ptr<ew::Texture> waveTex;

    ew::Mesh plane;

    batteries::light_t light;
    batteries::material_t material;
};
