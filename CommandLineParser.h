#ifndef __COMMANDLINEPARSER_H__
#define __COMMANDLINEPARSER_H__

#include <string>
#include <map>
#include <list>
#include <set>

// command -l<patern> accountId<value>
class CommandLineParser
{
public:
	CommandLineParser(void);
	~CommandLineParser(void);

	struct CommandLineSwitch
	{
		std::string					m_sPatern;
		std::string					m_sAccessKey;
		//std::set<std::string>		m_value;
		bool						m_bDuplicable;
		bool						m_bHasValue;

		CommandLineSwitch()
		{
			m_bDuplicable = false;
			m_bHasValue = false;
		}
	};

	int AddCommandLineSwitch(const char* pPatern,const char* pAccessKey,bool bHasValue = true,bool bDuplicable = false);

	int InitWithCommandLine(int argc,char** argv);

	bool HasSwitch(const std::string &sAccessKey);

	bool HasSwitch(const std::string &sAccessKey,const std::string &sValue);

	bool GetSwicth(const std::string &sAccessKey,std::string &sValue,bool &bHasMoreValues);

	bool GetSwitch(const std::string &sAccessKey,std::set<std::string> &vecValue);

	bool IsCommandLineValid(std::string &errInfo);

	bool GetProgrameName(std::string &programeName);

protected:
	std::map<std::string,CommandLineSwitch>		m_switchPaterns;
	std::map<std::string,std::set<std::string> >m_commandSwitches;
	bool										m_bIsValid;
	std::string									m_sInvalidCommandArgs;
	std::string									m_sProgramName;
};

#endif
