#include <utility>
#include "Drawables.h"


/***************************************************************
 * EffectProperty definitions
 ***************************************************************/
Effect::Effect(const std::string &name, DrawContext *context, int ID, GLProgram program, glm::ivec4 viewport)
    : _name{name},
      _context{context},
      _ID{ID},
      _program{std::move(program)},

      _bufferBits{GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT},
      _backgroundColor{0.0f, 0.0f, 0.0f, 1.0f},
      _viewport{viewport},

      _redMask{true},
      _blueMask{true},
      _greenMask{true},
      _alphaMask{true},

      _enableCullface{false},
      _cullface{GL_CCW},

      _enableStencilTest{false},
      _stencilFunc{GL_ALWAYS},
      _stencilFuncRef{0x00},
      _stencilFuncMask{0xFF},
      _stencilOpStencilFailMask{GL_KEEP},
      _stencilOpDepthFailMask{GL_KEEP},
      _stencilOpPassMask{GL_KEEP},

      _enableBlend{false},
      _blendEquaSFactor{GL_ONE},
      _blendEquaDFactor{GL_ZERO},
      _blendEquation{GL_FUNC_ADD},

      _enableDepthTest{false},
      _depthFunc{GL_LESS},
      _depthRangeNear{0.0},
      _depthRangeFar{1.0},
      _enableDepthMask{false}
{
    for (const auto &attribute : _program.getAttributes()) {
        _attributes.insert({attribute.name, attribute});
    }
}


Effect &Effect::operator=(Effect &&other) noexcept {
    Effect(std::move(other)).swap(*this);
    return *this;
}


void Effect::swap(Effect &other) noexcept {
    using std::swap;
    swap(_name, other._name);
    swap(_context, other._context);
    swap(_ID, other._ID);
    swap(_program, other._program);
    swap(_attributes, other._attributes);

    swap(_bufferBits, other._bufferBits);
    swap(_backgroundColor, other._backgroundColor);
    swap(_viewport, other._viewport);

    swap(_redMask, other._redMask);
    swap(_blueMask, other._blueMask);
    swap(_greenMask, other._greenMask);
    swap(_alphaMask, other._alphaMask);

    swap(_enableCullface, other._enableCullface);
    swap(_cullface, other._cullface);

    swap(_enableStencilTest, other._enableStencilTest);
    swap(_stencilFunc, other._stencilFunc);
    swap(_stencilFuncRef, other._stencilFuncRef);
    swap(_stencilFuncMask, other._stencilFuncMask);
    swap(_stencilOpStencilFailMask, other._stencilOpStencilFailMask);
    swap(_stencilOpDepthFailMask, other._stencilOpDepthFailMask);
    swap(_stencilOpPassMask, other._stencilOpPassMask);

    swap(_enableBlend, other._enableBlend);
    swap(_blendEquaSFactor, other._blendEquaSFactor);
    swap(_blendEquaDFactor, other._blendEquaDFactor);
    swap(_blendEquation, other._blendEquation);

    swap(_enableDepthTest, other._enableDepthTest);
    swap(_depthFunc, other._depthFunc);
    swap(_depthRangeNear, other._depthRangeNear);
    swap(_depthRangeFar, other._depthRangeFar);
    swap(_enableDepthMask, other._enableDepthMask);
}


void Effect::setColorMask(bool red, bool blue, bool green, bool alpha) {
    _redMask = red;
    _blueMask = blue;
    _greenMask = green;
    _alphaMask = alpha;
}


void Effect::enableCullFace(bool enableOrDisable) {
    _enableCullface = enableOrDisable;
}


void Effect::setCullFace(unsigned face) {
    _cullface = face;
}


void Effect::enableStencilTest(bool enableOrDisable) {
    _enableStencilTest = enableOrDisable;
}


void Effect::setStencilFunc(unsigned func, int ref, unsigned mask) {
    _stencilFunc = func;
    _stencilFuncRef = ref;
    _stencilFuncMask = mask;
}


void Effect::setStencilOp(unsigned stencilFail, unsigned depthFail, unsigned depthStencilPass) {
    _stencilOpStencilFailMask = stencilFail;
    _stencilOpDepthFailMask = depthFail;
    _stencilOpPassMask = depthStencilPass;
}


void Effect::enableBlend(bool enableOrDisable) {
    _enableBlend = enableOrDisable;
}


void Effect::setBlendFunc(unsigned sfactor, unsigned dfactor) {
    _blendEquaSFactor = sfactor;
    _blendEquaDFactor = dfactor;
}


void Effect::setBlendEquation(unsigned equation) {
    _blendEquation = equation;
}


void Effect::enableDepthTest(bool enableOrDisable) {
    _enableDepthTest = enableOrDisable;
}


void Effect::setDepthFunc(unsigned func) {
    _depthFunc = func;
}


void Effect::setDepthRange(double near, double far) {
    _depthRangeNear = near;
    _depthRangeFar = far;
}


void Effect::enableDepthMask(bool enableOrDisable) {
    _enableDepthMask = enableOrDisable;
}


void Effect::clearBufferBit(unsigned bufferBit) {
    _bufferBits = bufferBit;
}


void Effect::clearColor(glm::vec4 color) {
    _backgroundColor = color;
}


void Effect::setViewport(glm::ivec4 viewport) {
    _viewport = viewport;
}


EffectProperty Effect::createProperty() {
    const auto &uniforms = _program.getUniforms();
    std::map<std::string, GLUniform> values;
    for (const auto &uniform : uniforms) {
        values.insert({std::string(uniform.name()), uniform});
    }

    return {this, values};
}


void Effect::bind() {
    auto driver = _context->getDriver();

    // bind framebuffer here
    driver->setViewport(_viewport.x, _viewport.y, _viewport.z, _viewport.w);
    driver->clearColor(_backgroundColor);
    driver->clearBufferBit(_bufferBits);
    driver->setColorMask(_redMask, _blueMask, _greenMask, _alphaMask);

    driver->enableCullFace(_enableCullface);
    driver->setCullFace(_cullface);

    driver->enableStencilTest(_enableStencilTest);
    driver->setStencilFunc(_stencilFunc, _stencilFuncRef, _stencilFuncMask);
    driver->setStencilOp(_stencilOpStencilFailMask, _stencilOpDepthFailMask, _stencilOpPassMask);

    driver->enableBlend(_enableBlend);
    driver->setBlendFunc(_blendEquaSFactor, _blendEquaDFactor);
    driver->setBlendEquation(_blendEquation);

    driver->enableDepthTest(_enableDepthTest);
    driver->setDepthFunc(_depthFunc);
    driver->setDepthRange(_depthRangeNear, _depthRangeFar);
    driver->enableDepthMask(_enableDepthMask);

    // bind program
    _program.bind();
}


void Effect::unbind() {
    // unbind framebuffer

    // unbind program
    _program.unbind();
}


void Effect::applyProperty(const EffectProperty &property) {
    for (const auto &p : property) {
        _program.applyUniform(p.second);
    }
}


/***************************************************************
 * EffectProperty definitions
 ***************************************************************/
EffectProperty::EffectProperty()
    : _effect{nullptr}
{}


EffectProperty::EffectProperty(Effect *effect, const std::map<std::string, GLUniform> &values)
    : _effect{effect}, _values{values}
{}


EffectProperty::operator bool() const {
    return _effect != nullptr;
}


const GLUniform *EffectProperty::getParam(const std::string &name) const {
    auto uniform = _values.find(name);
    if (uniform == _values.end())
        return nullptr;
    return &uniform->second;
}


GLUniform *EffectProperty::getParam(const std::string &name) {
    auto uniform = _values.find(name);
    if (uniform == _values.end())
        return nullptr;
    return &uniform->second;
}


/***************************************************
 * Geometry definitions
 ***************************************************/
const GLAttribute Geometry::POSITION_ATTRIBUTE = {
    "vPosition",
    3,
    GL_FLOAT,
    -1
};


const GLAttribute Geometry::NORMAL_ATTRIBUTE = {
    "vNormal",
    3,
    GL_FLOAT,
    -1
};


Geometry::Geometry(DrawContext *context,
                   unsigned usage,
                   const std::vector<int> &elements,
                   const std::vector<glm::vec3> &positions,
                   const std::vector<glm::vec3> &normals)
    : _context{context}, _vertexCount{static_cast<unsigned>(elements.size())}
{
    int normalsCount = static_cast<int>(normals.size() * sizeof(glm::vec3));
    int positionsCount = static_cast<int>(positions.size() * sizeof(glm::vec3));

    auto driver = _context->getDriver();
    _vao = driver->createVertexArray(elements, usage);

    _buffer = driver->createBuffer(GL_ARRAY_BUFFER, usage);
    _buffer.bind();
    _buffer.loadData(nullptr, positionsCount + normalsCount);

    if (!positions.empty()) {
        _positionsOffset = 0;
        _buffer.loadSubData(_positionsOffset, positions.data(), positionsCount);
    }
    else {
        _positionsOffset = -1;
    }

    if (!normals.empty()) {
        _normalsOffset = positionsCount;
        _buffer.loadSubData(_normalsOffset, normals.data(), normalsCount);
    }
    else {
        _normalsOffset = -1;
    }
}


Geometry &Geometry::operator=(Geometry &&other) noexcept {
    Geometry(std::move(other)).swap(*this);
    return *this;
}


void Geometry::swap(Geometry &other) noexcept {
    using std::swap;
    swap(_context, other._context);
    swap(_buffer, other._buffer);
    swap(_vao, other._vao);
    swap(_vertexCount, other._vertexCount);
    swap(_positionsOffset, other._positionsOffset);
    swap(_normalsOffset, other._normalsOffset);
}


void Geometry::setEffectProperty(const EffectProperty &effectProperty) {
    auto effect = effectProperty.getEffect();

    // enable attribs for VAO in here
    _vao.bind();
    enableAttribute(effect, POSITION_ATTRIBUTE);
    enableAttribute(effect, NORMAL_ATTRIBUTE);
    _vao.unbind();

    _effectProperty = effectProperty;
}


void Geometry::bind() {
    _vao.bind();
}


void Geometry::unbind() {
    _vao.unbind();
}


void Geometry::draw() {
    _context->getDriver()->drawElements(GL_TRIANGLES, _vertexCount, GL_INT, 0);
}


void Geometry::enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute) {
    const auto &attributes = effect->getAttributes();
    auto effectAttrib = attributes.find(requiredAttribute.name);
    if (_positionsOffset != -1 &&
        effectAttrib == attributes.end() &&
        effectAttrib->second.size == requiredAttribute.size &&
        effectAttrib->second.dataType == requiredAttribute.dataType)
    {
        const auto &attrib = effectAttrib->second;
        _vao.attribPointer(attrib.location, attrib.size, attrib.dataType, false, 0, _positionsOffset);
        _vao.enableAttrib(attrib.location);
    }
}


/***************************************************
 * DrawContext definitions
 ***************************************************/
Effect *DrawContext::createEffect(const std::string &name, const std::vector<std::pair<unsigned, std::string> > &shaderFiles) {
    // read from shader files
    std::vector<std::pair<unsigned, std::string>> shaderSrcs;
    shaderSrcs.reserve(shaderFiles.size());

    GLProgram program = _driver.createProgram(shaderSrcs);
    auto &effect = _effects.emplace_back(name, this, _effects.size(), std::move(program), glm::vec4{});
    return &effect;
}


Effect *DrawContext::getEffect(int ID) {
    assert((ID < _effects.size() && ID >= 0) && "INVALID ID EFFECT");
    return &_effects[ID];
}


void DrawContext::removeEffect(int ID) {
    assert((ID < _effects.size() && ID >= 0) && "INVALID ID EFFECT");
    _effects.erase(_effects.begin() + ID);
}

