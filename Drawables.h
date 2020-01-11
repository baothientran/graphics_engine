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
             std::shared_ptr<GLVertexArray> vao,
             std::shared_ptr<GLBuffer> buffer,
             unsigned numOfElement,
             unsigned elementOffset,
             int positionOffset,
             int normalOffset);

    Geometry(const Geometry &) = delete;

    Geometry(Geometry &&) = default;

    Geometry &operator=(const Geometry &) = delete;

    Geometry &operator=(Geometry &&) = default;

    void setEffectProperty(std::shared_ptr<EffectProperty> effectProperty) override;

    inline const EffectProperty *getEffectProperty() const override;

    inline EffectProperty *getEffectProperty() override;

    void draw() override;

    static const GLAttribute POSITION_ATTRIBUTE;
    static const GLAttribute NORMAL_ATTRIBUTE;

private:
    void enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute, int offset);

    std::shared_ptr<EffectProperty> _effectProperty;
    std::shared_ptr<GLVertexArray> _vao;
    std::shared_ptr<GLBuffer> _buffer;
    unsigned _numOfElements;
    unsigned _elementOffset;
    int _positionsOffset;
    int _normalsOffset;
};


#endif // DRAWABLES_H
