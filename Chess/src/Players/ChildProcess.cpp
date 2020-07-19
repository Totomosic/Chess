#include "ChildProcess.h"

namespace Chess
{

	ChildProcess::ChildProcess(const std::vector<std::string>& args)
		: m_StdoutRead(NULL), m_StdinWrite(NULL), m_Process(NULL), m_Thread(NULL), m_ListenerThread(), m_Events(),
		OnMessageReceived(m_Events.GetEmitter<ChildProcessMessage>())
	{
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;
		
		HANDLE stdoutWrite;
		HANDLE stdinRead;

		CreatePipe(&m_StdoutRead, &stdoutWrite, &saAttr, 0);
		CreatePipe(&stdinRead, &m_StdinWrite, &saAttr, 0);
		SetHandleInformation(m_StdinWrite, HANDLE_FLAG_INHERIT, 0);

		std::wstring argString = L"";
		for (const std::string& a : args)
		{
			argString += L"\"" + std::wstring(a.begin(), a.end()) + L"\"" + L" ";
		}
		TCHAR* szCmdLine = new TCHAR[argString.size() + 1];
		memcpy(szCmdLine, argString.data(), (argString.size() + 1) * sizeof(TCHAR));
		PROCESS_INFORMATION procInfo;
		STARTUPINFO startInfo;

		ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&startInfo, sizeof(STARTUPINFO));
		startInfo.cb = sizeof(STARTUPINFO);
		startInfo.hStdError = stdoutWrite;
		startInfo.hStdOutput = stdoutWrite;
		startInfo.hStdInput = stdinRead;
		startInfo.dwFlags |= STARTF_USESTDHANDLES;

		CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &procInfo);

		m_Process = procInfo.hProcess;
		m_Thread = procInfo.hThread;
		CloseHandle(stdoutWrite);
		CloseHandle(stdinRead);

		m_ListenerThread = std::thread([this]()
			{
				while (true)
				{
					char buffer[4096];
					DWORD read;
					bool success = ReadFile(m_StdoutRead, buffer, sizeof(buffer), &read, NULL);
					if (!success)
						break;
					buffer[read] = '\0';
					ChildProcessMessage message;
					message.Message = buffer;
					OnMessageReceived.Emit(message);
				}
			});

		delete[] szCmdLine;
	}

	ChildProcess::~ChildProcess()
	{
		if (m_Process)
		{
			CloseHandle(m_StdoutRead);
			CloseHandle(m_StdinWrite);
			m_ListenerThread.join();

			TerminateProcess(m_Process, 0);
			WaitForSingleObject(m_Process, INFINITE);

			CloseHandle(m_Process);
			CloseHandle(m_Thread);
		}
	}

	void ChildProcess::SendCommand(const std::string& message)
	{
		std::string line = message + '\n';
		DWORD written;
		if (!WriteFile(m_StdinWrite, (const void*)line.data(), line.size() * sizeof(char), &written, NULL))
			BLT_WARN("Failed to send message {}", message);
	}

}
