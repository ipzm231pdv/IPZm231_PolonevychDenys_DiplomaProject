#include "DPE_VideoUI.h"
#include <QFileDialog>
#include <string>
#include <QMessageBox>
#include "DPE_VideoThread.h"
#include "DPE_Filter.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "DPE_Audio.h"
#include <QFile>
using namespace std;
using namespace cv;
static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;
static bool isBlend = false;
static bool isMerge = false;

DPE_VideoUI::DPE_VideoUI(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//setWindowFlags(Qt::FramelessWindowHint);
	qRegisterMetaType<cv::Mat>("cv::Mat");

	QObject::connect(DPE_VideoThread::Get(),
		SIGNAL(ViewImage1(cv::Mat)),
		ui.src1,
		SLOT(SetImage(cv::Mat))
	);
	QObject::connect(DPE_VideoThread::Get(),
		SIGNAL(ViewImage2(cv::Mat)),
		ui.src2,
		SLOT(SetImage(cv::Mat))
	);

	QObject::connect(DPE_VideoThread::Get(),
		SIGNAL(ViewDes(cv::Mat)),
		ui.des,
		SLOT(SetImage(cv::Mat))
	);


	QObject::connect(DPE_VideoThread::Get(),
		SIGNAL(SaveEnd()),
		this,
		SLOT(ExportEnd())
	);
	Pause();
	startTimer(40);
}
void DPE_VideoUI::timerEvent(QTimerEvent* e)
{
	if (pressSlider)return;
	double pos = DPE_VideoThread::Get()->GetPos();
	ui.playSlider->setValue(pos * 1000);

}
void DPE_VideoUI::Play()
{
	ui.pauseButton->show();
	ui.pauseButton->setGeometry(ui.playButton->geometry());
	DPE_VideoThread::Get()->Play();
	ui.playButton->hide();
}
void DPE_VideoUI::Pause()
{
	ui.playButton->show();
	//ui.playButton->setGeometry(ui.playButton->geometry());
	ui.pauseButton->hide();
	DPE_VideoThread::Get()->Pause();
}

void DPE_VideoUI::Open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Виберіть відеофайл"));
	if (name.isEmpty())return;
	string file = name.toLocal8Bit().data();
	if (!DPE_VideoThread::Get()->Open(file))
	{
		QMessageBox::information(this, "error", name + " open failed!");
		return;
	}

	Play();
	//QMessageBox::information(this, "", name);
}

void DPE_VideoUI::SliderPress()
{
	pressSlider = true;
}

void DPE_VideoUI::SliderRelease()
{
	pressSlider = false;
}

void DPE_VideoUI::Left(int pos)
{
	DPE_VideoThread::Get()->SetBegin((double)pos / 1000.);
	SetPos(pos);
}

void DPE_VideoUI::Right(int pos)
{
	DPE_VideoThread::Get()->SetEnd((double)pos / 1000.);
}
void DPE_VideoUI::SetPos(int pos)
{
	DPE_VideoThread::Get()->Seek((double)pos / 1000.);
}

void DPE_VideoUI::Merge()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(
		this, "select video:");
	if (name.isEmpty())
	{
		return;
	}
	std::string file = name.toLocal8Bit().data();
	isMerge = DPE_VideoThread::Get()->Open2(file);
}
void DPE_VideoUI::Blend()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(
		this, "select video:");
	if (name.isEmpty())
	{
		return;
	}
	std::string file = name.toLocal8Bit().data();
	isBlend = DPE_VideoThread::Get()->Open2(file);
}

void DPE_VideoUI::Mark()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(
		this, "select image:");
	if (name.isEmpty())
	{
		return;
	}
	std::string file = name.toLocal8Bit().data();
	cv::Mat mark = imread(file);
	if (mark.empty())return;
	DPE_VideoThread::Get()->SetMark(mark);
	isMark = true;


}

void DPE_VideoUI::ExportEnd()
{
	isExport = false;
	ui.exportButton->setText("Start Export");

	string src = DPE_VideoThread::Get()->src1file;
	string des = DPE_VideoThread::Get()->desFile;
	int ss = 0;
	int t = 0;
	ss = DPE_VideoThread::Get()->totalMs * ((double)ui.left->value() / 1000.);
	int end = DPE_VideoThread::Get()->totalMs * ((double)ui.right->value() / 1000.);
	t = end - ss;

	DPE_Audio::Get()->ExportA(src, src + ".mp3", ss, t);
	string tmp = des + ".avi";
	QFile::remove(tmp.c_str());
	QFile::rename(des.c_str(), tmp.c_str());
	DPE_Audio::Get()->Merge(tmp, src + ".mp3", des);


	//DPE_Audio::Get()->Merge()


}
void DPE_VideoUI::Export()
{

	if (isExport)
	{
		DPE_VideoThread::Get()->StopSave();
		isExport = false;
		ui.exportButton->setText("Start Export");
		return;
	}
	QString name = QFileDialog::getSaveFileName(
		this, "save", "out1.avi");
	if (name.isEmpty())return;
	std::string filename = name.toLocal8Bit().data();
	int w = ui.width->value();
	int h = ui.height->value();
	if (DPE_VideoThread::Get()->StartSave(filename, w, h, isColor))
	{
		isExport = true;
		ui.exportButton->setText("Stop Export");
	}
}
void DPE_VideoUI::Set()
{
	DPE_Filter::Get()->Clear();
	isColor = true;
	bool isClip = false;
	double cx = ui.cx->value();
	double cy = ui.cy->value();
	double cw = ui.cw->value();
	double ch = ui.ch->value();
	if (cx + cy + cw + ch > 0.0001)
	{
		isClip = true;
		DPE_Filter::Get()->Add(XTask{ XTASK_CLIP,
		{ cx,cy,cw,ch } });
		double w = DPE_VideoThread::Get()->width;
		double h = DPE_VideoThread::Get()->height;
		DPE_Filter::Get()->Add(XTask{ XTASK_RESIZE, { w, h } });
	}

	bool isPy = false;
	int down = ui.pydown->value();
	int up = ui.pyup->value();
	if (up > 0)
	{
		isPy = true;
		DPE_Filter::Get()->Add(XTask{ XTASK_PYUP, { (double)up } });
		int w = DPE_VideoThread::Get()->width;
		int h = DPE_VideoThread::Get()->height;
		for (int i = 0; i < up; i++)
		{
			h = h * 2;
			w = w * 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}
	if (down > 0)
	{
		isPy = true;
		DPE_Filter::Get()->Add(XTask{ XTASK_PYDOWN, { (double)down } });
		int w = DPE_VideoThread::Get()->width;
		int h = DPE_VideoThread::Get()->height;
		for (int i = 0; i < up; i++)
		{
			h = h / 2;
			w = w / 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}

	double w = ui.width->value();
	double h = ui.height->value();
	if (!isMerge && !isClip && !isPy && ui.width->value() > 0 && ui.height->value() > 0)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_RESIZE, { w, h } });
	}

	if (ui.bright->value() > 0 ||
		ui.contrast->value() > 1)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_GAIN,
		{ (double)ui.bright->value(), ui.contrast->value() }
			});
	}

	if (ui.color->currentIndex() == 1)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_GRAY });
		isColor = false;
	}

	if (ui.rotate->currentIndex() == 1)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_ROTATE90 });
	}
	else if (ui.rotate->currentIndex() == 2)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_ROTATE180 });
	}
	else if (ui.rotate->currentIndex() == 3)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_ROTATE270 });
	}

	if (ui.flip->currentIndex() == 1)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_FLIPX });
	}
	else if (ui.flip->currentIndex() == 2)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_FLIPY });
	}
	else if (ui.flip->currentIndex() == 3)
	{
		DPE_Filter::Get()->Add(XTask{ XTASK_FLIPXY });
	}

	if (isMark)
	{
		double x = ui.mx->value();
		double y = ui.my->value();
		double a = ui.ma->value();
		DPE_Filter::Get()->Add(XTask{ XTASK_MARK, {x,y,a} });
	}

	if (isBlend)
	{
		double a = ui.ba->value();
		DPE_Filter::Get()->Add(XTask{ XTASK_BLEND, { a } });
	}

	if (isMerge)
	{
		double h2 = DPE_VideoThread::Get()->height2;
		double h1 = DPE_VideoThread::Get()->height;
		int w = DPE_VideoThread::Get()->width2 * (h2 / h1);
		DPE_Filter::Get()->Add(XTask{ XTASK_MERGE });
		ui.width->setValue(DPE_VideoThread::Get()->width + w);
		ui.height->setValue(h1);
	}
}