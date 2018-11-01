// CommandLineParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommandLineParser.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CommandLineParser parser;
	int ret = parser.AddCommandLineSwitch("-l","loginId");
	ret = parser.AddCommandLineSwitch("-p","loginPassword");
	ret = parser.AddCommandLineSwitch("-t","loginToken");
	ret = parser.AddCommandLineSwitch("-j","meetingNo");
	ret = parser.AddCommandLineSwitch("-r","record",false);
	ret = parser.AddCommandLineSwitch("-v","UISetting",true,true);
	ret = parser.AddCommandLineSwitch("--config-path","config-path");

	ret = parser.InitWithCommandLine(argc,argv);
	printf("InitWithCommandLine return %d\n",ret);

	std::string programeName;
	parser.GetProgrameName(programeName);
	printf("programeName : %s\n",programeName.c_str());

	std::string errInfo;
	bool bIsValid = parser.IsCommandLineValid(errInfo);
	printf("IsValidCommandLine:%d, errInfo:%s\n",(int)bIsValid,errInfo.c_str());

	bool bRet = parser.HasSwitch(std::string("record"));
	printf("HasSwitch -r : %d\n",(int)bRet);

	bRet = parser.HasSwitch(std::string("UISetting"),std::string("NoMinimum"));
	printf("HasSwitch -v NoMinimum : %d\n",(int)bRet);

	bRet = parser.HasSwitch(std::string("UISetting"),std::string("NoQuit"));
	printf("HasSwitch -v NoQuit : %d\n",(int)bRet);

	bRet = parser.HasSwitch(std::string("UISetting"),std::string("NoDesktop"));
	printf("HasSwitch -v NoDesktop : %d\n",(int)bRet);

	std::string sValue;
	bool bHasMoreValue = false;
	bRet = parser.GetSwicth(std::string("meetingNo"),sValue,bHasMoreValue);
	printf("GetSwicth -j meetingNo : %d, meetingNo=%s\n",(int)bRet,sValue.c_str());

	bHasMoreValue = false;
	bRet = parser.GetSwicth(std::string("loginToken"),sValue,bHasMoreValue);
	printf("GetSwicth -t token : %d, token=%s\n",(int)bRet,sValue.c_str());

	bHasMoreValue = false;
	bRet = parser.GetSwicth(std::string("loginPassword"),sValue,bHasMoreValue);
	printf("GetSwicth -p password : %d, password=%s\n",(int)bRet,sValue.c_str());

	bHasMoreValue = false;
	bRet = parser.GetSwicth(std::string("loginId"),sValue,bHasMoreValue);
	printf("GetSwicth -l account : %d, account=%s\n",(int)bRet,sValue.c_str());

	bHasMoreValue = false;
	bRet = parser.GetSwicth(std::string("config-path"),sValue,bHasMoreValue);
	printf("GetSwicth --config-path config-path : %d, config-path=%s\n",(int)bRet,sValue.c_str());

	getchar();

	return 0;
}



CommandLineParser::CommandLineParser(void)
{
	m_bIsValid = false;
	m_sInvalidCommandArgs = "Not Init";
}


CommandLineParser::~CommandLineParser(void)
{
}

int CommandLineParser::AddCommandLineSwitch(const char* pPatern,const char* pAccessKey,bool bHasValue,bool bDuplicable)
{
	if (NULL == pPatern)
	{
		//wrong function argument
		return -1;
	}
	std::string sPatern = pPatern;
	std::string sAccessKey;
	if (NULL == pAccessKey || 0 >= strlen(pAccessKey))
	{
		sAccessKey = sPatern;
	}
	else
	{
		sAccessKey = pAccessKey;
	}
	if (m_switchPaterns.find(sPatern) != m_switchPaterns.end())
	{
		//add duplicate switch
		return -2;
	}
	CommandLineSwitch commandSwitch;
	commandSwitch.m_sPatern = sPatern;
	commandSwitch.m_sAccessKey = sAccessKey;
	commandSwitch.m_bDuplicable = bDuplicable;
	commandSwitch.m_bHasValue = bHasValue;

	m_switchPaterns.insert(std::make_pair<std::string,CommandLineSwitch>(sPatern,commandSwitch));

	return 0;
}

int CommandLineParser::InitWithCommandLine(int argc,char** argv)
{
	if (argc <= 0 || NULL == argv)
	{
		m_sInvalidCommandArgs = "Invalid Init arguments";
		return -1;
	}
	if (argc == 1)
	{
		m_sProgramName = argv[0];
		m_sInvalidCommandArgs = "no arguments";
		return 0;
	}
	bool bIsValid = true;
	m_sInvalidCommandArgs.clear();
	m_sProgramName = argv[0];

	std::list<std::string> arglist;
	for (int iPos = 1; iPos < argc; iPos++)
	{
		if (NULL == argv[iPos])
		{
			m_sInvalidCommandArgs = "Invalid Init arguments";
			return -2;
		}
		arglist.push_back(std::string(argv[iPos]));
	}

	while (!arglist.empty())
	{
		std::string sPatern = arglist.front();
		arglist.pop_front();

		if (m_switchPaterns.find(sPatern) != m_switchPaterns.end())
		{
			if (m_switchPaterns[sPatern].m_bHasValue)
			{
				if (arglist.empty())
				{
					std::string err = sPatern;
					err.append(" should have a value. ");
					m_sInvalidCommandArgs.append(err);
					return -2;
				}
				std::string sValue = arglist.front();
				arglist.pop_front();
				if (m_commandSwitches.find(m_switchPaterns[sPatern].m_sAccessKey) != m_commandSwitches.end())
				{
					if (!m_switchPaterns[sPatern].m_bDuplicable)
					{
						m_commandSwitches[m_switchPaterns[sPatern].m_sAccessKey].insert(sValue);
					}
					else
					{
						std::string err = sPatern;
						err.append(" set multy values. ");
						m_sInvalidCommandArgs.append(err);
						//return -2;
						bIsValid = false;
					}
				}
				else
				{
					std::set<std::string> values;
					values.insert(sValue);
					m_commandSwitches[m_switchPaterns[sPatern].m_sAccessKey] = values;
				}
			}
			else
			{
				if (m_commandSwitches.find(m_switchPaterns[sPatern].m_sAccessKey) != m_commandSwitches.end())
				{
					std::string err = sPatern;
					err.append(" set multy values. ");
					m_sInvalidCommandArgs.append(err);
					//return -2;
					bIsValid = false;
				}
				else
				{
					std::set<std::string> values;
					m_commandSwitches[m_switchPaterns[sPatern].m_sAccessKey] = values;
				}
			}
		}
		else
		{
			std::string err = sPatern;
			err.append(" is invalid patern. ");
			m_sInvalidCommandArgs.append(err);
			bIsValid = false;
		}
	}

	m_bIsValid = bIsValid;

	return 0;
}

bool CommandLineParser::HasSwitch(const std::string &sAccessKey)
{
	if (m_commandSwitches.find(sAccessKey) != m_commandSwitches.end())
	{
		return true;
	}
	return false;
}

bool CommandLineParser::HasSwitch(const std::string &sAccessKey,const std::string &sValue)
{
	if (m_commandSwitches.find(sAccessKey) != m_commandSwitches.end() &&
		m_commandSwitches[sAccessKey].find(sValue) != m_commandSwitches[sAccessKey].end())
	{
		return true;
	}
	return false;
}

bool CommandLineParser::GetSwicth(const std::string &sAccessKey,std::string &sValue,bool &bHasMoreValues)
{
	if (m_commandSwitches.find(sAccessKey) != m_commandSwitches.end())
	{
		if (!m_commandSwitches[sAccessKey].empty())
		{
			sValue = *m_commandSwitches[sAccessKey].begin();
		}
		else
		{
			sValue = "";
		}
		bHasMoreValues = m_commandSwitches[sAccessKey].size() > 1;
		return true;
	}
	return false;
}

bool CommandLineParser::GetSwitch(const std::string &sAccessKey,std::set<std::string> &vecValue)
{
	if (m_commandSwitches.find(sAccessKey) != m_commandSwitches.end())
	{
		vecValue = m_commandSwitches[sAccessKey];
		return true;
	}
	return false;
}

bool CommandLineParser::IsCommandLineValid(std::string &errInfo)
{
	errInfo = m_sInvalidCommandArgs;
	return m_bIsValid;
}

bool CommandLineParser::GetProgrameName(std::string &programeName)
{
	programeName = m_sProgramName;
	return !m_sProgramName.empty();
}
