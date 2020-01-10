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
                   unsigned usage,
                   const std::vector<unsigned> &elements,
                   const std::vector<glm::vec3> &positions,
                   const std::vector<glm::vec3> &normals)
    : Drawable{context}, _vertexCount{static_cast<unsigned>(elements.size())}
{
    int positionsCount = static_cast<int>(positions.size() * sizeof(glm::vec3));
    int normalsCount = static_cast<int>(normals.size() * sizeof(glm::vec3));

    auto &driver = _context->getDriver();
    _vao = driver.createVertexArray(elements, usage);

    _buffer = driver.createBuffer(GL_ARRAY_BUFFER, usage);
    _buffer->bind();
    _buffer->loadData(nullptr, positionsCount + normalsCount);

    if (!positions.empty()) {
        _positionsOffset = 0;
        _buffer->loadSubData(_positionsOffset, positions.data(), positionsCount);
    }
    else {
        _positionsOffset = -1;
    }

    if (!normals.empty()) {
        _normalsOffset = positionsCount;
        _buffer->loadSubData(_normalsOffset, normals.data(), normalsCount);
    }
    else {
        _normalsOffset = -1;
    }
}


void Geometry::setEffectProperty(const EffectProperty &effectProperty) {
    auto effect = effectProperty.getEffect();

    // enable attribs for VAO in here
    _vao->bind();
    enableAttribute(effect, POSITION_ATTRIBUTE, _positionsOffset);
    enableAttribute(effect, NORMAL_ATTRIBUTE, _normalsOffset);
    _vao->unbind();

    _effectProperty = effectProperty;
}


void Geometry::setEffectProperty(EffectProperty &&effectProperty) {
    auto effect = effectProperty.getEffect();

    // enable attribs for VAO in here
    _vao->bind();
    enableAttribute(effect, POSITION_ATTRIBUTE, _positionsOffset);
    enableAttribute(effect, NORMAL_ATTRIBUTE, _normalsOffset);
    _vao->unbind();

    _effectProperty = std::move(effectProperty);
}


const EffectProperty *Geometry::getEffectProperty() const {
    return _effectProperty == std::nullopt ? nullptr : &_effectProperty.value();
}


EffectProperty *Geometry::getEffectProperty() {
    return _effectProperty == std::nullopt ? nullptr : &_effectProperty.value();
}


void Geometry::draw() {
    _vao->bind();
    _context->getDriver().drawElements(GL_TRIANGLES, _vertexCount, GL_UNSIGNED_INT, 0);
    _vao->unbind();
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

