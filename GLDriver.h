#ifndef GLDRIVER_H
#define GLDRIVER_H
#define GLM_FORCE_SWIZZLE

#include <string>
#include <memory>
#include <vector>
#include <QOpenGLFunctions_4_2_Core>
#include <QDebug>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QSurface>
#include <glm/glm.hpp>
#include <cassert>
#include <variant>


class GLDriver;


using UniformType = std::variant<
    int,
    unsigned,
    float,
    double,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat2,
    glm::mat3,
    glm::mat4,
    std::vector<int>,
    std::vector<unsigned>,
    std::vector<float>,
    std::vector<double>,
    std::vector<glm::vec2>,
    std::vector<glm::vec3>,
    std::vector<glm::vec4>,
    std::vector<glm::mat2>,
    std::vector<glm::mat3>,
    std::vector<glm::mat4>
    >;


class GLUniform {
public:
    GLUniform(int location, UniformType val)
        : _val{val}, _location{location}
    {}

    template<typename T>
    inline void setValue(const T & value) {
        assert(std::holds_alternative<T>(_val) && "UNIFORM VALUE HAS TO BE THE SAME TYPE");
        _val = value;
    }

    inline const UniformType &getValue() const { return _val; }

    inline int location() const { return _location; }

private:
    UniformType _val;
    int _location;
};


class GLProgram {
public:
    GLProgram(GLDriver *driver,
              const std::vector<std::pair<unsigned, std::string>> &shaders);

    GLProgram(const GLProgram &) = delete;

    GLProgram(GLProgram &&) noexcept;

    GLProgram &operator=(const GLProgram &) = delete;

    GLProgram &operator=(GLProgram &&) noexcept;

    ~GLProgram() noexcept;

    void swap(GLProgram &other) noexcept;

    void bind();

    void unbind();

    std::map<std::string, GLUniform> getUniforms() const;

    std::map<std::string, int> getAttributes() const;

    void applyUniform(const GLUniform &uniform);

private:
    int queryUniformLocation(const std::string &uniformName) const;

    int queryAttributeLocation(const std::string &uniformName) const;

    template<typename T>
    inline void applyUniformImpl(int uniformLoc, T val) {
        applyUniformImpl(uniformLoc, &val, 1);
    }

    template<typename T>
    inline void applyUniformImpl(int uniformLoc, const std::vector<T> &vals) {
        applyUniformImpl(uniformLoc, vals.data(), static_cast<int>(vals.size()));
    }

    void applyUniformImpl(int uniformLoc, const int *val, int count);

    void applyUniformImpl(int uniformLoc, const unsigned *val, int count);

    void applyUniformImpl(int uniformLoc, const float *val, int count);

    void applyUniformImpl(int uniformLoc, const double *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::vec2 *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::vec3 *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::vec4 *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::mat2 *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::mat3 *val, int count);

    void applyUniformImpl(int uniformLoc, const glm::mat4 *val, int count);

    unsigned _prog;
    GLDriver *_driver;
};


class GLBuffer {
public:
    GLBuffer(GLDriver *driver, unsigned target, unsigned usage);

    GLBuffer(const GLBuffer &) = delete;

    GLBuffer(GLBuffer &&) noexcept;

    GLBuffer &operator=(const GLBuffer &) = delete;

    GLBuffer &operator=(GLBuffer &&) noexcept;

    ~GLBuffer() noexcept;

    void swap(GLBuffer &other) noexcept;

    void bind();

    void unbind();

    void loadData(const void *data, int count);

    void loadSubData(int offset, const void *data, int count);

private:
    GLDriver *_driver;
    unsigned _buffer;
    unsigned _target;
    unsigned _usage;
    int _capacity;
};


class GLVertexArray {
public:
    GLVertexArray(GLDriver *driver, const std::vector<unsigned> &elements, unsigned usage);

    GLVertexArray(const GLVertexArray &) = delete;

    GLVertexArray(GLVertexArray &&) noexcept;

    GLVertexArray &operator=(const GLVertexArray &) = delete;

    GLVertexArray &operator=(GLVertexArray &&) noexcept;

    ~GLVertexArray() noexcept;

    void swap(GLVertexArray &) noexcept;

    void bind();

    void unbind();

    void attribPointer(int attribIdx, int size, unsigned type, bool normalized, int bufferStride, int bufferOffset);

    void enableAttrib(int attribIdx);

    void disableAttrib(int attribIdx);

    void attribDivisor(int attribIdx, unsigned divisor);

private:
    GLDriver *_driver;
    std::optional<GLBuffer> _elementBuffer;
    unsigned _vao;
};


class GLDriver {
public:
    GLDriver();

    GLDriver(const GLDriver &) = delete;

    GLDriver &operator=(const GLDriver &) = delete;

    inline QOpenGLFunctions_4_2_Core *GL() { return &_GL; }

    void initialize(QSurface *surface);

    void swapBuffers(QSurface *surface);

    void makeCurrent(QSurface *surface);

    void setSize(const QSize &size);

    void setDevicePixelRatio(qreal ratio);

    QPainter createPainter();

    GLProgram createProgram(const std::vector<std::pair<unsigned, std::string>> &shaders);

    GLBuffer createBuffer(unsigned target, unsigned usage);

    GLVertexArray createVertexArray(const std::vector<unsigned> &elements, unsigned usage);

    void setColorMask(bool red, bool blue, bool green, bool alpha);

    void enableCullFace(bool enableOrDisable);

    void setCullFace(unsigned face);

    void setFrontFace(unsigned face);

    void enableStencilTest(bool enableOrDisable);

    void setStencilFunc(unsigned func, int ref, unsigned mask);

    void setStencilOp(unsigned stencilFail, unsigned depthFail, unsigned depthStencilPass);

    void enableBlend(bool enableOrDisable);

    void setBlendFunc(unsigned sfactor, unsigned dfactor);

    void setBlendEquation(unsigned equation);

    void enableDepthTest(bool enableOrDisable);

    void setDepthFunc(unsigned func);

    void setDepthRange(double near, double far);

    void enableDepthMask(bool enableOrDisable);

    void clearBufferBit(unsigned bufferBit);

    void clearColor(glm::vec4 color);

    void setViewport(int x, int y, int width, int height);

    void drawElements(unsigned mode, unsigned elementCount, unsigned elementType, unsigned offset);

    void drawElementsInstanced(unsigned mode, unsigned elementCount, unsigned elementType, unsigned offset, int instanceCount);

private:
    QOpenGLFunctions_4_2_Core _GL;
    QOpenGLContext _context;
    std::unique_ptr<QOpenGLPaintDevice> _device;
};



#endif // GLDRIVER_H
