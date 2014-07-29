#include "Global.h"

bool Global::isNumber(const std::string s)
{
	const std::string b = Global::noWhiteSpaceString(s,true,true);
	if(s.at(0) == '-')
	{
		const std::string a = b.substr(1,b.npos);
		int i = !a.empty() && std::find_if(a.begin(), 
			 a.end(), [](char c) { return !std::isdigit(c); }) == a.end();
		return i*(-1);
	}
	else
	{
		 return !b.empty() && std::find_if(b.begin(), 
			 b.end(), [](char c) { return !std::isdigit(c); }) == b.end();
	}
}
int Global::getRandomInt()
{
	return rand() * rand() + rand() - rand() + rand() + rand()*2;
}
int Global::getRandomInt(int maxValue, int minValue)
{
	return getRandomInt()%(maxValue-minValue) + minValue;
}
void Global::toUpperConvert(std::string &s)
{
	for(unsigned int l = 0; l < s.length(); l++)
	{
		s[l] = toupper(s[l]);
	}
}
std::string Global::toUpperString(std::string s)
{
	for(unsigned int l = 0; l < s.length(); l++)
	{
		s[l] = toupper(s[l]);
	}
	return s;
}
std::string Global::noWhiteSpaceString(std::string s, bool trimLeft = true,bool trimRight = true)
{
	if(s == "")
		return "";
	else
	{
		std::string whitespace = " \t";
		int strStart = s.find_first_not_of(whitespace);
		int strEnd = s.find_last_not_of(whitespace);
		int strLength;
		if(trimLeft && trimRight)
		{
			strLength = strEnd - strStart+1;
			s = s.substr(strStart,strLength);
			return s;
		}
		else if(trimLeft)
		{
			s = s.substr(strStart,s.size());
			return s;
		}
		else if(trimRight)
		{
			s = s.substr(0,strEnd);
			return s;
		}
		else
			//no trimming at all?
			return s;
	}
}
void Global::noWhiteSpaceConvert(std::string &s)
{
	if(s != "")
	{
		std::string whitespace = " \t";
		int strStart = s.find_first_not_of(whitespace);
		int strEnd = s.find_last_not_of(whitespace);
		int strLength = strEnd - strStart+1;
		s = s.substr(strStart,strLength);
	}
}
bool Global::getLeLine(std::ifstream &file, std::string &string)
{
	std::string leString;
	while(getline(file,leString))
	{
		int stringEnd = leString.find(';');
		if(stringEnd == -1)
			stringEnd = leString.length();
		else
			stringEnd = leString.find(';');

		if(leString.find(';') == 0 || leString == "" || std::string::npos == leString.find_first_not_of(" ") || std::string::npos == leString.substr(0,stringEnd).find_first_not_of(" "))
		{/*This line is a comment or empty move on*/}
		else
		{
			string = noWhiteSpaceString(leString.substr(0,stringEnd));
			return true;
		}
	}
	return false;
}
bool Global::getLeLine(std::ifstream &file, std::string &string,int &lineCounter)
{
	std::string leString;
	while(getline(file,leString))
	{
		lineCounter++;
		int stringEnd = leString.find(';');
		if(stringEnd == -1)
			stringEnd = leString.length();
		else
			stringEnd = leString.find(';');

		if(leString.find(';') == 0 || leString == "" || std::string::npos == leString.find_first_not_of(" ") || std::string::npos == leString.substr(0,stringEnd).find_first_not_of(" "))
		{/*This line is a comment or empty move on*/}
		else
		{
			string = noWhiteSpaceString(leString.substr(0,stringEnd));
			return true;
		}
	}
	return false;
}
std::string Global::getLeVarVal(std::string string)
{
	return noWhiteSpaceString(string.substr(string.find('=')+1,string.npos-string.find('=')));
}
std::string Global::getLeVarName(std::string string)
{
	return noWhiteSpaceString(toUpperString(string.substr(0,string.find('='))));
}
void Global::convertToLeVarVal(std::string &string)
{
	string = noWhiteSpaceString(string.substr(string.find('=')+1,string.npos-string.find('=')));
}
void Global::convertToLeVarName(std::string &string)
{
	string = noWhiteSpaceString(toUpperString(string.substr(0,string.find('='))));
}

std::string Global::printSpace(int amount)
{
	std::string s = "";
	for(int i=0;i<amount;i++)
		s.append(" ");
	return s;
}
void Global::clearBuffer()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Global::cWHITE);
	std::cin.clear();
	std::cin.ignore(std::cin.rdbuf()->in_avail());
}