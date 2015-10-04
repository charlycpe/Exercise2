/*******************************************************************************
 *  phong.cpp
 *******************************************************************************
 *  Copyright (c) 2013 Alexandre Kaspar <alexandre.kaspar@a3.epfl.ch>
 *  For Advanced Computer Graphics, at the LGG / EPFL
 * 
 *        DO NOT REDISTRIBUTE
 ***********************************************/

#include <nori/acg.h>
#include <nori/bsdf.h>
#include <nori/frame.h>
#include "hemisampling.cpp"
#include <stdlib.h>
#include <time.h>

NORI_NAMESPACE_BEGIN

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// put your group number here!
#define GROUP_NUMBER 17
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

GROUP_NAMESPACE_BEGIN()

/**
 * \brief Phong BRDF model
 */
class Phong : public BSDF {
public:

    Phong(const PropertyList &propList) {
        m_Kd = propList.getColor("kd", Color3f(0.5f));
        m_Ks = propList.getColor("ks", Color3f(0.5f));
        m_exp = propList.getFloat("n", 20.0f);
        srand(time(NULL));
    }

    /// Reflection in local coordinates
    inline Vector3f reflect(const Vector3f &wi) const {
        return Vector3f(-wi.x(), -wi.y(), wi.z());
    }

    /// Evaluate the BRDF model

    Color3f eval(const BSDFQueryRecord &bRec) const {
        /* This is a smooth BRDF -- return zero if the measure
           is wrong, or when queried for illumination on the backside */
        if (bRec.measure != ESolidAngle
                || Frame::cosTheta(bRec.wi) <= 0
                || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.0f);

        // Based on http://www.cs.virginia.edu/~jdl/importance.doc‎

        // TODO evaluate the phong brdf for the pair of given incoming
        //      and outgoing directions bRec.wi & bRec.wo

        float alpha = bRec.wi.dot(bRec.wo);
        if (acos(alpha) > M_PI/2.0f)
        {
             alpha = cos(M_PI/2.0f);
        }

       Color3f brdf=m_Kd/M_PI+(m_Ks/(2.0f*M_PI))*(m_exp+2.0f)*pow(alpha,m_exp);
       return brdf;
    }

    /// Compute the density of \ref sample() wrt. solid angles

    float pdf(const BSDFQueryRecord &bRec) const {
        /* This is a smooth BRDF -- return zero if the measure
           is wrong, or when queried for illumination on the backside */
        if (bRec.measure != ESolidAngle
                || Frame::cosTheta(bRec.wi) <= 0
                || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;

        // TODO implement the pdf of phong importance sampling
        float pdf_diff=(1.0f/M_PI)*Frame::cosTheta(bRec.wo);
        float alpha = bRec.wo.dot(bRec.wi);
        if (acos(alpha) > M_PI/2.0f)
        {
              alpha = cos(M_PI/2.0f);
        }

        float pdf_spec=((m_exp+1.0f)/(2.0f*M_PI))*pow(alpha,m_exp);
        //we return a mixture of both pdf
        float kd=m_Kd.getLuminance();
        float ks=m_Ks.getLuminance();
        float pdf=kd*pdf_diff+ks*pdf_spec;

        return pdf;
    }

    /// Draw a a sample from the BRDF model

    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample_) const {
        Point2f sample(sample_);
        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.0f);

        float r = float(rand())/RAND_MAX;
        // 1. Select diffuse or specular
        float kd = m_Kd.getLuminance();
        float ks = m_Ks.getLuminance();
        float specSamplingWeight = ks / (ks + kd);
        bool useSpecular = true;
        if (r > specSamplingWeight) {
            useSpecular = false;
        }

        bRec.measure = ESolidAngle;
        bRec.eta = 1.0f; // no change in relative index
        
        // TODO implement phong importance sampling
        
        float theta, phi;
        if (useSpecular)
        {
               theta=acos(pow(sample.x(),1.0f/(1.0f+m_exp)));
        }
        else
        {
               theta=acos(sqrt(sample.x()));
        }
        phi=2.0f*M_PI*sample.y();
        float w_x=sin(theta)*cos(phi);
        float w_y=sin(theta)*sin(phi);
        float w_z=cos(theta);

        bRec.wo = Vector3f(w_x, w_y, w_z); // this is utterly wrong! plus maintenant?
        Color3f color=(eval(bRec)/pdf(bRec)) * Frame::cosTheta(bRec.wi);
        //cout<<color<<endl;
        return color;
    }

    /// Return a human-readable summary

    QString toString() const {
        return QString(
                "Phong[\n"
                "  Kd = %1\n"
                "  Ks = %2\n"
                "  n  = %3\n"
                "]").arg(m_Kd.toString()).arg(m_Ks.toString()).arg(m_exp);
    }

    Color3f getColor() const {
        return m_Kd;
    }

    EClassType getClassType() const {
        return EBSDF;
    }
private:
    Color3f m_Kd, m_Ks;
    float m_exp;
};

GROUP_NAMESPACE_END

NORI_REGISTER_GROUP_CLASS(Phong, "phong");
NORI_NAMESPACE_END
