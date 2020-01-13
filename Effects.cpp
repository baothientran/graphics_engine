#include "Effects.h"
#include "Utility.h"
#include "Drawables.h"
#include <glm/gtc/matrix_access.hpp>


/***************************************************
 * ForwardPhongEffect definitions
 ***************************************************/
const std::size_t ForwardPhongEffect::PointLightUniforms::MAX = 10;

const std::string ForwardPhongEffect::EFFECT_NAME = "ForwardPhongEffect";

const std::string ForwardPhongEffect::AMBIENT_COLOR = "ambientColor";
const std::string ForwardPhongEffect::DIFFUSE_COLOR = "diffuseColor";
const std::string ForwardPhongEffect::SPECULAR_COLOR = "specularColor";
const std::string ForwardPhongEffect::SHININESS = "shininess";

const std::string ForwardPhongEffect::MVP_MAT = "modelViewProjMat";
const std::string ForwardPhongEffect::MV_MAT = "modelViewMat";
const std::string ForwardPhongEffect::NORMAL_MAT = "normalMat";
const std::string ForwardPhongEffect::LIGHT_AMBIENT = "lightAmbient";
const ForwardPhongEffect::PointLightUniforms ForwardPhongEffect::POINT_LIGHTS;


ForwardPhongEffect::ForwardPhongEffect(DrawContext *context)
    : Effect{context}
{
    auto &driver = context->getDriver();
    _program = driver.createProgram({
        {GL_VERTEX_SHADER,   readTextFile("shaders/ForwardPhongVert.glsl")},
        {GL_FRAGMENT_SHADER, readTextFile("shaders/ForwardPhongFrag.glsl")},
    });

    auto uniforms = _program->getUniforms();

    // effect wise uniforms
    _effectUniforms.insert({MVP_MAT, uniforms.at(MVP_MAT)});
    _effectUniforms.insert({MV_MAT, uniforms.at(MV_MAT)});
    _effectUniforms.insert({NORMAL_MAT, uniforms.at(NORMAL_MAT)});
    _effectUniforms.insert({LIGHT_AMBIENT, uniforms.at(LIGHT_AMBIENT)});
    for (std::size_t i = 0; i < PointLightUniforms::MAX; ++i) {
        const auto &position = POINT_LIGHTS.positions[i];
        const auto &color = POINT_LIGHTS.colors[i];
        const auto &radius = POINT_LIGHTS.radius[i];
        _effectUniforms.insert({position, uniforms.at(position)});
        _effectUniforms.insert({color, uniforms.at(color)});
        _effectUniforms.insert({radius, uniforms.at(radius)});
    }

    // drawable uniforms
    _drawableUniforms.insert({AMBIENT_COLOR, uniforms.at(AMBIENT_COLOR)});
    _drawableUniforms.insert({DIFFUSE_COLOR, uniforms.at(DIFFUSE_COLOR)});
    _drawableUniforms.insert({SPECULAR_COLOR, uniforms.at(SPECULAR_COLOR)});
    _drawableUniforms.insert({SHININESS, uniforms.at(SHININESS)});

#ifndef NDEBUG
    if (_drawableUniforms.size() + _effectUniforms.size() != uniforms.size()) {
        qDebug() << "Drawable uniforms and effect uniforms does not make up all shader uniforms in ForwardPhongEffect";
    }
#endif

    _attributes = _program->getAttributes();
}


const std::map<std::string, int> &ForwardPhongEffect::getAttributes() const {
    return _attributes;
}


EffectProperty ForwardPhongEffect::createEffectProperty() {
    return {this, _drawableUniforms};
}


void ForwardPhongEffect::draw(const std::vector<Drawable *> &drawables,
                              const std::vector<PointLight *> &pointLights) {
    _program->bind();
    const auto &camera = _context->getCamera();

    // draw ambient and directional light. TODO: hardcode for now
    _effectUniforms.at(LIGHT_AMBIENT).setValue(glm::vec3(0.2f));

    // draw point lights
    for (std::size_t i = 0; i < std::min(PointLightUniforms::MAX, pointLights.size()); ++i) {
        auto pointLight = pointLights[i];
        glm::vec4 lightPosition = glm::column(pointLight->getTransformation(), 3);
        glm::vec4 lightViewPosition = camera.getViewMatrix() * lightPosition;
        _effectUniforms.at(POINT_LIGHTS.positions[i]).setValue(lightViewPosition.xyz());
        _effectUniforms.at(POINT_LIGHTS.colors[i]).setValue(pointLight->getLightColor());
        _effectUniforms.at(POINT_LIGHTS.radius[i]).setValue(pointLight->getRadius());
    }

    // draw drawables
    for (auto drawable : drawables) {
        // apply transformation
        glm::mat4 mv = camera.getViewMatrix() * drawable->getTransformation();
        glm::mat4 mvp = camera.getProjMatrix() * mv;
        glm::mat4 normalMat = glm::inverse(glm::transpose(mv));
        _effectUniforms.at(MVP_MAT).setValue(mvp);
        _effectUniforms.at(MV_MAT).setValue(mv);
        _effectUniforms.at(NORMAL_MAT).setValue(normalMat);

        // apply effectwise uniforms
        for (const auto &uniform : _effectUniforms) {
            _program->applyUniform(uniform.second);
        }

        // apply individual uniforms
        auto effectProperty = drawable->getEffectProperty();
        for (const auto &uniform : *effectProperty) {
            _program->applyUniform(uniform.second);
        }

        // draw
        drawable->draw();
    }

    _program->unbind();
}
