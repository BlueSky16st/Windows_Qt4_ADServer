#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "thread.h"

class File;

// 启用线程数
const int const_ThreadCount = 10;

/*
 * TCP服务器类
 * 监听所有客户端的连接并将客户端加入到线程。
 * 
*/
class TCPServer : public QTcpServer
{
	Q_OBJECT
	
public:
	TCPServer(QList<File *> fileList, 
			  QObject * parent = 0);			// 构造函数
	
	void closeClient();							// 关闭客户端
	
signals:
	void haveMessage(QString msg);				// 新消息信号
	void clientCount(int count);				// 已连接客户端数信号
	
private slots:
	void cilentDisconnected();					// 客户端断开连接
	
private:
	void incomingConnection(int socketId);
	
	QList<File *> m_fileList;					// 广告文件列表
	Thread * m_thread[const_ThreadCount];		// 线程数组
	int m_clientCount;							// 客户端连接数
	
};

/*
 * 客户端Socket类
 * 接收客户端的发送数据和发送文件数据给客户端。
 * 
*/
class ClientSocket : public QTcpSocket
{
	Q_OBJECT
	
public:
	ClientSocket(QList<File *> & fileList, 
				 QObject * parent = 0);			// 构造函数
	
signals:
	void haveMessage(QString msg);				// 新消息信号
	
private slots:
	void readClient();							// 读取客户端的数据
	void clientDisconnect();					// 客户端断开连接
	
private:
	bool sendFileToClient(const QString & filePath);	// 发送文件数据给客户端
	
	QList<File *> m_fileList;					// 广告文件列表
	
};

#endif // TCPSERVER_H
