#include "DPE_VideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "DPE_Filter.h"
using namespace std;
using namespace cv;


static VideoCapture cap1;
static VideoCapture cap2;

static VideoWriter vw;

static bool isexit = false;

bool DPE_VideoThread::StartSave(const std::string filename, int width, int height, bool isColor)
{
	cout << "Start export" << endl;
	Seek(begin);
	mutex.lock();
	if (!cap1.isOpened())
	{
		mutex.unlock();
		return false;
	}
	if (width <= 0)
		width = cap1.get(CAP_PROP_FRAME_WIDTH);
	if (height <= 0)
		height = cap1.get(CAP_PROP_FRAME_HEIGHT);

	vw.open(filename,
		VideoWriter::fourcc('X', '2', '6', '4'),
		this->fps,
		Size(width, height), isColor
	);
	if (!vw.isOpened())
	{
		mutex.unlock();
		cout << "start save failed!" << endl;
		return false;
	}
	this->isWrite = true;
	desFile = filename;
	mutex.unlock();
	return true;
}

void DPE_VideoThread::StopSave()
{
	cout << "stop export" << endl;
	mutex.lock();
	vw.release();
	isWrite = false;
	mutex.unlock();
}
void DPE_VideoThread::SetBegin(double p)
{
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	begin = frame;
	mutex.unlock();
}
void DPE_VideoThread::SetEnd(double p)
{
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	end = frame;
	mutex.unlock();
}
bool DPE_VideoThread::Seek(double pos)
{
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = pos * count;
	return Seek(frame);
}
bool DPE_VideoThread::Seek(int frame)
{
	mutex.lock();
	if (!cap1.isOpened())
	{
		mutex.unlock();
		return  false;
	}
	int re = cap1.set(CAP_PROP_POS_FRAMES, frame);
	if (cap2.isOpened())
		cap2.set(CAP_PROP_POS_FRAMES, frame);
	mutex.unlock();
	return re;
}
double DPE_VideoThread::GetPos()
{
	double pos = 0;
	mutex.lock();
	if (!cap1.isOpened())
	{
		mutex.unlock();
		return pos;
	}
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	double cur = cap1.get(CAP_PROP_POS_FRAMES);
	if (count > 0.001)
		pos = cur / count;
	mutex.unlock();
	return pos;
}

bool DPE_VideoThread::Open2(const std::string file)
{
	Seek(0);
	cout << "open2 :" << file << endl;
	mutex.lock();
	bool re = cap2.open(file);
	mutex.unlock();
	cout << re << endl;
	if (!re)
		return re;
	//fps = cap1.get(CAP_PROP_FPS);
	width2 = cap2.get(CAP_PROP_FRAME_WIDTH);
	height2 = cap2.get(CAP_PROP_FRAME_HEIGHT);
	//if (fps <= 0) fps = 25;
	return true;
}
bool DPE_VideoThread::Open(const std::string file)
{
	cout << "open :" << file << endl;
	Seek(0);

	mutex.lock();
	bool re = cap1.open(file);

	mutex.unlock();

	cout << re << endl;
	if (!re)
		return re;
	fps = cap1.get(CAP_PROP_FPS);
	width = cap1.get(CAP_PROP_FRAME_WIDTH);
	height = cap1.get(CAP_PROP_FRAME_HEIGHT);
	if (fps <= 0) fps = 25;
	src1file = file;
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	totalMs = (count / (double)fps) * 1000;
	return true;
}
void DPE_VideoThread::run()
{
	Mat mat1;
	//for (;;)
	while (!isexit)
	{
		mutex.lock();
		if (isexit)
		{
			mutex.unlock();
			break;
		}
		if (!cap1.isOpened())
		{
			mutex.unlock();
			msleep(5);
			continue;
		}
		if (!isPlay)
		{
			mutex.unlock();
			msleep(5);
			continue;
		}

		int cur = cap1.get(CAP_PROP_POS_FRAMES);
		if ((end > 0 && cur >= end) || !cap1.read(mat1) || mat1.empty())
		{
			mutex.unlock();
			if (isWrite)
			{
				StopSave();
				SaveEnd();
			}

			msleep(5);
			continue;
		}
		Mat mat2 = mark;
		if (cap2.isOpened())
		{
			cap2.read(mat2);
		}
		if (!isWrite)
		{
			ViewImage1(mat1);
			if (!mat2.empty())
				ViewImage2(mat2);
		}



		Mat des = DPE_Filter::Get()->Filter(mat1, mat2);

		if (!isWrite)
			ViewDes(des);

		int s = 0;
		s = 1000 / fps;
		if (isWrite)
		{
			s = 1;
			vw.write(des);
		}

		msleep(s);

		mutex.unlock();
	}
}
DPE_VideoThread::DPE_VideoThread()
{
	start();

}


DPE_VideoThread::~DPE_VideoThread()
{

	mutex.lock();
	isexit = true;
	mutex.unlock();
	wait();
}
