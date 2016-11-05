#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class File;

class TCPServer : public QTcpServer
{
	Q_OBJECT
	
public:
	TCPServer(QList<File *> fileList, QObject * parent = 0);
	
signals:
	void haveMessage(QString msg);
	
private:
	void incomingConnection(int socketId);
	QList<File *> m_fileList;
	
};

class ClientSocket : public QTcpSocket
{
	Q_OBJECT
	
public:
	ClientSocket(QList<File *> fileList, QObject * parent = 0);
	
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
