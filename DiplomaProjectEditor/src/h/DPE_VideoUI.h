#pragma once

#include <QtWidgets/QWidget>
#include "ui_DPE_VideoUI.h"

class DPE_VideoUI : public QWidget
{
	Q_OBJECT

public:
	DPE_VideoUI(QWidget* parent = Q_NULLPTR);
	void timerEvent(QTimerEvent* e);
public slots:
	void Open();
	void Play();
	void Pause();

	void SliderPress();
	void SliderRelease();

	void SetPos(int);
	void Left(int pos);
	void Right(int pos);


	void Set();


	void Export();


	void ExportEnd();


	void Mark();


	void Blend();


	void Merge();


private:
	Ui::DPE_VideoUIClass ui;
};