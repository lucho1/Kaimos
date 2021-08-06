#ifndef _LIGHT_H_
#define _LIGHT_H_

namespace Kaimos {

	enum class LightType { DIRECTIONAL = 0, POINTLIGHT };

	class Light
	{
	public:

		// --- Public Class Methods ---
		Light(LightType type = LightType::DIRECTIONAL) : m_Type(type) {}
		~Light() = default;

		LightType GetLightType() const { return m_Type; }
		void SetLightType(LightType type) { m_Type = type; }

	public:

		// --- Public Variables ---
		glm::vec4 Radiance = glm::vec4(1.0f); // Color
		float Intensity = 1.0f;

	protected:

		// --- Private Variables ---
		LightType m_Type = LightType::DIRECTIONAL;
	};



	class PointLight : public Light
	{
	public:

		// --- Public Class Methods ---
		PointLight() : Light(LightType::POINTLIGHT) {}
		~PointLight() = default;

	public:

		// --- Setters/Getters ---
		float GetRadius() const { return m_Radius; }
		void SetRadius(float radius);

	private:

		// --- Private Lights Methods ---
		void CalculateAttenuationValues();

	public:

		// --- Public Variables ---
		float FalloffMultiplier = 1.0f;

	private:

		// --- Private Variables ---
		float m_Radius = 50.0f;
		float m_AttenuationLinearFactor = 0.09f, m_AttenuationQuadraticFactor = 0.032f;
	};
}

#endif //_LIGHT_H_
