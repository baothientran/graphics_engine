#ifndef DRAWCONTEXT_H
#define DRAWCONTEXT_H

#include <queue>
#include "GLDriver.h"
#include "Scene.h"

class DrawContext;
class Effect;
class Drawable;


class Camera {
public:
    Camera();

    void setProjMatrix(glm::mat4 projMatrix);

    void setPosition(glm::vec3 position);

    void setFocus(glm::vec3 focus);

    void setViewQuat(glm::quat viewQuat);

    glm::vec3 getFocus() const;

    glm::vec3 getPosition() const;

    glm::quat getViewQuat() const;

    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjMatrix() const;

    static const glm::vec3 UP_DIRECTION;

private:
    mutable bool _isDirty;
    glm::vec3 _focus;
    glm::vec3 _position;
    glm::quat _viewQuat;
    mutable glm::mat4 _viewMatrix;
    glm::mat4 _projMatrix;
};


class EffectProperty {
public:
    using Iterator = std::map<std::string, GLUniform>::iterator;

    using ConstIterator = std::map<std::string, GLUniform>::const_iterator;

    EffectProperty(Effect *effect, const std::map<std::string, GLUniform> &values);

    inline Effect *getEffect() { return _effect; }

    inline const Effect *getEffect() const { return _effect; }

    template<typename T>
    void setParam(const std::string &name, T &&val) {
        _values.at(name).setValue(std::forward<T>(val));
    }

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
    Effect(DrawContext *context)
        : _context{context}
    {}

    virtual ~Effect() = default;

    inline DrawContext *getContext() { return _context; }

    virtual const std::map<std::string, int> &getAttributes() const = 0;

    virtual EffectProperty createEffectProperty() = 0;

    virtual void draw(const std::vector<Drawable*> &drawables) = 0;

protected:
    DrawContext *_context;
};


class Drawable {
public:
    Drawable(DrawContext *context)
        : _context{context}
    {}

    virtual ~Drawable() = default;

    inline const std::string &getName() const { return _name; }

    inline void setName(const std::string &name) { _name = name; }

    inline DrawContext *getContext() { return _context; }

    inline void setTransformation(glm::mat4 transformaiton) {
        _transformation = std::move(transformaiton);
    }

    inline glm::mat4 getTransformation() const { return _transformation; }

    virtual void setEffectProperty(std::shared_ptr<EffectProperty>) {}

    virtual const EffectProperty *getEffectProperty() const { return nullptr; }

    virtual EffectProperty *getEffectProperty() { return nullptr; }

    virtual void draw() = 0;

protected:
    DrawContext *_context;
    glm::mat4 _transformation;

private:
    std::string _name;
};


class DrawContext {
public:
    using SceneNode = Node<std::unique_ptr<Drawable>>;

    inline GLDriver &getDriver() { return _driver; }

    inline const SceneNode &getRoot() const { return _root; }

    inline SceneNode &getRoot() { return _root; }

    template<typename DrawableDerived, typename... Args>
    std::unique_ptr<DrawableDerived> createDrawable(Args&&... args) {
        return std::make_unique<DrawableDerived>(this, std::forward<Args>(args)...);
    }

    template<typename EffectDerived, typename... Args>
    EffectDerived &createEffect(const std::string &name, Args&&... args) {
        std::unique_ptr<EffectDerived> effect = std::make_unique<EffectDerived>(this, std::forward<Args>(args)...);
        EffectDerived *effectPtr = effect.get();

#ifndef NDEBUG
        if (_effects.find(name) != _effects.end())
            qDebug() << "Another effect with name " << name.c_str() << " already exists in the context";
#endif

        _effects.insert_or_assign(name, std::move(effect));
        return *effectPtr;
    }

    Effect *getEffect(const std::string &name) {
        auto effect = _effects.find(name);
        if (effect == _effects.end())
            return nullptr;

        return effect->second.get();
    }

    inline Camera &getCamera() { return _camera; }

    inline const Camera &getCamera() const { return _camera; }

private:
    Camera _camera;
    GLDriver _driver;
    SceneNode _root{nullptr};
    std::unordered_map<std::string, std::unique_ptr<Effect>> _effects;
};


template<>
struct NodeAction<std::unique_ptr<Drawable>> {
    static void draw(DrawContext::SceneNode &node);
};




#endif // DRAWABLES_H
