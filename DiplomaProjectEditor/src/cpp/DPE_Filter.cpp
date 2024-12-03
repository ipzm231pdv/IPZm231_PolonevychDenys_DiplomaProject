#include "DPE_Filter.h"
#include <QThread>
#include <QMutex>
#include "DPE_ImagePro.h"
class CDPE_Filter :public DPE_Filter
{
public:
	std::vector<XTask> tasks;
	QMutex mutex;
	cv::Mat Filter(cv::Mat mat1, cv::Mat mat2)
	{
		mutex.lock();
		DPE_ImagePro p;
		p.Set(mat1, mat2);
		for (int i = 0; i < tasks.size(); i++)
		{
			switch (tasks[i].type)
			{
			case XTASK_GAIN:
				p.Gain(tasks[i].para[0], tasks[i].para[1]);
				break;
			case XTASK_ROTATE90:
				p.Rotate90();
				break;
			case XTASK_ROTATE180:
				p.Rotate180();
				break;
			case XTASK_ROTATE270:
				p.Rotate270();
				break;

			case XTASK_FLIPX:
				p.FlipX();
				break;
			case XTASK_FLIPY:
				p.FlipY();
				break;
			case XTASK_FLIPXY:
				p.FlipXY();
				break;
			case XTASK_RESIZE:
				p.Resize(tasks[i].para[0], tasks[i].para[1]);
				break;

			case XTASK_PYDOWN:
				p.PyDown(tasks[i].para[0]);
				break;
			case XTASK_PYUP:
				p.PyUp(tasks[i].para[0]);
				break;
			case XTASK_CLIP:
				p.Clip(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2], tasks[i].para[3]);
				break;
			case XTASK_GRAY:
				p.Gray();
				break;
			case XTASK_MARK:
				p.Mark(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2]);
				break;
			case XTASK_BLEND:
				p.Blend(tasks[i].para[0]);
				break;
			case XTASK_MERGE:
				p.Merge();
				break;
			default:
				break;
			}
		}
		cv::Mat re = p.Get();
		mutex.unlock();
		return re;
	}
	void Add(XTask task)
	{
		mutex.lock();
		tasks.push_back(task);
		mutex.unlock();
	}
	void Clear()
	{
		mutex.lock();
		tasks.clear();
		mutex.unlock();
	}
};

DPE_Filter* DPE_Filter::Get()
{
	static CDPE_Filter cx;
	return &cx;
}
DPE_Filter::DPE_Filter()
{
}


DPE_Filter::~DPE_Filter()
{
}