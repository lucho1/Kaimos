#ifndef _TIMESTEP_H_
#define _TIMESTEP_H_


namespace Kaimos {

	class Timestep
	{
	public:
		
		Timestep(float time = 0.0f) : m_Time(time) {}

		inline float GetSeconds()		const { return m_Time; }
		inline float GetMilliseconds()	const { return m_Time * 1000.0f; }

		inline operator float() const { return m_Time; }

	private:

		float m_Time;
	};
}

#endif //_TIMESTEP_H_
