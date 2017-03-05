#ifndef LOGHPP
#define LOGHPP
#include <GL/glew.h>
#include <string>
#include <sstream>

namespace GameEngine
{
	// Interfejs Log
	class Log 
	{
	public:
		virtual void operator() (const GLchar* msg, bool EnableTime = false, bool ClearData = false) = 0;
		virtual void operator() (const std::string msg, bool EnableTime = false, bool ClearData = false) = 0;
		virtual void operator() (const std::stringstream& msg, bool EnableTime = false, bool ClearData = false) = 0;
	};

	// ErrorLog zapisywany do pliku
	class ErrorLog : public Log
	{
	public:
		void operator() (const GLchar* msg, bool EnableTime = true, bool ClearData = false);
		void operator() (const std::string msg, bool EnableTime = true, bool ClearData = false);
		void operator() (const std::stringstream& msg, bool EnableTime = true, bool ClearData = false);

		static void Write (const GLchar* msg, bool EnableTime = true, bool ClearData = false);
		static void Write(const std::string msg, bool EnableTime = true, bool ClearData = false);
		static void Write(const std::stringstream& msg, bool EnableTime = true, bool ClearData = false);
	};

	// Wyœwietla b³¹d w oknie dialogowym, wywo³uje ErrorLog, przerywa pracê
	class CriticalLog : public Log
	{
	public:
		void operator() (const GLchar* msg, bool EnableTime = true, bool ClearData = false);
		void operator() (const std::string msg, bool EnableTime = true, bool ClearData = false);
		void operator() (const std::stringstream& msg, bool EnableTime = true, bool ClearData = false);

		static void Write(const GLchar* msg, const GLchar* msgLog);
		static void Write(const std::string msg, const std::string msgLog);
		static void Write(const std::stringstream& msg, const std::stringstream& msgLog);
	};

	// Log zapisywany do pliku
	class NormalLog : public Log
	{
	public:
		void operator() (const GLchar* msg, bool EnableTime = false, bool ClearData = false);
		void operator() (const std::string msg, bool EnableTime = false, bool ClearData = false);
		void operator() (const std::stringstream& msg, bool EnableTime = false, bool ClearData = false);

		static void Write(const GLchar* msg, bool EnableTime = false, bool ClearData = false);
		static void Write(const std::string msg, bool EnableTime = false, bool ClearData = false);
		static void Write(const std::stringstream& msg, bool EnableTime = false, bool ClearData = false);
	};
}

#endif
