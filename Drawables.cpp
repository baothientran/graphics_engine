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
                   std::shared_ptr<EffectProperty> effectProperty,
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
    setEffectProperty(std::move(effectProperty));
}


Geometry::Geometry(DrawContext *context,
                   std::shared_ptr<EffectProperty> effectProperty,
                   const std::vector<unsigned> &elements,
                   const std::vector<glm::vec3> &positions,
                   const std::vector<glm::vec3> &normals)
    : Drawable{context}
{
    _numOfElements = static_cast<unsigned>(elements.size());
    _elementOffset = 0;

    // create VAO and Buffer
    int positionCount = static_cast<int>(positions.size() * sizeof(glm::vec3));
    int normalCount = static_cast<int>(normals.size() * sizeof(glm::vec3));
    _positionsOffset = -1;
    _normalsOffset = -1;

    GLVertexArray vao = _context->getDriver().createVertexArray(elements.data(), elements.size(), GL_STATIC_DRAW);
    GLBuffer buffer = _context->getDriver().createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    buffer.bind();
    buffer.loadData(nullptr, positionCount + normalCount);
    if (!positions.empty()) {
        _positionsOffset = 0;
        buffer.loadSubData(_positionsOffset, positions.data(), positionCount);
    }

    if (!normals.empty()) {
        _normalsOffset = positionCount;
        buffer.loadSubData(_normalsOffset, normals.data(), normalCount);
    }

    _vao = std::make_shared<GLVertexArray>(std::move(vao));
    _buffer = std::make_shared<GLBuffer>(std::move(buffer));

    // set property
    setEffectProperty(std::move(effectProperty));
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


void Geometry::setEffectProperty(std::shared_ptr<EffectProperty> effectProperty) {
    auto effect = effectProperty->getEffect();

    // enable attribs for VAO in here
    _vao->bind();
    enableAttribute(effect, POSITION_ATTRIBUTE, _positionsOffset);
    enableAttribute(effect, NORMAL_ATTRIBUTE, _normalsOffset);
    _vao->unbind();

    _effectProperty = effectProperty;
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


/***************************************************
 * PointLight definitions
 ***************************************************/
PointLight::PointLight(DrawContext *context, glm::vec3 lightColor, float radius)
    : Drawable{context}, _lightColor{lightColor}, _radius{radius}
{
    _geometry = _context->getPointLightGeometry();
}


PointLight *PointLight::asPointLight() {
    return this;
}


EffectProperty *PointLight::getEffectProperty() {
    return _geometry->getEffectProperty();
}


const EffectProperty *PointLight::getEffectProperty() const {
    return _geometry->getEffectProperty();
}


void PointLight::draw() {
    _geometry->draw();
}
