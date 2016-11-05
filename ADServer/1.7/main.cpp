/****************************************************
 *													*
 * Project created by QtCreator 2016-09-03 15:07:39	*
 *													*
 ****************************************************/
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QDir>

/*
 * 如果采用静态编译，则定义QT_Static
 * 
*/
//#define QT_Static

#ifdef QT_Static
#include <QtPlugin>

Q_IMPORT_PLUGIN(qcncodecs)
#endif

/*
 * 主函数
 * 
*/
int main(int argc, char * argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icon.ico"));
	
	QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath()) + QDir::separator() + "plugins");
	qApp->addLibraryPath(strLibPath);
	
	MainWindow window;
	window.show();
	
	return app.exec();
}
