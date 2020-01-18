#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QMimeData>
#include <unordered_set>
#include "Viewer.h"
#include "Effects.h"
#include "Utility.h"

/***************************************************
 * OrbitCameraPlugin definitions
 ***************************************************/
OrbitCameraPlugin::OrbitCameraPlugin(Viewer *viewer)
    : ViewerPlugin{viewer}, _prevMousePos{glm::vec2(0.0f)}, _isMousePress{false}
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
    auto &camera = context.getCamera();
    auto camPos = camera.getPosition();
    auto scale = glm::abs(glm::length(camPos - camera.getFocus())) / 25.0f + 1.0f;
    auto orientation = glm::mat3(camera.getViewMatrix());
    auto camDir = glm::normalize(-glm::row(orientation, 2));
    auto newPos = camPos + camDir * scale * 0.007f * static_cast<float>(event->delta());
    camera.setPosition(newPos);

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

    auto &context = _viewer->getDrawContext();
    auto forwardPhongEffect = context.getEffect(ForwardPhongEffect::EFFECT_NAME);
    _defaultEffectProperty = std::make_shared<EffectProperty>(forwardPhongEffect->createEffectProperty());
    _defaultEffectProperty->setParam(ForwardPhongEffect::AMBIENT_COLOR, glm::vec3(1.0f));
    _defaultEffectProperty->setParam(ForwardPhongEffect::DIFFUSE_COLOR, glm::vec3(1.0f));
    _defaultEffectProperty->setParam(ForwardPhongEffect::SPECULAR_COLOR, glm::vec3(1.0f));
    _defaultEffectProperty->setParam(ForwardPhongEffect::SHININESS, 32.0f);

}


void ImportMeshFilePlugin::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}


void ImportMeshFilePlugin::dropEvent(QDropEvent *event) {
    auto urls = event->mimeData()->urls();
    for (const auto &url : urls) {
        if (url.isLocalFile() && url.path().endsWith(".obj") ) {
            loadMeshFile(url.path().toStdString());
        }
    }

    _viewer->renderLater();
}


void ImportMeshFilePlugin::loadMeshFile(const std::string &file) {
    std::string baseDir = getBaseDir(file);
    if (baseDir.empty())
        baseDir = ".";
#ifdef _WIN32
    baseDir += "\\";
#else
    baseDir += "/";
#endif

    std::string warn;
    std::string error;
    tinyobj::attrib_t attrib_t;
    std::vector<tinyobj::shape_t> shape_ts;
    std::vector<tinyobj::material_t> material_ts;
    bool ret = tinyobj::LoadObj(&attrib_t,
                                &shape_ts,
                                &material_ts,
                                &warn,
                                &error,
                                file.c_str(),
                                baseDir.c_str(),
                                true,
                                true);

    if (!ret) {
        // TODO: display error message here
        return;
    }

    // clear existing mesh for now. TODO: develop UI so user will do it themselves
    auto &context = _viewer->getDrawContext();
    auto &rootNode = context.getRoot();
    auto it = rootNode.childBegin();
    while (it != rootNode.childEnd()) {
        auto &drawable = it->getDrawable();
        if (drawable->asPointLight()) {
            ++it;
        }
        else
            it = rootNode.removeChild(it);
    }

    // add imported mesh to the scene
    std::vector<std::shared_ptr<EffectProperty>> effectProperties;
    for (const auto &material_t : material_ts) {
        std::shared_ptr<EffectProperty> effectProperty = processMaterial(material_t);
        effectProperties.push_back(std::move(effectProperty));
    }

    for (const auto &shape_t : shape_ts) {
        processShape(shape_t, attrib_t, effectProperties);
    }
}


std::string ImportMeshFilePlugin::getBaseDir(const std::string &file) {
    auto pos = file.find_last_of("\\/");
    if (pos != std::string::npos)
        return file.substr(0, pos);

    return "";
}


std::shared_ptr<EffectProperty> ImportMeshFilePlugin::processMaterial(const tinyobj::material_t &material_t) {
    glm::vec3 ambientColor = glm::vec3(material_t.ambient[0], material_t.ambient[1], material_t.ambient[2]);
    glm::vec3 diffuseColor = glm::vec3(material_t.diffuse[0], material_t.diffuse[1], material_t.diffuse[2]);
    glm::vec3 specularColor = glm::vec3(material_t.specular[0], material_t.specular[1], material_t.specular[2]);
    float shininess = equals(material_t.shininess, 0.0f) ? 1.0f : material_t.shininess;

    // set phong property for imported mesh
    auto &context = _viewer->getDrawContext();
    auto forwardPhongEffect = context.getEffect(ForwardPhongEffect::EFFECT_NAME);
    auto effectProperty = forwardPhongEffect->createEffectProperty();
    effectProperty.setParam(ForwardPhongEffect::AMBIENT_COLOR, ambientColor);
    effectProperty.setParam(ForwardPhongEffect::DIFFUSE_COLOR, diffuseColor);
    effectProperty.setParam(ForwardPhongEffect::SPECULAR_COLOR, specularColor);
    effectProperty.setParam(ForwardPhongEffect::SHININESS, shininess);

    return std::make_shared<EffectProperty>(std::move(effectProperty));
}


void ImportMeshFilePlugin::processShape(const tinyobj::shape_t &shape_t,
                                        const tinyobj::attrib_t &attrib_t,
                                        const std::vector<std::shared_ptr<EffectProperty>> &effectProperties)
{
    // find the size of position, color, normal, and texCoord
    unsigned numOfElement = 0;
    std::unordered_map<int, unsigned> posToElem;
    std::unordered_set<int> uniqueNormal, uniqueTexCoord;
    for (const auto &index_t : shape_t.mesh.indices) {
        if (index_t.vertex_index >= 0 &&  (posToElem.find(index_t.vertex_index) == posToElem.end()))
            posToElem.insert({index_t.vertex_index, numOfElement++});

        if (index_t.normal_index >= 0)
            uniqueNormal.insert(index_t.normal_index);

        if (index_t.texcoord_index >= 0)
            uniqueTexCoord.insert(index_t.texcoord_index);
    }

    // find the position, color, normal, and texCoord
    std::vector<unsigned> elements;
    std::vector<glm::vec3> positions(posToElem.size());
    std::vector<glm::vec3> normals(posToElem.size());
    std::vector<glm::vec2> texCoords;
    if (uniqueTexCoord.size() > 0)
        texCoords.resize(posToElem.size());

    std::size_t indexOffset = 0;
    for (auto vertPerFace : shape_t.mesh.num_face_vertices) {
        for (auto i = indexOffset; i < indexOffset + vertPerFace; ++i) {
            auto index_t = shape_t.mesh.indices[i];

            auto element = posToElem.at(index_t.vertex_index);
            elements.push_back(element);

            // position
            positions[element] = retrievePositionAttrib_t(attrib_t, index_t);

            // normal
            if (uniqueNormal.size() > 0)
                normals[element] += retrieveNormalAttrib_t(attrib_t, index_t);
            else
                normals[element] += calcSurfaceNormal(attrib_t, shape_t, indexOffset);

            // texture coordinate
            if (uniqueTexCoord.size() > 0) {
                texCoords[element] = retrieveTexCoordAttrib_t(attrib_t, index_t);
            }
        }

        indexOffset += vertPerFace;
    }

    std::for_each(normals.begin(), normals.end(), [](glm::vec3 &n){
        if (!equals(glm::length(n), 0.0f))
            n = glm::normalize(n);
    });


    // create VAO and Buffer
    int positionCount = static_cast<int>(positions.size() * sizeof(glm::vec3));
    int normalCount = static_cast<int>(normals.size() * sizeof(glm::vec3));
    int positionOffset = -1;
    int normalOffset = -1;

    auto &context = _viewer->getDrawContext();
    GLVertexArray vao = context.getDriver().createVertexArray(elements.data(), elements.size(), GL_STATIC_DRAW);
    GLBuffer buffer = context.getDriver().createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    buffer.bind();
    buffer.loadData(nullptr, positionCount + normalCount);
    if (!positions.empty()) {
        positionOffset = 0;
        buffer.loadSubData(positionOffset, positions.data(), positionCount);
    }

    if (!normals.empty()) {
        normalOffset = positionCount;
        buffer.loadSubData(normalOffset, normals.data(), normalCount);
    }


    // create geometries
    std::shared_ptr<GLVertexArray> vaoptr = std::make_shared<GLVertexArray>(std::move(vao));
    std::shared_ptr<GLBuffer> bufferptr = std::make_shared<GLBuffer>(std::move(buffer));
    auto &rootNode = context.getRoot();
    const auto &materials_ids = shape_t.mesh.material_ids;
    std::size_t idx = 0;
    while (idx < materials_ids.size()) {
        std::size_t right = idx+1;
        while(right < materials_ids.size() && materials_ids[right] == materials_ids[idx]) {
            ++right;
        }

        std::shared_ptr<EffectProperty> effectProperty;
        if (materials_ids[idx] >= 0) {
            effectProperty = effectProperties[materials_ids[idx]];
        }
        else {
            effectProperty = _defaultEffectProperty;
        }

        auto drawable = context.createDrawable<Geometry>(std::move(effectProperty),
                                                         vaoptr,
                                                         bufferptr,
                                                         (right - idx) * 3,
                                                         idx * 3 * sizeof(unsigned),
                                                         positionOffset,
                                                         normalOffset);

        drawable->setName(shape_t.name + "_mat" + std::to_string(idx));

        rootNode.createChild(std::move(drawable));
        idx = right;
    }
}


glm::vec3 ImportMeshFilePlugin::calcSurfaceNormal(const tinyobj::attrib_t &attrib_t, const tinyobj::shape_t &shape_t, std::size_t beginPoint) {
    auto beginIdx = shape_t.mesh.indices[beginPoint];
    auto secIdx = shape_t.mesh.indices[beginPoint+1];
    auto lastIdx = shape_t.mesh.indices[beginPoint+2];

    glm::vec3 p1 = retrievePositionAttrib_t(attrib_t, beginIdx);
    glm::vec3 p2 = retrievePositionAttrib_t(attrib_t, secIdx);
    glm::vec3 p3 = retrievePositionAttrib_t(attrib_t, lastIdx);
    glm::vec3 normal = glm::cross(p2-p1, p3-p1);

    if (!equals(glm::length(normal), 0.0f))
        return glm::normalize(normal);

    return normal;
}


glm::vec3 ImportMeshFilePlugin::retrievePositionAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx) {
    auto px = attrib_t.vertices[3 * idx.vertex_index + 0];
    auto py = attrib_t.vertices[3 * idx.vertex_index + 1];
    auto pz = attrib_t.vertices[3 * idx.vertex_index + 2];

    return {px, py, pz};
}


glm::vec3 ImportMeshFilePlugin::retrieveNormalAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx) {
    auto nx = attrib_t.normals[3 * idx.normal_index + 0];
    auto ny = attrib_t.normals[3 * idx.normal_index + 1];
    auto nz = attrib_t.normals[3 * idx.normal_index + 2];

    return {nx, ny, nz};
}


glm::vec2 ImportMeshFilePlugin::retrieveTexCoordAttrib_t(const tinyobj::attrib_t &attrib_t, tinyobj::index_t idx) {
    auto tx = attrib_t.texcoords[2 * idx.texcoord_index + 0];
    auto ty = attrib_t.texcoords[2 * idx.texcoord_index + 1];

    return {tx, ty};
}
