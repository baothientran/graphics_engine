#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QMimeData>
#include "Viewer.h"
#include "Effects.h"

/***************************************************
 * OrbitCameraPlugin definitions
 ***************************************************/
OrbitCameraPlugin::OrbitCameraPlugin(Viewer *viewer)
    : ViewerPlugin{viewer}
{
    connect(_viewer, &Viewer::onMouseMoveEvent, this, &OrbitCameraPlugin::mouseMoveEvent);
    connect(_viewer, &Viewer::onMousePressEvent, this, &OrbitCameraPlugin::mousePressEvent);
    connect(_viewer, &Viewer::onMouseReleaseEvent, this, &OrbitCameraPlugin::mouseReleaseEvent);
    connect(_viewer, &Viewer::onWheelEvent, this, &OrbitCameraPlugin::wheelEvent);
}


void OrbitCameraPlugin::mouseMoveEvent(QMouseEvent *event) {
    if (!_isMousePress)
        return;

    auto pos = event->pos();
    auto currMousePos = glm::vec2{pos.x(), pos.y()};
    if (currMousePos == _prevMousePos)
        return;

    auto &context = _viewer->getDrawContext();
    {
        std::scoped_lock lock(context.mutex);

        auto &camera = context.getCamera();
        auto orientation = glm::mat3(camera.getViewMatrix());
        auto viewDir = glm::normalize(glm::row(orientation, 2));
        auto upDir = Camera::UP_DIRECTION;
        glm::vec3 xaxis = glm::cross(viewDir, upDir);
        if (glm::dot(xaxis, glm::row(orientation, 0)) < 0)
            xaxis = -xaxis;

        auto rotateX = glm::angleAxis(0.007f  * (currMousePos.y - _prevMousePos.y), normalize(xaxis));
        auto rotateY = glm::angleAxis(0.007f  * (currMousePos.x - _prevMousePos.x), normalize(upDir));
        auto viewQuat = camera.getViewQuat() * rotateX * rotateY;
        auto newCamPos = camera.getPosition() * rotateX * rotateY;
        camera.setViewQuat(glm::normalize(viewQuat));
        camera.setPosition(newCamPos);
    }

    // reset mouse position when leaving the window
    bool boundary = false;
    if (currMousePos.x < 0) {
        currMousePos.x = _viewer->width();
        boundary = true;
    }
    if (currMousePos.x > _viewer->width()) {
        currMousePos.x = 0.0f;
        boundary = true;
    }

    if (currMousePos.y < 0) {
        currMousePos.y = _viewer->height();
        boundary = true;
    }
    if (currMousePos.y > _viewer->height()) {
        currMousePos.y = 0;
        boundary = true;
    }

    if (boundary) {
        QPoint absoluteCurrMouse = _viewer->mapToGlobal(QPoint(currMousePos.x, currMousePos.y));
        QCursor::setPos(absoluteCurrMouse);
    }

    _prevMousePos = currMousePos;

    _viewer->renderLater();
}


void OrbitCameraPlugin::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::MidButton) {
        auto mousePos = event->pos();
        _isMousePress = true;
        _prevMousePos = glm::vec2(mousePos.x(), mousePos.y());
    }
}


void OrbitCameraPlugin::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::MidButton) {
        _isMousePress = false;
    }
}


void OrbitCameraPlugin::wheelEvent(QWheelEvent *event) {
    auto &context = _viewer->getDrawContext();
    {
        std::scoped_lock lock(context.mutex);

        auto &camera = context.getCamera();
        auto camPos = camera.getPosition();
        auto scale = glm::abs(glm::length(camPos - camera.getFocus())) / 25.0f + 1.0f;
        auto orientation = glm::mat3(camera.getViewMatrix());
        auto camDir = glm::normalize(-glm::row(orientation, 2));
        auto newPos = camPos + camDir * scale * 0.007f * static_cast<float>(event->delta());
        camera.setPosition(newPos);
    }

    _viewer->renderLater();
}


/***************************************************
 * PerspectiveCameraPlugin definitions
 ***************************************************/
PerspectiveCameraPlugin::PerspectiveCameraPlugin(Viewer *viewer)
    : ViewerPlugin{viewer}
{
    connect(_viewer, &Viewer::onResizeEvent, this, &PerspectiveCameraPlugin::resizeEvent);

    // initialize camera projection
    auto &context = _viewer->getDrawContext();
    setCameraProjMatrix(context);
}


void PerspectiveCameraPlugin::resizeEvent(QResizeEvent *) {
    auto &context = _viewer->getDrawContext();
    std::scoped_lock lock(context.mutex);
    setCameraProjMatrix(context);

    _viewer->renderLater();
}


void PerspectiveCameraPlugin::setCameraProjMatrix(DrawContext &context) {
    auto &camera = context.getCamera();
    camera.setProjMatrix(glm::perspective(glm::radians(CAM_FOV), static_cast<float>(_viewer->width()) / _viewer->height(), CAM_NEAR, CAM_FAR));
}


/***************************************************
 * ImportMeshFilePlugin definitions
 ***************************************************/
ImportMeshFilePlugin::ImportMeshFilePlugin(Viewer *viewer)
    : ViewerPlugin{viewer}
{
    connect(_viewer, &Viewer::onDragEnterEvent, this, &ImportMeshFilePlugin::dragEnterEvent);
    connect(_viewer, &Viewer::onDropEvent, this, &ImportMeshFilePlugin::dropEvent);

    // set default phong property for any imported mesh
    auto &context = _viewer->getDrawContext();
    auto forwardPhongEffect = context.getEffect(ForwardPhongEffect::EFFECT_NAME);
    _defaultEffectProperty = forwardPhongEffect->createEffectProperty();
    _defaultEffectProperty->setParam(ForwardPhongEffect::DIFFUSE_COLOR, glm::vec3(1.0f));
    _defaultEffectProperty->setParam(ForwardPhongEffect::SPECULAR_COLOR, glm::vec3(1.0f));
    _defaultEffectProperty->setParam(ForwardPhongEffect::DIFFUSE_REFLECTION, 1.0f);
    _defaultEffectProperty->setParam(ForwardPhongEffect::SPECULAR_REFLECTION, 1.0f);
    _defaultEffectProperty->setParam(ForwardPhongEffect::SHININESS, 32.0f);
}


void ImportMeshFilePlugin::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}


void ImportMeshFilePlugin::dropEvent(QDropEvent *event) {
    auto urls = event->mimeData()->urls();
    for (const auto &url : urls) {
        if (url.path().endsWith(".obj")) {
            // parse OBJ file
        }
    }
}
