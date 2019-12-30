#ifndef DRAWABLES_H
#define DRAWABLES_H

#include "GLDriver.h"
#include "Scene.h"

class Effect;
class DrawContext;


class EffectProperty {
public:
    using Iterator = std::map<std::string, GLUniform>::iterator;

    using ConstIterator = std::map<std::string, GLUniform>::const_iterator;

    EffectProperty();

    EffectProperty(Effect *effect, const std::map<std::string, GLUniform> &values);

    operator bool() const;

    inline Effect *getEffect() { return _effect; }

    inline const Effect *getEffect() const { return _effect; }

    const GLUniform *getParam(const std::string &name) const;

    GLUniform *getParam(const std::string &name);

    inline Iterator begin() { return _values.begin(); }

    inline ConstIterator begin() const { return _values.end(); }

    inline Iterator end() { return _values.end(); }

    inline ConstIterator end() const { return _values.end(); }

private:
    Effect *_effect;
    std::map<std::string, GLUniform> _values;
};


class Effect {
public:
    Effect(const std::string &name, DrawContext *context, int ID, GLProgram program, glm::ivec4 viewport);

    Effect(const Effect &other) = delete;

    Effect(Effect &&other) = default;

    Effect &operator=(const Effect &) = delete;

    Effect &operator=(Effect &&other) noexcept;

    void swap(Effect &other) noexcept;

    inline const std::string &getName() const { return _name; }

    inline int getID() const { return _ID; }

    inline const std::map<std::string, GLAttribute> &getAttributes() const { return _attributes; }

    void setColorMask(bool red, bool blue, bool green, bool alpha);

    void enableCullFace(bool enableOrDisable);

    void setCullFace(unsigned face);

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

    void setViewport(glm::ivec4 viewport);

    EffectProperty createProperty();

    void bind();

    void unbind();

    void applyProperty(const EffectProperty &property);

private:
    std::string _name;
    DrawContext *_context;
    int _ID;
    GLProgram _program;
    std::map<std::string, GLAttribute> _attributes;

    unsigned _bufferBits;
    glm::vec4 _backgroundColor;
    glm::ivec4 _viewport;

    bool _redMask;
    bool _blueMask;
    bool _greenMask;
    bool _alphaMask;

    bool _enableCullface;
    unsigned _cullface;

    bool _enableStencilTest;
    unsigned _stencilFunc;
    int _stencilFuncRef;
    unsigned _stencilFuncMask;
    unsigned _stencilOpStencilFailMask;
    unsigned _stencilOpDepthFailMask;
    unsigned _stencilOpPassMask;

    bool _enableBlend;
    unsigned _blendEquaSFactor;
    unsigned _blendEquaDFactor;
    unsigned _blendEquation;

    bool _enableDepthTest;
    unsigned _depthFunc;
    double _depthRangeNear;
    double _depthRangeFar;
    bool _enableDepthMask;
};


class Geometry {
public:
    Geometry(DrawContext *context,
             unsigned usage,
             const std::vector<int> &elements,
             const std::vector<glm::vec3> &positions,
             const std::vector<glm::vec3> &normals);

    Geometry(const Geometry &) = delete;

    Geometry(Geometry &&) = default;

    Geometry &operator=(const Geometry &) = delete;

    Geometry &operator=(Geometry &&) noexcept;

    void swap(Geometry &) noexcept;

    void setEffectProperty(const EffectProperty &effectProperty);

    inline const EffectProperty &getEffectProperty() const { return _effectProperty; }

    inline EffectProperty &getEffectProperty() { return _effectProperty; }

    void bind();

    void unbind();

    void draw();

    static const GLAttribute POSITION_ATTRIBUTE;
    static const GLAttribute NORMAL_ATTRIBUTE;

private:
    void enableAttribute(const Effect *effect, const GLAttribute &requiredAttribute);

    DrawContext *_context;
    EffectProperty _effectProperty;
    GLBuffer _buffer;
    GLVertexArray _vao;
    unsigned _vertexCount;
    int _positionsOffset;
    int _normalsOffset;
};


class DrawContext {
public:
    using Node = Node<Geometry>;

    inline GLDriver *getDriver() { return &_driver; }

    inline const Node *getRoot() const { return &_root; }

    inline Node *getRoot() { return &_root; }

    Effect *createEffect(const std::string &name, const std::vector<std::pair<unsigned, std::string> > &shaderFiles);

    Effect *getEffect(int ID);

    void removeEffect(int ID);

private:
    GLDriver _driver;
    Node _root;
    std::vector<Effect> _effects;
};



#endif // DRAWABLES_H
