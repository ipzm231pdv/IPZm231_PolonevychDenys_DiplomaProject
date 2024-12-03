#pragma once
#include <QOpenGLWidget>
#include <opencv2/core.hpp>
class DPE_VideoWidget :public QOpenGLWidget
{
	Q_OBJECT

public:
	DPE_VideoWidget(QWidget* p);
	void paintEvent(QPaintEvent* e);
	virtual ~DPE_VideoWidget();
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;
};

