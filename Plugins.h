#ifndef PLUGINS_H
#define PLUGINS_H

#include <QWindow>
#include <QResizeEvent>
#include <glm/glm.hpp>
#include "tiny_obj_loader.h"
#include "DrawContext.h"

class Viewer;

class ViewerPlugin : public QObject {
    Q_OBJECT

public:
    ViewerPlugin(Viewer *viewer)
        : QObject{}, _viewer{viewer}
    {}

    virtual ~ViewerPlugin() = default;

protected:
    Viewer *_viewer;
};


class OrbitCameraPlugin : public ViewerPlugin {
    Q_OBJECT

public:
    OrbitCameraPlugin(Viewer *viewer);

public slots:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

private:
    glm::vec2 _prevMousePos;
    bool _isMousePress;
};


class PerspectiveCameraPlugin : public ViewerPlugin {
    Q_OBJECT

public:
    PerspectiveCameraPlugin(Viewer *viewer);

public slots:
    void resizeEvent(QResizeEvent *event);

private:
    void setCameraProjMatrix(DrawContext &context);

    const float CAM_FOV = 45.0f;
    const float CAM_NEAR = 0.1f;
    const float CAM_FAR = 10000.0f;
};


class ImportMeshFilePlugin : public ViewerPlugin {
    Q_OBJECT

public:
    ImportMeshFilePlugin(Viewer *viewer);

public slots:
    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);

private:
    void loadMeshFile(const std::string &file);

    std::string getBaseDir(const std::string &file);

    void processShape(const tinyobj::shape_t &shape_t,
                      const tinyobj::attrib_t &attrib_t,
                      const std::vector<std::shared_ptr<EffectProperty>> &effectProperties);

    std::shared_ptr<EffectProperty> processMaterial(const tinyobj::material_t &material_t);

    glm::vec3 calcSurfaceNormal(const tinyobj::attrib_t &attrib_t, const tinyobj::shape_t &shape_t, std::size_t beginPoint);

    glm::vec3 retrievePositionAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx);

    glm::vec3 retrieveNormalAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx);

    glm::vec2 retrieveTexCoordAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx);

    std::string correctTexturePath(std::string filePath);

    std::shared_ptr<EffectProperty> _defaultEffectProperty;
};


#endif // PLUGINS_H
