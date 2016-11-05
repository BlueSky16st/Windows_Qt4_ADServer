#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QSplashScreen>
#include <QTextCodec>

//#define QT_Static

#ifdef QT_Static
#include <QtPlugin>

Q_IMPORT_PLUGIN(qcncodecs)
#endif

int main(int argc, char * argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icon.ico"));
	
	QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath()) + QDir::separator() + "plugins");
	qApp->addLibraryPath(strLibPath);
	
	QSplashScreen * splash = new QSplashScreen;
	splash->setPixmap(QPixmap(":/clientLogo.png"));
	splash->show();
	
	MainWindow window;
	
	QEventLoop eventLoop;
	QTimer timer;
	timer.setSingleShot(true);
	timer.setInterval(2000);
	QObject::connect(&timer, SIGNAL(timeout()),
					 &eventLoop, SLOT(quit()));
	QObject::connect(&timer, SIGNAL(timeout()),
					 splash, SLOT(close()));
	timer.start();
	eventLoop.exec();
	
	window.showMaximized();
	
	return app.exec();
}
