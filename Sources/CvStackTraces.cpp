#include "CvStackTraces.h"

#include "CvPython.h"
#include "StackWalker.h"
#include <vector>


std::string getPyTrace()
{
	std::vector<Cy::StackFrame> trace = Cy::get_stack_trace();

	std::stringstream buffer;

	for (std::vector<Cy::StackFrame>::const_iterator itr = trace.begin(); itr != trace.end(); ++itr)
	{
		if (itr != trace.begin()) buffer << "\r\n";
		buffer << CvString::format("%s.py (%d): %s", itr->filename.c_str(), itr->line, itr->code.c_str());
	}

	return buffer.str();
}

class C2CStackWalker : StackWalker
{
public:
	std::string get_callstack(int skip = 0)
	{
		m_buffer.str(std::string());
		m_idx = 0;
		m_skip = skip;
		m_started = false;
		ShowCallstack();
		return m_buffer.str();
	}

	static C2CStackWalker& get()
	{
		static C2CStackWalker inst;
		return inst;
	}

protected:

	virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
	{
		if ((eType != lastEntry) && (entry.offset != 0) && m_idx++ >= m_skip)
		{
			if (m_started)
				m_buffer << "\r\n";
			m_started = true;
			if (entry.lineFileName[0] == 0)
			{
				m_buffer << (LPVOID)entry.offset;
				if (entry.moduleName[0] == 0)
					m_buffer << " (module-name not available):";
				else
					m_buffer << " (" << entry.moduleName << "):";
				m_buffer << " (filename not available): ";
			}
			else
			{
				std::string fileName(entry.lineFileName);
				std::string fileNameLwr(fileName);
				algo::transform(fileNameLwr, fileNameLwr.begin(), ::tolower);
				size_t offs = fileNameLwr.find("caveman2cosmos");
				if (offs != std::string::npos)
				{
					fileName = fileName.substr(offs + 15);
				}
				m_buffer << fileName << " (" << entry.lineNumber << "): ";
			}
			if (entry.undName[0] != 0)
				m_buffer << entry.undName;
			else if (entry.undFullName[0] != 0)
				m_buffer << entry.undFullName;
			else if (entry.name[0] != 0)
				m_buffer << entry.name;
			else
				m_buffer << "(function-name not available)";
		}
	}

private:
	int m_idx;
	int m_skip;
	bool m_started;
	std::stringstream m_buffer;
};

std::string getDLLTrace()
{
	return C2CStackWalker::get().get_callstack(4);
}