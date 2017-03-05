#include <time.h>
#include <GL/glew.h>
#include "event.hpp"

namespace GameEngine
{
	Timer::Timer() { previousseconds = static_cast <float>(clock()) / CLOCKS_PER_SEC; };

	float Timer::GetElapsedSeconds() {
		float currentseconds = static_cast <float>(clock()) / CLOCKS_PER_SEC;
		float elapsedseconds = currentseconds - previousseconds;
		return elapsedseconds;
	}
	void Timer::ResetTimer() { previousseconds = static_cast <float>(clock()) / CLOCKS_PER_SEC; };

	Time::Time()
	{
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(elapsedTime), &elapsedTime, GL_DYNAMIC_DRAW);
	}

	Time::~Time()
	{
		glDeleteBuffers(1, &UBO);
	}

	void Time::BindBuffer(const unsigned& program, const unsigned& blockIndex, const char* blockName) const
	{
		glUniformBlockBinding(program, glGetUniformBlockIndex(program, blockName), blockIndex);
		glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, UBO);
	}

	void Time::Update()
	{
		elapsedTime = timer.GetElapsedSeconds();
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(elapsedTime), &elapsedTime);
	}

	void Time::RestartTime()
	{
		timer.ResetTimer();
		Update();
	}

	Time& Time::GlobalTime()
	{
		static Time time;
		return time;
	}
}