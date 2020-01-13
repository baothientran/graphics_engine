#ifndef DRAWABLES_H
#define DRAWABLES_H

#include "DrawContext.h"


class Geometry : public Drawable {
    struct GLAttribute {
        int size;
        unsigned dataType;
        bool normalized;
        int stride;
        std::string name;
    };

public:
    Geometry(DrawContext *context,
             std::shared_ptr<EffectProperty> effectProperty,
             std::shared_ptr<GLVertexArray> vao,
             std::shared_ptr<GLBuffer> buffer,
             unsigned numOfElement,
             unsigned elementOffset,
             int positionOffset,
             int normalOffset);

    Geometry(DrawContext *context,
             std::shared_ptr<EffectProperty> effectProperty,
             const std::vector<unsigned> &elements,
             const std::vector<glm::vec3> &positions,
             const std::vector<glm::vec3> &normals);

    Geometry(const Geometry &) = delete;

    Geometry(Geometry &&) = default;

    Geometry &operator=(const Geometry &) = delete;

    Geometry &operator=(Geometry &&) = default;

    inline const EffectProperty *getEffectProperty() const override;

    inline EffectProperty *getEffectProperty() override;

    void draw() override;

    static const GLAttribute POSITION_ATTRIBUTE;
    static const GLAttribute NORMAL_ATTRIBUTE;

private:
    void setEffectProperty(std::shared_ptr<EffectProperty> effectProperty);

    void enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute, int offset);

    std::shared_ptr<EffectProperty> _effectProperty;
    std::shared_ptr<GLVertexArray> _vao;
    std::shared_ptr<GLBuffer> _buffer;
    unsigned _numOfElements;
    unsigned _elementOffset;
    int _positionsOffset;
    int _normalsOffset;
};


class PointLight : public Drawable {
public:
    PointLight(DrawContext *context,
               glm::vec3 lightColor,
               float radius);

    inline void setLightColor(glm::vec3 lightColor) {
        _lightColor = lightColor;
    }

    inline glm::vec3 getLightColor() const { return _lightColor; }

    inline void setRadius(float radius) {
        _radius = radius;
    }

    inline float getRadius() const { return _radius; }

    PointLight *asPointLight() override;

    EffectProperty * getEffectProperty() override;

    const EffectProperty * getEffectProperty() const override;

    void draw() override;

private:
    Drawable *_geometry;
    glm::vec3 _lightColor;
    float _radius;
};

#endif // DRAWABLES_H
