#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

#include <QList>
#include <QTimer>

class QTcpSocket;
class QLabel;
class QFile;
class ParseXML;
class Scene;

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT
	
public:
	MainWindow(QWidget * parent = 0);
	
private slots:
	void showMessage(QString msg);
	
	void recvFile();
	bool sendRequest();
	void sendID();
	
	void error();
	
	void ADTimeOut();			// 开始新广告节目播放的定时器
	void ADPlayTimeOut();		// 广告轮流播放的定时器
	
private:
	void parseXML();
	void playAD();
	
	QLabel * statusLabel;
	
	QTcpSocket * m_tcpSocket;
	
	QList<ParseXML *> m_parseFileList;	// 已得到的广告文件
	
	QFile * m_file;
	ParseXML * m_curOpXML;	// 当前正在做处理的广告文件所属的XML文件
	
	bool m_recvFileING;		// 标识有媒体文件正在从服务器端接收
	int m_fileNumUnRecv;	// 标识接收失败的文件数
	
	QTimer m_timer;			// 开始播放广告节目的定时器
	QTimer m_ADPlayTimer;	// 轮流播放广告的定时器
	
	Scene * m_curPlayScene;	// 当前正在播放的广告节点
	int m_ADPlayIndex;		// 当前正在播放第几个广告
	
};

#endif // MAINWINDOW_H
