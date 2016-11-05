#include "mainwindow.h"
#include <QApplication>

//#define QT_Static

#ifdef QT_Static
#include <QtPlugin>

Q_IMPORT_PLUGIN(qcncodecs)
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	
	return a.exec();
}
