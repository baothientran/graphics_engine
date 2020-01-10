#ifndef PLUGINS_H
#define PLUGINS_H

#include <QWindow>
#include <QResizeEvent>
#include <glm/glm.hpp>
#include <thread>
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
    std::optional<EffectProperty> _defaultEffectProperty;
    std::thread _meshParsingJob;
};


#endif // PLUGINS_H
