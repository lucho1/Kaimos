#include "kspch.h"
#include "Light.h"
#include "Core/Utils/Maths/Maths.h"

namespace Kaimos {

	// ----------------------- Preset Values --------------------------------------------------------------
	static const std::vector<float> LightDistanceValues = { 7.0f, 13.0f, 20.0f, 32.0f, 50.0f, 65.0f, 100.0f, 160.0f, 200.0f, 325.0f, 600.0f, 3250.0f };
	static const float LightAttenuationValues[2][12] = {{ 0.7f, 0.35f, 0.22f, 0.14f, 0.09f, 0.07f, 0.045f, 0.027f, 0.022f, 0.014f, 0.007f, 0.0014f },
														{ 1.8f, 0.44f, 0.2f, 0.07f, 0.032f, 0.017f, 0.0075f, 0.0028f, 0.0019f, 0.0007f, 0.0002f, 0.000007f }};



	// ----------------------- Light Methods ---------------------------------------------------
	void PointLight::SetMinRadius(float radius)
	{
		m_MinRadius = radius;
		CalculateAttenuationValues();
	}

	void PointLight::CalculateAttenuationValues()
	{
		uint index = 0;
		if(m_MinRadius >= LightDistanceValues[1])
		{
			for (uint i = 11; i >= 0; --i)
			{
				if (m_MinRadius > LightDistanceValues[i])
				{
					index = i;
					break;
				}
			}
		}

		m_AttenuationLinearFactor = LightAttenuationValues[0][index];
		m_AttenuationQuadraticFactor = LightAttenuationValues[1][index];
	}
}
