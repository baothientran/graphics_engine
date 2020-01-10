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
             unsigned usage,
             const std::vector<unsigned> &elements,
             const std::vector<glm::vec3> &positions,
             const std::vector<glm::vec3> &normals);

    Geometry(const Geometry &) = delete;

    Geometry(Geometry &&) = default;

    Geometry &operator=(const Geometry &) = delete;

    Geometry &operator=(Geometry &&) = default;

    void setEffectProperty(const EffectProperty &effectProperty) override;

    void setEffectProperty(EffectProperty &&effectProperty) override;

    inline const EffectProperty *getEffectProperty() const override;

    inline EffectProperty *getEffectProperty() override;

    void draw() override;

    static const GLAttribute POSITION_ATTRIBUTE;
    static const GLAttribute NORMAL_ATTRIBUTE;

private:
    void enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute, int offset);

    std::optional<EffectProperty> _effectProperty;
    std::optional<GLBuffer> _buffer;
    std::optional<GLVertexArray> _vao;
    unsigned _vertexCount;
    int _positionsOffset;
    int _normalsOffset;
};


#endif // DRAWABLES_H
