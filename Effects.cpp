#include "Effects.h"
#include "Utility.h"


/***************************************************
 * ForwardPhongEffect definitions
 ***************************************************/
const std::string ForwardPhongEffect::EFFECT_NAME = "ForwardPhongEffect";

const std::string ForwardPhongEffect::DIFFUSE_COLOR = "diffuseColor";
const std::string ForwardPhongEffect::SPECULAR_COLOR = "specularColor";
const std::string ForwardPhongEffect::DIFFUSE_REFLECTION = "diffuseReflection";
const std::string ForwardPhongEffect::SPECULAR_REFLECTION = "specularReflection";
const std::string ForwardPhongEffect::SHININESS = "shininess";

const std::string ForwardPhongEffect::MVP_MAT = "modelViewProjMat";
const std::string ForwardPhongEffect::MV_MAT = "modelViewMat";
const std::string ForwardPhongEffect::NORMAL_MAT = "normalMat";
const std::string ForwardPhongEffect::LIGHT_POSITION = "lightPosition";
const std::string ForwardPhongEffect::LIGHT_COLOR = "lightColor";
const std::string ForwardPhongEffect::LIGHT_AMBIENT = "lightAmbient";
const std::string ForwardPhongEffect::LIGHT_RADIUS = "lightRadius";

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
    _effectUniforms.insert({LIGHT_POSITION, uniforms.at(LIGHT_POSITION)});
    _effectUniforms.insert({LIGHT_COLOR, uniforms.at(LIGHT_COLOR)});
    _effectUniforms.insert({LIGHT_AMBIENT, uniforms.at(LIGHT_AMBIENT)});
    _effectUniforms.insert({LIGHT_RADIUS, uniforms.at(LIGHT_RADIUS)});

    // drawable uniforms
    _drawableUniforms.insert({DIFFUSE_COLOR, uniforms.at(DIFFUSE_COLOR)});
    _drawableUniforms.insert({SPECULAR_COLOR, uniforms.at(SPECULAR_COLOR)});
    _drawableUniforms.insert({DIFFUSE_REFLECTION, uniforms.at(DIFFUSE_REFLECTION)});
    _drawableUniforms.insert({SPECULAR_REFLECTION, uniforms.at(SPECULAR_REFLECTION)});
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


void ForwardPhongEffect::draw(const std::vector<Drawable *> &drawables) {
    _program->bind();

    const auto &camera = _context->getCamera();
    for (auto drawable : drawables) {
        // apply transformation
        glm::mat4 mv = camera.getViewMatrix() * drawable->getTransformation();
        glm::mat4 mvp = camera.getProjMatrix() * mv;
        glm::mat4 normalMat = glm::transpose(glm::inverse(mv));
        glm::vec4 lightViewPosition = camera.getViewMatrix() * glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
        _effectUniforms.at(MVP_MAT).setValue(mvp);
        _effectUniforms.at(MV_MAT).setValue(mv);
        _effectUniforms.at(NORMAL_MAT).setValue(normalMat);
        _effectUniforms.at(LIGHT_POSITION).setValue(lightViewPosition.xyz());
        _effectUniforms.at(LIGHT_COLOR).setValue(glm::vec3(12.0f));
        _effectUniforms.at(LIGHT_AMBIENT).setValue(glm::vec3(0.5f));
        _effectUniforms.at(LIGHT_RADIUS).setValue(3.55f);

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
