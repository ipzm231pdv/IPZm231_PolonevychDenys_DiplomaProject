#pragma once
#include <opencv2/core.hpp>
class DPE_ImagePro
{
public:
	void Set(cv::Mat mat1, cv::Mat mat2);

	cv::Mat Get() { return des; }

	void Gain(double bright, double contrast);

	void Rotate90();
	void Rotate180();
	void Rotate270();

	void FlipX();
	void FlipY();
	void FlipXY();

	void Resize(int width, int height);

	void PyDown(int count);
	void PyUp(int count);

	void Clip(int x, int y, int w, int h);


	void Gray();

	void Mark(int x, int y, double a);

	void Blend(double a);

	void Merge();

	DPE_ImagePro();
	~DPE_ImagePro();
private:
	cv::Mat src1, src2;

	cv::Mat des;
};