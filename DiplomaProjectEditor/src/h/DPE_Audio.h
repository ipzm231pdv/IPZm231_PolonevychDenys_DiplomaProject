#pragma once
#include <string>
class DPE_Audio
{
public:
	static DPE_Audio* Get();

	virtual bool ExportA(std::string src, std::string out, int beginMs = 0, int outMs = 0) = 0;

	virtual bool Merge(std::string v, std::string a, std::string out) = 0;

	DPE_Audio();
	virtual ~DPE_Audio();
};