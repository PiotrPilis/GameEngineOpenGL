#ifndef EVENTHPP
#define EVENTHPP

namespace GameEngine
{
	class Timer
	{
	public:
		Timer();
		float GetElapsedSeconds();
		void ResetTimer();
	protected:
		float previousseconds;
	};

	class Time
	{
	private:
		Time();
		~Time();

	public:
		// returns an instance of the global time
		static Time& GlobalTime();

		void BindBuffer(const unsigned& program, const unsigned& blockIndex, const char* blockName = "Time") const;
		void Update();

		void RestartTime();
	private:
		Timer timer;
		float elapsedTime;

		unsigned UBO;
	};
}

#endif