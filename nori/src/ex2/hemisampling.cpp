/*******************************************************************************
 *  hemisampling.cpp
 *******************************************************************************
 *  Copyright (c) 2013 Alexandre Kaspar <alexandre.kaspar@a3.epfl.ch>
 *  For Advanced Computer Graphics, at the LGG / EPFL
 * 
 *        DO NOT REDISTRIBUTE
 ***********************************************/

#include <nori/vector.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Cosine hemisphere sampling
 * 
 * \param sample a 2d uniform sample
 */
inline Vector3f squareToCosineHemisphere(const Point2f &sample) {

    float r = sqrtf(sample.x());
    float sinThe, cosThe;
    sincosf(2.0f * M_PI * sample.y(), &sinThe, &cosThe);

    return Vector3f(r*cosThe, r*sinThe, sqrtf(1.0f - sample.x()));
}

NORI_NAMESPACE_END
