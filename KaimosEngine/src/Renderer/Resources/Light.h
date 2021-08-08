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
		float GetMaxRadius() const { return m_MaxRadius; }
		float GetMinRadius() const { return m_MinRadius; }
		void SetMaxRadius(float radius) { m_MaxRadius = radius; }
		void SetMinRadius(float radius);

		float GetLinearAttenuationFactor() const { return m_AttenuationLinearFactor; }
		float GetQuadraticAttenuationFactor() const { return m_AttenuationQuadraticFactor; }

	private:

		// --- Private Lights Methods ---
		void CalculateAttenuationValues();

	public:

		// --- Public Variables ---
		float FalloffMultiplier = 1.0f;

	private:

		// --- Private Variables ---
		float m_MinRadius = 50.0f, m_MaxRadius = 100.0f;
		float m_AttenuationLinearFactor = 0.09f, m_AttenuationQuadraticFactor = 0.032f;
	};
}

#endif //_LIGHT_H_
