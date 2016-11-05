#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "thread.h"

class File;

const int const_ThreadCount = 10;

class TCPServer : public QTcpServer
{
	Q_OBJECT
	
public:
	TCPServer(QList<File *> fileList, QObject * parent = 0);
	
	void closeClient();
	
signals:
	void haveMessage(QString msg);
	void clientCount(int count);
	
private slots:
	void cilentDisconnected();
	
private:
	void incomingConnection(int socketId);
	QList<File *> m_fileList;
	Thread * m_thread[const_ThreadCount];
	
	int m_clientCount;
	
};

class ClientSocket : public QTcpSocket
{
	Q_OBJECT
	
public:
	ClientSocket(QList<File *> & fileList, QObject * parent = 0);
	
signals:
	void haveMessage(QString msg);
	
private slots:
	void readClient();
	void clientDisconnect();
	
private:
	bool sendFileToClient(const QString & filePath);
	
	QList<File *> m_fileList;
	
};

#endif // TCPSERVER_H
