#pragma once
#include "Core/MathDefinitions.h"

struct WaterPlaneData {
    Vect3f m_waterColorOne;
    float m_metallic;
    Vect3f m_waterColorTwo;
    float m_roughness;

    Vect2f m_waterDirectionOne;
    Vect2f m_waterDirectionTwo;

    float m_speed;
    float m_noiseScale;
    float m_heightScale;
    float m_textureScale;

    Vect3f m_edgeColor;
    float  m_far;
    
    float  m_near;
    float  m_edgeScale;
};