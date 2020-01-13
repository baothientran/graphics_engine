#include "BasicGeometry.h"


/********************************************************
 * helper to generate sphere mesh
 ********************************************************/
static std::vector<glm::vec3> createSpherePositions(unsigned longDivisions, unsigned latDivisions, float radius) {
    std::vector<glm::vec3> positions;
    float long_inc = 360.0f / longDivisions;
    float lat_inc  = 180.0f / latDivisions;

    float top_x = radius * glm::cos(0.0f) * glm::sin(0.0f);
    float top_z = radius * glm::sin(0.0f) * glm::sin(0.0f);
    float top_y = radius * glm::cos(0.0f);
    positions.push_back({top_x, top_y, top_z});

    for (unsigned lat_div = 1; lat_div < latDivisions; ++lat_div) {
        for (unsigned long_div = 0; long_div < longDivisions; ++long_div) {
            float long_angle = glm::radians(long_div * long_inc);
            float lat_angle  = glm::radians(lat_div * lat_inc);
            float x = radius * glm::cos(long_angle) * glm::sin(lat_angle);
            float z = radius * glm::sin(long_angle) * glm::sin(lat_angle);
            float y = radius * glm::cos(lat_angle);
            positions.push_back({x, y, z});
        }
    }

    float bottom_x = radius * glm::cos(glm::pi<float>() * 2.0f) * glm::sin(glm::pi<float>());
    float bottom_z = radius * glm::sin(glm::pi<float>() * 2.0f) * glm::sin(glm::pi<float>());
    float bottom_y = radius * glm::cos(glm::pi<float>());
    positions.push_back({bottom_x, bottom_y, bottom_z});

    return positions;
}


static std::vector<unsigned> createSphereElements(unsigned longDivisions, unsigned latDivisions) {
    std::vector<unsigned> elements;

    // connect top
    for (unsigned long_div = 0; long_div < longDivisions; ++long_div) {
        elements.push_back(0);
        elements.push_back(1 + (long_div+1) % longDivisions);
        elements.push_back(1 + long_div);
    }

    // connect body
    for (unsigned lat_div = 1; lat_div < latDivisions-1; ++lat_div) {
        for (unsigned long_div = 0; long_div < longDivisions; ++long_div) {
            unsigned current           = (lat_div-1) * longDivisions + long_div + 1;
            unsigned next              = (lat_div-1) * longDivisions + (long_div+1) % longDivisions + 1;
            unsigned down_current      = lat_div * longDivisions + long_div + 1;
            unsigned down_current_next = lat_div * longDivisions + (long_div+1) % longDivisions + 1;

            elements.push_back(current);
            elements.push_back(next);
            elements.push_back(down_current);

            elements.push_back(next);
            elements.push_back(down_current_next);
            elements.push_back(down_current);
        }
    }

    // connect bottom
    unsigned bottom = 1 + (latDivisions - 1) * longDivisions;
    for (unsigned long_div = 0; long_div < longDivisions; ++long_div) {
        unsigned current = (latDivisions-2) * longDivisions + long_div + 1;
        unsigned next    = (latDivisions-2) * longDivisions + (long_div+1) % longDivisions + 1;
        elements.push_back(current);
        elements.push_back(next);
        elements.push_back(bottom);
    }

    return elements;
}


static std::vector<glm::vec3> createSphereNormals(const std::vector<glm::vec3> &positions, const std::vector<unsigned> &elements) {
    std::vector<glm::vec3> normals;
    normals.resize(positions.size(), glm::vec3{});
    for (unsigned i = 0; i < elements.size(); i+=3) {
        auto elem0 = elements[i];
        auto elem1 = elements[i+1];
        auto elem2 = elements[i+2];
        auto pos0  = positions[elem0];
        auto pos1  = positions[elem1];
        auto pos2  = positions[elem2];

        auto n = glm::normalize(glm::cross(pos1 - pos0, pos2 - pos1));
        normals[elem0] += n;
        normals[elem1] += n;
        normals[elem2] += n;
    }

    std::for_each(normals.begin(), normals.end(), [&](auto &normal){
        if (glm::length(normal) != 0.0f)
            normal = glm::normalize(normal);
    });

    return normals;
}


std::unique_ptr<Drawable> createSphere(DrawContext *context,
                                       std::shared_ptr<EffectProperty> effectProperty,
                                       unsigned longDivisions, unsigned latDivisions, float radius)
{
    auto positions = createSpherePositions(longDivisions, latDivisions, radius);
    auto elements  = createSphereElements(longDivisions, latDivisions);
    auto normals   = createSphereNormals(positions, elements);

    auto sphere = context->createDrawable<Geometry>(
        std::move(effectProperty),
        elements,
        positions,
        normals);

    return std::move(sphere);
}

