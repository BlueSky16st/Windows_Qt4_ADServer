#include "tcpserver.h"
#include <QtNetwork>
#include <QTextStream>
#include <QFile>
#include "file.h"

TCPServer::TCPServer(QList<File *> fileList, QObject * parent) : QTcpServer(parent)
{
	m_fileList = fileList;
}

void TCPServer::incomingConnection(int socketId)
{
	ClientSocket * socket = new ClientSocket(m_fileList, this);
	socket->setSocketDescriptor(socketId);
	
	emit haveMessage(tr("客户端(%1:%2)已连接。")
					 .arg(socket->peerAddress().toString())
					 .arg(socket->peerPort()));
	
	connect(socket, SIGNAL(haveMessage(QString)),
			this, SIGNAL(haveMessage(QString)));
	
}


ClientSocket::ClientSocket(QList<File *> fileList, QObject * parent) : QTcpSocket(parent)
{
	m_fileList = fileList;
	
	connect(this, SIGNAL(readyRead()),
			this, SLOT(readClient()));
	connect(this, SIGNAL(disconnected()),
			this, SLOT(clientDisconnect()));
	
}

void ClientSocket::readClient()
{
	QTextStream in(this);
	in.setCodec(QTextCodec::codecForName("GBK"));
	QString str = in.readAll();
	
	bool ok;
	str.toInt(&ok);
	if(ok)
	{
		foreach(File * file, m_fileList)
		{
			if(file->sendID().split(' ').indexOf(str) != -1)
			{
				QTextCodec * code= QTextCodec::codecForName("GBK");
				QByteArray byteStr = "adm" + code->fromUnicode(file->fileName());
				write(byteStr);
				flush();
				
				if(sendFileToClient(file->filePath()))
				{
					QTime time = QTime::currentTime();
					QTime now;
					do
					{
						now = QTime::currentTime();
					} while(time.msecsTo(now) <= 700);
					
					write("ReEOF");
					flush();
					emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
									 .arg(peerAddress().toString())
									 .arg(peerPort())
									 .arg(file->filePath()));
				}
				else
				{
					write("FileError");
					flush();
					emit haveMessage(tr("发送失败：客户端(%1:%2)请求文件[%3]。")
									 .arg(peerAddress().toString())
									 .arg(peerPort())
									 .arg(file->filePath()));
				}
			}
			
			QTime time = QTime::currentTime();
			QTime now;
			do
			{
				now = QTime::currentTime();
			} while(time.msecsTo(now) <= 700);
			
		}
		write("FND");
		flush();
		
	}
	else if(QFileInfo(str).exists())
	{
		if(sendFileToClient(str))
		{
			write("ReEOF");
			flush();
			emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
		else
		{
			write("FileError");
			flush();
			emit haveMessage(tr("发送失败：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
	}
	else
	{
		write("FileError");
		flush();
		
		//emit haveMessage("ERR:" + str);
	}
	
}

bool ClientSocket::sendFileToClient(const QString & filePath)
{
	/*
	// 发送文本文件
	QFile file(filePath);
	if(!file.open(QFile::ReadOnly | QFile::Text))
		return false;
	
	QTextStream in(&file);
	in.setCodec(QTextCodec::codecForName("GB2312"));
	
	while(!in.atEnd())
	{
		QTextCodec * code= QTextCodec::codecForName("GBK");
		QString str = in.readAll();
		QByteArray byteStr = code->fromUnicode(str);
		
		write(byteStr);
	}
	
	file.close();
	*/
	
	QFile file(filePath);
	if(!file.open(QFile::ReadOnly))
		return false;
	
	qint64 bytesToWrite = file.size();	//还剩数据大小
	qint64 loadSize = 1024;				//缓冲区大小
	QByteArray outBlock;				//缓存一次发送的数据
	 
	
	//已经发送数据的大小
	while(bytesToWrite > 0) //如果还有数据待发送
	{
		//每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足1KB，
		//就发送剩余数据的大小
		//从文件中读取数据
		outBlock = file.read(qMin(bytesToWrite, loadSize));
		
		//发送完一次数据后还剩余数据的大小
		int n = (int)write(outBlock);
		flush();
		
		bytesToWrite -= n;
		
		//清空发送缓冲区
		outBlock.resize(0);
		
	}  
	
	//如果没有发送任何数据，则关闭文件
	file.close();
	return true;
}

void ClientSocket::clientDisconnect()
{
	emit haveMessage(tr("客户端(%1:%2)已断开连接。")
					 .arg(peerAddress().toString())
					 .arg(peerPort()));
	
	deleteLater();
}
