#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

class Timer
{
public:

	Timer() = default;

	void Start()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_Running = true;
	}

	void Stop()
	{
		if (m_Running)
		{
			m_EndTime = std::chrono::high_resolution_clock::now();
			m_Running = false;
		}
	}

	float GetNanoseconds() const
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> end_time;

		if (m_Running)
			end_time = std::chrono::high_resolution_clock::now();
		else
			end_time = m_EndTime;

		double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - m_StartTime).count();
		return (float)time;
	}



	float GetMicroseconds() const
	{
		return GetNanoseconds() / 1000.0f;
	}

	float GetMilliseconds() const
	{
		return GetMicroseconds() / 1000.0f;
	}

	float GetSeconds() const
	{
		return GetMilliseconds() / 1000.0f;
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;
	bool m_Running = false;
};

#endif //_TIMER_H_
