#ifndef EFFECTS_H
#define EFFECTS_H


#include "DrawContext.h"


class ColorEffect : public Effect {
public:
    ColorEffect(DrawContext *context);

    const std::map<std::string, int> &getAttributes() const override;

    EffectProperty createEffectProperty() override;

    void draw(const std::vector<Drawable*> &drawables) override;

    static const std::string EFFECT_NAME;
};


class ForwardPhongEffect : public Effect {
public:
    ForwardPhongEffect(DrawContext *context);

    const std::map<std::string, int> &getAttributes() const override;

    EffectProperty createEffectProperty() override;

    void draw(const std::vector<Drawable*> &drawables) override;

    static const std::string EFFECT_NAME;

    static const std::string DIFFUSE_COLOR;
    static const std::string SPECULAR_COLOR;
    static const std::string DIFFUSE_REFLECTION;
    static const std::string SPECULAR_REFLECTION;
    static const std::string SHININESS;

private:
    static const std::string MVP_MAT;
    static const std::string MV_MAT;
    static const std::string NORMAL_MAT;
    static const std::string LIGHT_POSITION;
    static const std::string LIGHT_COLOR;
    static const std::string LIGHT_AMBIENT;
    static const std::string LIGHT_RADIUS;

    std::optional<GLProgram> _program;
    std::map<std::string, GLUniform> _effectUniforms;
    std::map<std::string, GLUniform> _drawableUniforms;
    std::map<std::string, int> _attributes;
};


#endif // EFFECTS_H
