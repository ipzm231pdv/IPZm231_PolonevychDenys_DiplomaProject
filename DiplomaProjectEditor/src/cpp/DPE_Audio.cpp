#include "DPE_Audio.h"
#include <iostream>
#include <QMutex>
using namespace std;
struct DPE_Time
{
	DPE_Time(int tms)
	{
		h = (tms / 1000) / 3600;
		m = ((tms / 1000) % 3600) / 60;
		s = (tms / 1000) % (3600 * 60);
		ms = tms % 1000;
	}
	std::string ToString()
	{
		char buf[16] = { 0 };
		sprintf(buf, "%d:%d:%d.%d", h, m, s, ms);
		return buf;
	}
	int h = 0;
	int m = 0;
	int s = 0;
	int ms = 0;
};

class CDPE_Audio :public DPE_Audio
{
public:
	QMutex mutex;
	bool ExportA(std::string src, std::string out, int beginMs, int outMs)
	{
		//ffmpeg -i test.mp4  -ss 0:1:10.112 -t 0:1:10.100 -vn -y test.mp3
		string cmd = "ffmpeg ";
		cmd += " -i ";
		cmd += src;
		cmd += " ";
		if (beginMs > 0)
		{
			cmd += " -ss ";
			DPE_Time xt(beginMs);
			cmd += xt.ToString();
		}
		if (outMs > 0)
		{
			cmd += " -t ";
			DPE_Time xt(outMs);
			cmd += xt.ToString();
		}


		cmd += " -vn -y ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}

	bool Merge(std::string v, std::string a, std::string out)
	{
		//ffmpeg -i test.avi -i test.mp3 -c copy out.avi
		string cmd = "ffmpeg -i ";
		cmd += v;
		cmd += " -i ";
		cmd += a;
		cmd += " -c copy ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}


};

DPE_Audio* DPE_Audio::Get()
{
	static CDPE_Audio ca;
	return &ca;
}
DPE_Audio::DPE_Audio()
{
}


DPE_Audio::~DPE_Audio()
{
}