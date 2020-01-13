#ifndef EFFECTS_H
#define EFFECTS_H


#include "DrawContext.h"


class ForwardPhongEffect : public Effect {
public:
    ForwardPhongEffect(DrawContext *context);

    const std::map<std::string, int> &getAttributes() const override;

    EffectProperty createEffectProperty() override;

    void draw(const std::vector<Drawable*> &drawables,
              const std::vector<PointLight *> &pointLights) override;

    static const std::string EFFECT_NAME;
    static const std::string AMBIENT_COLOR;
    static const std::string DIFFUSE_COLOR;
    static const std::string SPECULAR_COLOR;
    static const std::string SHININESS;

private:
    struct PointLightUniforms {
        PointLightUniforms() {
            for (std::size_t i = 0; i < MAX; ++i) {
                std::string pointLight = "pointLights[" + std::to_string(i) +"]";
                positions[i] = pointLight + ".position";
                colors[i] = pointLight + ".color";
                radius[i] = pointLight + ".radius";
            }
        }

        static const std::size_t MAX;
        std::string positions[10];
        std::string colors[10];
        std::string radius[10];
    };

    static const std::string MVP_MAT;
    static const std::string MV_MAT;
    static const std::string NORMAL_MAT;
    static const std::string LIGHT_AMBIENT;
    static const PointLightUniforms POINT_LIGHTS;

    std::optional<GLProgram> _program;
    std::map<std::string, GLUniform> _effectUniforms;
    std::map<std::string, GLUniform> _drawableUniforms;
    std::map<std::string, int> _attributes;
};


#endif // EFFECTS_H
