#pragma once
#include "Bolt.h"

namespace Chess
{

	struct ChildProcessMessage
	{
	public:
		std::string Message;
	};

	class ChildProcess
	{
	private:
		HANDLE m_StdoutRead;
		HANDLE m_StdinWrite;

		HANDLE m_Process;
		HANDLE m_Thread;
		std::thread m_ListenerThread;

		EventBus m_Events;

	public:
		EventEmitter<ChildProcessMessage> OnMessageReceived;

	public:
		ChildProcess(const std::vector<std::string>& args);
		ChildProcess(const ChildProcess& other) = delete;
		ChildProcess& operator=(const ChildProcess& other) = delete;
		ChildProcess(ChildProcess&& other) = delete;
		ChildProcess& operator=(ChildProcess&& other) = delete;
		~ChildProcess();

		void SendCommand(const std::string& message);

	};

}
