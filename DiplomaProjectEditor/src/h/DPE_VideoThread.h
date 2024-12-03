#pragma once
#include <QThread>
#include <QMutex>
#include "opencv2/core.hpp"
class DPE_VideoThread :public QThread
{
	Q_OBJECT

public:
	int fps = 0;
	int width = 0;
	int height = 0;
	int width2 = 0;
	int height2 = 0;
	int totalMs = 0;
	std::string src1file;
	std::string desFile;

	int begin = 0;
	int end = 0;

	void SetBegin(double p);
	void SetEnd(double p);




	static DPE_VideoThread* Get()
	{
		static DPE_VideoThread vt;
		return &vt;
	}

	bool Open(const std::string file);


	bool Open2(const std::string file);

	void Play() { mutex.lock(); isPlay = true; mutex.unlock(); };
	void Pause() { mutex.lock(); isPlay = false; mutex.unlock(); };



	double GetPos();

	bool Seek(int frame);
	bool Seek(double pos);


	bool StartSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);


	void StopSave();


	void SetMark(cv::Mat m) { mutex.lock(); this->mark = m; mutex.unlock(); };


	~DPE_VideoThread();

	void run();

signals:
	void ViewImage1(cv::Mat mat);
	void ViewImage2(cv::Mat mat);

	void ViewDes(cv::Mat mat);
	void SaveEnd();
protected:
	QMutex mutex;

	bool isWrite = false;
	bool isPlay = false;
	cv::Mat mark;
	DPE_VideoThread();
};

