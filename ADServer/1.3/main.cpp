#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

#define QT_Static

#ifdef QT_Static
#include <QtPlugin>

Q_IMPORT_PLUGIN(qcncodecs)
#endif


int main(int argc, char * argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icon.ico"));  
	
	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale());
	
	MainWindow window;
	window.show();
	
	return app.exec();
}
