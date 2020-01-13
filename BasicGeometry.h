#ifndef BASICGEOMETRY_H
#define BASICGEOMETRY_H

#include "Drawables.h"


std::unique_ptr<Drawable> createSphere(DrawContext *context,
                                       std::shared_ptr<EffectProperty> effectProperty,
                                       unsigned longDivisions, unsigned latDivisions, float radius);

#endif // BASICGEOMETRY_H
