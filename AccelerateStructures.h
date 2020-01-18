#ifndef ACCELERATESTRUCTURES_H
#define ACCELERATESTRUCTURES_H

#include <array>
#include <queue>
#include "Scene.h"


class BoundingBox {
public:
    BoundingBox()
        : BoundingBox(glm::vec3(0.0f), glm::vec3(0.0f))
    {}

    BoundingBox(glm::vec3 min, glm::vec3 max)
        : _min{min}, _max{max}
    {}

    inline void setMin(glm::vec3 min) {
        _min = min;
    }

    inline void setMax(glm::vec3 max) {
        _max = max;
    }

    inline glm::vec3 getMin() const {
        return _min;
    }

    inline glm::vec3 getMax() const {
        return _max;
    }

    bool overlap(const BoundingBox &box) const {
        glm::vec3 otherMin = box.getMin();
        glm::vec3 otherMax = box.getMax();
        return overlap(_min.x, _max.x, otherMin.x, otherMax.x) &&
               overlap(_min.y, _max.y, otherMin.y, otherMax.y) &&
               overlap(_min.z, _max.z, otherMin.z, otherMax.z);
    }

private:
    bool overlap(float min1, float max1, float min2, float max2) const {
        return max1 > min2 && max2 > min1;
    }

    glm::vec3 _min;
    glm::vec3 _max;
};


template<typename Drawable>
class Octree
{
public:
    Octree(const BoundingBox &boundingBox, const std::vector<Node<Drawable> *> &nodes)
        : _parent{nullptr}, _boundingBox{boundingBox}, _pendingNodes{nodes}
    {}

    void update() {
        if (_pendingNodes.empty())
            return;

        glm::vec3 dimension = _boundingBox.getMax() - _boundingBox.getMin();
        if (dimension.x < DIM_MIN || dimension.y < DIM_MIN || dimension.z < DIM_MIN)
            return;


    }

private:
    static constexpr const unsigned DIM_MIN = 1;

    Octree *_parent;
    BoundingBox _boundingBox;
    std::array<Octree, 8> _children;
    std::vector<Node<Drawable> *> _nodes;
    std::queue<Node<Drawable> *> _pendingNodes;
};

#endif // ACCELERATESTRUCTURES_H
