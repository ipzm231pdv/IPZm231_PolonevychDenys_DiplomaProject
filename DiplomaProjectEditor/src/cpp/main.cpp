#include "DPE_VideoUI.h"
#include <QtWidgets/QApplication>
//#include "XAudio.h"
int main(int argc, char* argv[])
{
    //XAudio::Get()->ExportA("test3.mp4", "test3.mp3");
    //XAudio::Get()->Merge("test3.avi", "test3.mp3", "out3.avi");

    QApplication a(argc, argv);
    DPE_VideoUI w;
    w.show();
    w.setWindowTitle("Полоневич Денис. Відеоредактор (Дипломна робота) ");
    return a.exec();
}