#ifndef _TIMESTEP_H
#define _TIMESTEP_H


namespace Kaimos {

	class Timestep
	{
	public:
		
		Timestep(float time = 0.0f) : m_Time(time) {}

		operator float() const { return m_Time; }

		const float GetSeconds() const { return m_Time; }
		const float GetMilliseconds() const { return m_Time * 1000.0f; }

	private:

		float m_Time;
	};
}

#endif