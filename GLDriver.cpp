#include "GLDriver.h"
#include "glm/gtc/type_ptr.hpp"


/***************************************************
 * GLUniform utility definitions
 ***************************************************/
static UniformType getUniformType(unsigned type, int size) {
    switch (type) {
    case GL_INT:
        if (size == 1)
            return int{};
        return std::vector<int>();
    case GL_UNSIGNED_INT:
        if (size == 1)
            return unsigned{};
        return std::vector<unsigned>();
    case GL_FLOAT:
        if (size == 1)
            return float{};
        return std::vector<float>();
    case GL_DOUBLE:
        if (size == 1)
            return double{};
        return std::vector<double>();
    case GL_FLOAT_VEC2:
        if (size == 1)
            return glm::vec2{};
        return std::vector<glm::vec2>();
    case GL_FLOAT_VEC3:
        if (size == 1)
            return glm::vec3{};
        return std::vector<glm::vec3>();
    case GL_FLOAT_VEC4:
        if (size == 1)
            return glm::vec4{};
        return std::vector<glm::vec4>();
    case GL_FLOAT_MAT2:
        if (size == 1)
            return glm::mat2{};
        return std::vector<glm::mat2>();
    case GL_FLOAT_MAT3:
        if (size == 1)
            return glm::mat3{};
        return std::vector<glm::mat3>();
    case GL_FLOAT_MAT4:
        if (size == 1)
            return glm::mat4{};
        return std::vector<glm::mat4>();
    default:
        assert(false && "UNIFORM TYPE NOT IMPLEMENTED");
    }
}


/***************************************************
 * GLProgram definitions
 ***************************************************/
GLProgram::GLProgram(GLDriver *driver,
                     const std::vector<std::pair<unsigned, std::string>> &shaders)
    : _driver{driver}
{
    auto GL = driver->GL();

    _prog = GL->glCreateProgram();

    std::vector<unsigned> createdShaders(shaders.size());
    for (const auto &shaderInfo : shaders) {
        unsigned shaderType = shaderInfo.first;
        const char *shaderSrc = shaderInfo.second.c_str();
        unsigned shader = GL->glCreateShader(shaderType);
        createdShaders.push_back(shader);

        GL->glShaderSource(shader, 1, &shaderSrc, nullptr);
        GL->glCompileShader(shader);
        GL->glAttachShader(_prog, shader);

#ifndef NDEBUG
        int success;
        char infoLog[4048];
        GL->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GL->glGetShaderInfoLog(shader, 4048, nullptr, infoLog);
            if (shaderType == GL_VERTEX_SHADER)
                qDebug() << "VERTEX SHADER:";
            else if (shaderType == GL_FRAGMENT_SHADER)
                qDebug() << "FRAGMENT SHADER:";
            else
                qDebug() << "UNRECOGNIZED SHADER:";

            qDebug() << infoLog << "\n";
        }
#endif
    }

    GL->glLinkProgram(_prog);

#ifndef NDEBUG
    int success;
    char infoLog[4048];
    GL->glGetProgramiv(_prog, GL_LINK_STATUS, &success);
    if (!success) {
        GL->glGetProgramInfoLog(_prog, 4048, nullptr, infoLog);
        qDebug() << infoLog << "\n";
    }
#endif

    for (auto shader : createdShaders) {
        GL->glDeleteShader(shader);
        GL->glDetachShader(_prog, shader);
    }
}


GLProgram::GLProgram(GLProgram &&other) noexcept
    :_prog{other._prog},
    _driver{other._driver}
{
    other._prog = 0;
}


GLProgram &GLProgram::operator=(GLProgram &&other) noexcept {
    GLProgram(std::move(other)).swap(*this);
    return *this;
}


GLProgram::~GLProgram() noexcept {
    if (!_driver)
        return;

    auto GL = _driver->GL();
    GL->glDeleteProgram(_prog);
}


void GLProgram::swap(GLProgram &other) noexcept {
    using std::swap;
    swap(_driver, other._driver);
    swap(_prog, other._prog);
}


void GLProgram::bind() {
    auto GL = _driver->GL();
    GL->glUseProgram(_prog);
}


void GLProgram::unbind() {
    auto GL = _driver->GL();
    GL->glUseProgram(0);
}


std::map<std::string, int> GLProgram::getAttributes() const {
    auto GL = _driver->GL();
    int count;
    GL->glGetProgramiv(_prog, GL_ACTIVE_ATTRIBUTES, &count);

    std::map<std::string, int> attributes;

    char attribName[100];
    int attribSize;
    unsigned attribDataType;
    for (unsigned i = 0; i < static_cast<std::size_t>(count); ++i) {
        GL->glGetActiveAttrib(_prog, i, sizeof(attribName), nullptr, &attribSize, &attribDataType, attribName);
        attributes.insert({attribName, queryAttributeLocation(attribName)});
    }

    return attributes;
}


std::map<std::string, GLUniform> GLProgram::getUniforms() const {
    auto GL = _driver->GL();
    int count;
    GL->glGetProgramiv(_prog, GL_ACTIVE_UNIFORMS, &count);

    std::map<std::string, GLUniform> uniforms;

    char uniformName[100];
    int uniformSize;
    unsigned uniformType;
    for (unsigned i = 0; i < static_cast<std::size_t>(count); ++i) {
        GL->glGetActiveUniform(_prog, i, sizeof(uniformName), nullptr, &uniformSize, &uniformType, uniformName);
        GLUniform uniform(queryUniformLocation(uniformName), getUniformType(uniformType, uniformSize));
        uniforms.insert({uniformName, std::move(uniform)});
    }

    return uniforms;
}


void GLProgram::applyUniform(const GLUniform &uniform) {
    std::visit([&](const auto &val){
        applyUniformImpl(uniform.location(), val);
    }, uniform.getValue());
}


int GLProgram::queryUniformLocation(const std::string &uniformName) const {
    auto GL = _driver->GL();
    int uLoc = GL->glGetUniformLocation(_prog, uniformName.c_str());

#ifndef NDEBUG
    if (uLoc == -1)
        qDebug() << "Cannot Find Uniform: " << uniformName.data() << "\n";
#endif

    return uLoc;
}


int GLProgram::queryAttributeLocation(const std::string &attributeName) const {
    auto GL = _driver->GL();
    int aLoc = GL->glGetAttribLocation(_prog, attributeName.c_str());

#ifndef NDEBUG
    if (aLoc == -1)
        qDebug() << "Cannot Find Attribute: " << attributeName.data() << "\n";
#endif

    return aLoc;
}


void GLProgram::applyUniformImpl(int uniformLoc, const int *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform1iv(uniformLoc, count, val);
}


void GLProgram::applyUniformImpl(int uniformLoc, const unsigned *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform1uiv(uniformLoc, count, val);
}


void GLProgram::applyUniformImpl(int uniformLoc, const float *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform1fv(uniformLoc, count, val);
}


void GLProgram::applyUniformImpl(int uniformLoc, const double *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform1dv(uniformLoc, count, val);
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::vec2 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform2fv(uniformLoc, count, glm::value_ptr(*val));
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::vec3 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform3fv(uniformLoc, count, glm::value_ptr(*val));
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::vec4 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniform4fv(uniformLoc, count, glm::value_ptr(*val));
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::mat2 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniformMatrix2fv(uniformLoc, count, false, glm::value_ptr(*val));
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::mat3 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniformMatrix3fv(uniformLoc, count, false, glm::value_ptr(*val));
}


void GLProgram::applyUniformImpl(int uniformLoc, const glm::mat4 *val, int count) {
    auto GL = _driver->GL();
    GL->glUniformMatrix4fv(uniformLoc, count, false, glm::value_ptr(*val));
}


/***************************************************
 * GLBuffer definitions
 ***************************************************/
GLBuffer::GLBuffer(GLDriver *driver, unsigned target, unsigned usage)
    : _driver{driver}, _target{target}, _usage{usage}
{
    auto GL = _driver->GL();
    GL->glGenBuffers(1, &_buffer);
}


GLBuffer::GLBuffer(GLBuffer &&other) noexcept
    : _driver{other._driver},
    _buffer{other._buffer},
    _target{other._target},
    _usage{other._usage},
    _capacity{other._capacity}
{
    other._buffer = 0;
}


GLBuffer &GLBuffer::operator=(GLBuffer &&other) noexcept {
    GLBuffer(std::move(other)).swap(*this);
    return *this;
}


GLBuffer::~GLBuffer() noexcept {
    if (!_driver)
        return;

    auto GL = _driver->GL();
    GL->glDeleteBuffers(1, &_buffer);
}


void GLBuffer::swap(GLBuffer &other) noexcept {
    using std::swap;
    swap(_driver, other._driver);
    swap(_buffer, other._buffer);
    swap(_target, other._target);
    swap(_usage, other._usage);
    swap(_capacity, other._capacity);
}


void GLBuffer::bind() {
    auto GL = _driver->GL();
    GL->glBindBuffer(_target, _buffer);
}


void GLBuffer::unbind() {
    auto GL = _driver->GL();
    GL->glBindBuffer(_target, 0);
}


void GLBuffer::loadData(const void *data, int count) {
    auto GL = _driver->GL();
    GL->glBufferData(_target, count, data, _usage);
    _capacity = count;
}


void GLBuffer::loadSubData(int offset, const void *data, int count) {
    assert((offset + count <= _capacity) && "GLBuffer overflow");
    auto GL = _driver->GL();
    GL->glBufferSubData(_target, offset, count, data);
}


/***************************************************
 * GLVertexArray definitions
 ***************************************************/
GLVertexArray::GLVertexArray(GLDriver *driver, const std::vector<unsigned> &elements, unsigned usage)
    : _driver{driver}
{
    auto GL = _driver->GL();
    GL->glGenVertexArrays(1, &_vao);
    bind();
    _elementBuffer = _driver->createBuffer(GL_ELEMENT_ARRAY_BUFFER, usage);
    _elementBuffer->bind();
    _elementBuffer->loadData(elements.data(), static_cast<int>(elements.size() * sizeof(unsigned)));
    unbind();
    _elementBuffer->unbind();
}


GLVertexArray::GLVertexArray(GLVertexArray &&other) noexcept
    : _driver{other._driver},
    _elementBuffer{std::move(other._elementBuffer)},
    _vao{other._vao}
{
    other._vao = 0;
}


GLVertexArray &GLVertexArray::operator=(GLVertexArray &&other) noexcept {
    GLVertexArray(std::move(other)).swap(*this);
    return *this;
}


GLVertexArray::~GLVertexArray() noexcept {
    if (!_driver)
        return;

    auto GL = _driver->GL();
    GL->glDeleteVertexArrays(1, &_vao);
}


void GLVertexArray::swap(GLVertexArray &other) noexcept {
    using std::swap;
    swap(_driver, other._driver);
    swap(_elementBuffer, other._elementBuffer);
    swap(_vao, other._vao);
}


void GLVertexArray::bind() {
    auto GL = _driver->GL();
    GL->glBindVertexArray(_vao);
}


void GLVertexArray::unbind() {
    auto GL = _driver->GL();
    GL->glBindVertexArray(0);
}


void GLVertexArray::attribPointer(int attribIdx, int size, unsigned dataType, bool dataNormalized, int bufferStride, int bufferOffset) {
    auto GL = _driver->GL();
    GL->glVertexAttribPointer(static_cast<unsigned>(attribIdx), size, dataType, dataNormalized, bufferStride, reinterpret_cast<void*>(bufferOffset));
}


void GLVertexArray::enableAttrib(int attribIdx) {
    auto GL = _driver->GL();
    GL->glEnableVertexAttribArray(static_cast<unsigned>(attribIdx));
}


void GLVertexArray::disableAttrib(int attribIdx) {
    auto GL = _driver->GL();
    GL->glDisableVertexAttribArray(static_cast<unsigned>(attribIdx));
}


void GLVertexArray::attribDivisor(int attribIdx, unsigned divisor) {
    auto GL = _driver->GL();
    GL->glVertexAttribDivisor(static_cast<unsigned>(attribIdx), divisor);
}


/***************************************************
 * GLDriver definitions
 ***************************************************/
GLDriver::GLDriver() {}


void GLDriver::initialize(QSurface *surface) {
    _context.setFormat(surface->format());
    _context.create();
    _context.makeCurrent(surface);
    _GL.initializeOpenGLFunctions();

    _device = std::make_unique<QOpenGLPaintDevice>();
}


void GLDriver::swapBuffers(QSurface *surface) {
    _context.swapBuffers(surface);
}


void GLDriver::makeCurrent(QSurface *surface) {
    _context.makeCurrent(surface);
}


void GLDriver::setSize(const QSize &size) {
    _device->setSize(size);
}


void GLDriver::setDevicePixelRatio(qreal ratio) {
    _device->setDevicePixelRatio(ratio);
}

QPainter GLDriver::createPainter() {
    return QPainter(_device.get());
}


GLProgram GLDriver::createProgram(const std::vector<std::pair<unsigned, std::string> > &shaders) {
    return {this, shaders};
}


GLBuffer GLDriver::createBuffer(unsigned target, unsigned usage) {
    return {this, target, usage};
}


GLVertexArray GLDriver::createVertexArray(const std::vector<unsigned> &elements, unsigned usage) {
    return {this, elements, usage};
}


void GLDriver::setColorMask(bool red, bool blue, bool green, bool alpha) {
    _GL.glColorMask(red, green, blue, alpha);
}


void GLDriver::enableCullFace(bool enableOrDisable) {
    if (enableOrDisable) {
        _GL.glEnable(GL_CULL_FACE);
    }
    else {
        _GL.glDisable(GL_CULL_FACE);
    }
}


void GLDriver::setCullFace(unsigned face) {
    _GL.glCullFace(face);
}


void GLDriver::setFrontFace(unsigned face) {
    _GL.glFrontFace(face);
}


void GLDriver::enableStencilTest(bool enableOrDisable) {
    if (enableOrDisable) {
        _GL.glEnable(GL_STENCIL_TEST);
    }
    else {
        _GL.glDisable(GL_STENCIL_TEST);
    }
}


void GLDriver::setStencilFunc(unsigned func, int ref, unsigned mask) {
    _GL.glStencilFunc(func, ref, mask);
}


void GLDriver::setStencilOp(unsigned stencilFail, unsigned depthFail, unsigned depthStencilPass) {
    _GL.glStencilOp(stencilFail, depthFail, depthStencilPass);
}


void GLDriver::enableBlend(bool enableOrDisable) {
    if (enableOrDisable) {
        _GL.glEnable(GL_BLEND);
    }
    else {
        _GL.glDisable(GL_BLEND);
    }
}


void GLDriver::setBlendFunc(unsigned sfactor, unsigned dfactor) {
    _GL.glBlendFunc(sfactor, dfactor);
}


void GLDriver::setBlendEquation(unsigned equation) {
    _GL.glBlendEquation(equation);
}


void GLDriver::enableDepthTest(bool enableOrDisable) {
    if (enableOrDisable) {
        _GL.glEnable(GL_DEPTH_TEST);
    }
    else {
        _GL.glDisable(GL_DEPTH_TEST);
    }
}


void GLDriver::setDepthFunc(unsigned func) {
    _GL.glDepthFunc(func);
}


void GLDriver::setDepthRange(double near, double far) {
    _GL.glDepthRange(near, far);
}


void GLDriver::enableDepthMask(bool enableOrDisable) {
    _GL.glDepthMask(enableOrDisable);
}


void GLDriver::clearBufferBit(unsigned bufferBit) {
    _GL.glClear(bufferBit);
}


void GLDriver::clearColor(glm::vec4 color) {
    _GL.glClearColor(color.r, color.g, color.b, color.a);
}


void GLDriver::setViewport(int x, int y, int width, int height) {
    _GL.glViewport(x, y, width, height);
}


void GLDriver::drawElements(unsigned mode, unsigned elementCount, unsigned elementType, unsigned offset) {
    _GL.glDrawElements(mode, static_cast<int>(elementCount), elementType, reinterpret_cast<void*>(offset));
}


void GLDriver::drawElementsInstanced(unsigned mode, unsigned elementCount, unsigned elementType, unsigned offset, int instanceCount) {
    _GL.glDrawElementsInstanced(mode,
                                 static_cast<int>(elementCount),
                                 elementType,
                                 reinterpret_cast<void*>(offset),
                                 instanceCount);
}


