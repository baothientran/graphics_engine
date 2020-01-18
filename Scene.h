#ifndef SCENE_H
#define SCENE_H

#include <variant>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <QObject>
#include <GLDriver.h>


/***************************************************************
 * BasicNode definitions
 ***************************************************************/
template<typename T>
class NodeAction;

template <class T>
class Node {
public:
    using Iterator = typename std::vector<Node>::iterator;
    using ConstIterator = typename std::vector<Node>::const_iterator;

    explicit Node()
        : Node(T{})
    {}

    explicit Node(T drawable)
        : _position{0.0f}, _scale{1.0f}, _rotation{1.0f, 0.0f, 0.0f, 0.0f}, _drawable{std::move(drawable)}, _parent{nullptr}
    {}

    Node(const Node &other) = delete;

    Node(Node &&other) = default;

    Node &operator=(const Node &other) = delete;

    Node &operator=(Node &&other) noexcept {
        Node(std::move(other)).swap(*this);
        return *this;
    }

    void swap(Node &other) noexcept {
        using std::swap;
        swap(_drawable, other._drawable);
        swap(_parent, other._parent);
        swap(_children, other._children);
        swap(_rotation, other._rotation);
        swap(_position, other._position);
        swap(_scale, other._scale);
    }

    inline T &getDrawable() { return _drawable; }

    inline const T &getDrawable() const { return _drawable; }

    inline glm::vec3 position() const { return _position; }

    inline glm::vec3 scale() const { return _scale; }

    inline glm::quat rotation() const { return _rotation; }

    inline glm::vec3 &position() { return _position; }

    inline glm::vec3 &scale() { return _scale; }

    inline glm::quat &rotation() { return _rotation; }

    inline void setParent(Node *parent) {
        _parent = parent;
    }

    inline Node *getParent() { return _parent; }

    inline const Node *getParent() const { return _parent; }

    inline Iterator childBegin() { return _children.begin(); }

    inline ConstIterator childBegin() const { return _children.begin(); }

    inline Iterator childEnd() { return _children.end(); }

    inline ConstIterator childEnd() const { return _children.end(); }

    inline Node &createChild(T drawable) {
        auto &newlyAdded = _children.emplace_back(std::move(drawable));
        newlyAdded.setParent(this);
        return newlyAdded;
    }

    inline Iterator removeChild(Iterator pos) {
        return _children.erase(pos);
    }

    inline ConstIterator removeChild(ConstIterator pos) {
        return _children.erase(pos);
    }

    inline void clearChild() {
        _children.clear();
    }

    template<typename... Args>
    void draw(Args&&... args) {
        NodeAction<T>::draw(*this, std::forward<Args>(args)...);
    }

private:
    glm::vec3 _position;
    glm::vec3 _scale;
    glm::quat _rotation;
    T _drawable;
    std::vector<Node> _children;
    Node *_parent;
};

#endif // SCENE_H
