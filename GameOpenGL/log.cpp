#include <iostream>
#include <fstream>
#include <Windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <ctime>

#include "log.hpp"
#include "config.hpp"

#pragma warning(disable : 4996)

#define	LogFileName	"Log.txt"
#define	ErrLogFileName	"ErrorLog.txt"

using namespace std;

namespace GameEngine
{
	void ErrorLog::operator() (const GLchar* msg, bool EnableTime, bool ClearData)
	{
		ErrorLog:Write(msg, EnableTime, ClearData);
	}
	void ErrorLog::operator() (const std::string msg, bool EnableTime, bool ClearData)
	{
		ErrorLog:Write(msg, EnableTime, ClearData);
	}
	void ErrorLog::operator() (const std::stringstream& msg, bool EnableTime, bool ClearData)
	{
		ErrorLog:Write(msg, EnableTime, ClearData);
	}
	void ErrorLog::Write (const GLchar* msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(ErrLogFileName, ios::out | ios::trunc);
		else
			fout.open(ErrLogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cerr << msg << endl;
		fout << msg << endl;
		fout.close();
	}
	void ErrorLog::Write(const std::string msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(ErrLogFileName, ios::out | ios::trunc);
		else
			fout.open(ErrLogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cerr << msg << endl;
		fout << msg << endl;
		fout.close();
	}
	void ErrorLog::Write(const std::stringstream& msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(ErrLogFileName, ios::out | ios::trunc);
		else
			fout.open(ErrLogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cerr << msg.str() << endl;
		fout << msg.str() << endl;
		fout.close();
	}
	void CriticalLog::operator() (const GLchar* msg, bool EnableTime, bool ClearData)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg, NULL, MB_ICONHAND | MB_OK);
#endif
		std::string STRmsgLog(msg);
		ErrorLog::Write(("FATAL ERROR: " + STRmsgLog), EnableTime, ClearData);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void CriticalLog::operator() (const std::string msg, bool EnableTime, bool ClearData)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg.c_str(), NULL, MB_ICONHAND | MB_OK);
#endif
		ErrorLog::Write(("FATAL ERROR: " + msg), EnableTime, ClearData);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void CriticalLog::operator() (const std::stringstream& msg, bool EnableTime, bool ClearData)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg.str().c_str(), NULL, MB_ICONHAND | MB_OK);
#endif
		ErrorLog::Write(("FATAL ERROR: " + msg.str()), EnableTime, ClearData);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void CriticalLog::Write (const GLchar* msg, const GLchar* msgLog)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg, NULL, MB_ICONHAND | MB_OK);
#endif
		std::string STRmsgLog(msgLog);
		ErrorLog::Write(("FATAL ERROR: " + STRmsgLog), true, false);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void CriticalLog::Write(const std::string msg, const std::string msgLog)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg.c_str(), NULL, MB_ICONHAND | MB_OK);
#endif
		ErrorLog::Write(("FATAL ERROR: " + msgLog), true, false);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void CriticalLog::Write(const std::stringstream& msg, const std::stringstream& msgLog)
	{
#ifdef GAMEEDITOR
		QMessageBox::critical(NULL, "FATAL ERROR", msg, //QMessageBox::Ok);
#else
		MessageBox(NULL, msg.str().c_str(), NULL, MB_ICONHAND | MB_OK);
#endif
		ErrorLog::Write(("FATAL ERROR: " + msgLog.str()), true, false);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	void NormalLog::operator() (const GLchar* msg, bool EnableTime, bool ClearData)
	{
		NormalLog::Write(msg, EnableTime, ClearData);
	}
	void NormalLog::operator() (const std::string msg, bool EnableTime, bool ClearData)
	{
		NormalLog::Write(msg, EnableTime, ClearData);
	}
	void NormalLog::operator() (const std::stringstream& msg, bool EnableTime, bool ClearData)
	{
		NormalLog::Write(msg, EnableTime, ClearData);
	}
	void NormalLog::Write (const GLchar* msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(LogFileName, ios::out | ios::trunc);
		else
			fout.open(LogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cout << msg << endl;
		fout << msg << endl;
		fout.close();
	}
	void NormalLog::Write(const std::string msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(LogFileName, ios::out | ios::trunc);
		else
			fout.open(LogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cout << msg << endl;
		fout << msg << endl;
		fout.close();
	}
	void NormalLog::Write(const std::stringstream& msg, bool EnableTime, bool ClearData)
	{
		ofstream fout;
		if (ClearData)
			fout.open(LogFileName, ios::out | ios::trunc);
		else
			fout.open(LogFileName, ios::out | ios::app);
		if (!fout)
		{
			fout.close();
			return;
		}
		if (EnableTime)
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			fout << endl << std::ctime(&now);
		}
		if (DebugMode)
			cout << msg.str() << endl;
		fout << msg.str() << endl;
		fout.close();
	}
}