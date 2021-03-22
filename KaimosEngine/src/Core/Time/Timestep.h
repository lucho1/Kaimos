#ifndef _TIMESTEP_H_
#define _TIMESTEP_H_


namespace Kaimos {

	class Timestep
	{
	public:
		
		Timestep(float time = 0.0f) : m_Time(time) {}

		const float GetSeconds() const { return m_Time; }
		const float GetMilliseconds() const { return m_Time * 1000.0f; }

		operator float() const { return m_Time; }

	private:

		float m_Time;
	};
}

#endif //_TIMESTEP_H_
