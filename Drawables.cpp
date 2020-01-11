#include "Drawables.h"


/***************************************************
 * Geometry definitions
 ***************************************************/
const Geometry::GLAttribute Geometry::POSITION_ATTRIBUTE = {
    3,
    GL_FLOAT,
    false,
    0,
    "vPosition"
};


const Geometry::GLAttribute Geometry::NORMAL_ATTRIBUTE = {
    3,
    GL_FLOAT,
    false,
    0,
    "vNormal"
};


Geometry::Geometry(DrawContext *context,
                   std::shared_ptr<GLVertexArray> vao,
                   std::shared_ptr<GLBuffer> buffer,
                   unsigned numOfElements,
                   unsigned elementOffset,
                   int positionOffset,
                   int normalOffset)
    : Drawable{context},
    _vao{std::move(vao)},
    _buffer{std::move(buffer)},
    _numOfElements{numOfElements},
    _elementOffset{elementOffset},
    _positionsOffset{positionOffset},
    _normalsOffset{normalOffset}
{

}


void Geometry::setEffectProperty(std::shared_ptr<EffectProperty> effectProperty) {
    auto effect = effectProperty->getEffect();

    // enable attribs for VAO in here
    _vao->bind();
    enableAttribute(effect, POSITION_ATTRIBUTE, _positionsOffset);
    enableAttribute(effect, NORMAL_ATTRIBUTE, _normalsOffset);
    _vao->unbind();

    _effectProperty = effectProperty;
}


const EffectProperty *Geometry::getEffectProperty() const {
    return _effectProperty.get();
}


EffectProperty *Geometry::getEffectProperty() {
    return _effectProperty.get();
}


void Geometry::draw() {
    _vao->bind();
    _context->getDriver().drawElements(GL_TRIANGLES, _numOfElements, GL_UNSIGNED_INT, _elementOffset);
}


void Geometry::enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute, int offset) {
    const auto &attributes = effect->getAttributes();
    auto effectAttrib = attributes.find(requiredAttribute.name);
    if (offset != -1 &&
        effectAttrib != attributes.end())
    {
        auto attribLoc = effectAttrib->second;
        _vao->attribPointer(attribLoc,
                            requiredAttribute.size,
                            requiredAttribute.dataType,
                            requiredAttribute.normalized,
                            requiredAttribute.stride,
                            offset);

        _vao->enableAttrib(attribLoc);
    }
}

