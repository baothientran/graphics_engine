#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include "DrawContext.h"
#include "Effects.h"
#include "Drawables.h"
#include "BasicGeometry.h"


/***************************************************************
 * Camera definitions
 ***************************************************************/
const glm::vec3 Camera::UP_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera()
{
    setFocus(glm::vec3(0.0f));
    setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
    setViewQuat(glm::quat_cast(glm::lookAt(_position, _focus, UP_DIRECTION)));
}


void Camera::setProjMatrix(glm::mat4 projMatrix) {
    _projMatrix = projMatrix;
}


void Camera::setPosition(glm::vec3 position) {
    _position = position;
    _isDirty = true;
}


void Camera::setFocus(glm::vec3 focus) {
    _focus = focus;
    _isDirty = true;
}


void Camera::setViewQuat(glm::quat viewQuat) {
    _viewQuat = viewQuat;
    _isDirty = true;
}


glm::vec3 Camera::getFocus() const {
    return _focus;
}


glm::vec3 Camera::getPosition() const {
    return _position;
}


glm::quat Camera::getViewQuat() const {
    return _viewQuat;
}


glm::mat4 Camera::getViewMatrix() const {
    if (_isDirty) {
        _viewMatrix = glm::mat4_cast(_viewQuat);
        _viewMatrix = glm::translate(_viewMatrix, -_position - _focus);
    }

    _isDirty = false;
    return _viewMatrix;
}


glm::mat4 Camera::getProjMatrix() const {
    return _projMatrix;
}


/***************************************************************
 * EffectProperty definitions
 ***************************************************************/
EffectProperty::EffectProperty(Effect *effect, const std::map<std::string, GLUniform> &values)
    : _effect{effect}, _values{values}
{}


const GLUniform *EffectProperty::getParam(const std::string &name) const {
    auto uniform = _values.find(name);
    if (uniform == _values.end())
        return nullptr;
    return &uniform->second;
}


GLUniform *EffectProperty::getParam(const std::string &name) {
    auto uniform = _values.find(name);
    if (uniform == _values.end())
        return nullptr;
    return &uniform->second;
}


/***************************************************************
 * DrawContext definitions
 ***************************************************************/
Drawable *DrawContext::getPointLightGeometry() {
    if (!_pointLightGeometry) {
        _pointLightGeometry = createPointLightGeometry();
    }

    return _pointLightGeometry.get();
}


std::unique_ptr<Drawable> DrawContext::createPointLightGeometry() {
    unsigned longDivisons = 20;
    unsigned latDivisions = 20;
    float radius = 10.0f;

    auto effect = getEffect(ColorEffect::EFFECT_NAME);
    auto effectProperty = effect->createEffectProperty();
    effectProperty.setParam(ColorEffect::COLOR, glm::vec3(1.0f));
    auto sphere = createSphere(this,
                               std::make_shared<EffectProperty>(std::move(effectProperty)),
                               longDivisons, latDivisions, radius);

    return sphere;
}




/***************************************************
 * NodeAction definitions
 ***************************************************/
static void createDrawRequest(DrawContext::SceneNode &node, glm::mat4 transformation,
                              std::map<Effect *, std::vector<Drawable *>> &drawRequests,
                              std::vector<PointLight *> &pointLights)
{
    transformation = glm::scale(transformation, node.scale());
    transformation = glm::rotate(transformation, glm::angle(node.rotation()), glm::axis(node.rotation()));
    transformation = glm::translate(transformation, node.position());

    auto &drawable = node.getDrawable();
    if (drawable) {
        drawable->setTransformation(transformation);

        // check if it is point light
        auto pointLight = drawable->asPointLight();
        if (pointLight) {
            pointLights.push_back(pointLight);
        }

        // map drawable to effects
        auto effectProperty = drawable->getEffectProperty();
        if (effectProperty) {
            auto effect = effectProperty->getEffect();
            drawRequests[effect].push_back(drawable.get());
        }
    }

    for (auto child = node.childBegin(); child != node.childEnd(); ++child) {
        createDrawRequest(*child, transformation, drawRequests, pointLights);
    }
}


static glm::mat4 parentTransformation(DrawContext::SceneNode *node) {
    if (node == nullptr)
        return glm::mat4(1.0f);

    auto transformation = parentTransformation(node->getParent());
    transformation = glm::scale(transformation, node->scale());
    transformation = glm::rotate(transformation, glm::angle(node->rotation()), glm::axis(node->rotation()));
    transformation = glm::translate(transformation, node->position());

    return transformation;
}


void NodeAction<std::unique_ptr<Drawable>>::draw(DrawContext::SceneNode &node) {
    std::map<Effect *, std::vector<Drawable *>> drawRequest;
    std::vector<PointLight *> pointLights;
    glm::mat4 transformation = parentTransformation(node.getParent());
    createDrawRequest(node, transformation, drawRequest, pointLights);

    for (auto &request : drawRequest) {
        // reset driver
        auto effect = request.first;
        auto &driver = effect->getContext()->getDriver();

        // TODO rebind to default framebuffer
        // TODO reset viewport
        driver.setColorMask(true, true, true, true);

        driver.enableDepthTest(true);
        driver.enableDepthMask(true);
        driver.setDepthFunc(GL_LESS);
        driver.setDepthRange(0.0, 1.0);

        driver.enableStencilTest(false);
        driver.clearBufferBit(GL_STENCIL_BUFFER_BIT);
        driver.setStencilFunc(GL_ALWAYS, 0, 0xFF);
        driver.setStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        driver.enableCullFace(false);
        driver.setCullFace(GL_BACK);
        driver.setFrontFace(GL_CCW);

        driver.enableBlend(false);
        driver.setBlendEquation(GL_FUNC_ADD);

        // draw scene node
        auto &drawables = request.second;
        effect->draw(drawables, pointLights);
    }
}

