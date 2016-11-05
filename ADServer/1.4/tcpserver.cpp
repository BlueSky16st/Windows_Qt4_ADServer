#include "tcpserver.h"
#include <QtNetwork>
#include <QTextStream>
#include <QFile>
#include "file.h"
#include "thread.h"

TCPServer::TCPServer(QList<File *> fileList, QObject * parent) : QTcpServer(parent)
{
	m_clientCount = 0;
	m_fileList = fileList;
	
	for(int i = 0; i < const_ThreadCount; i++)
	{
		m_thread[i] = new Thread(m_fileList);
		
		connect(m_thread[i], SIGNAL(clientDisconnect()),
				this, SLOT(cilentDisconnected()));
	}
	connect(&Thread::m_messageQueue, SIGNAL(sendMessage(QString)),
			this, SIGNAL(haveMessage(QString)));
	
}

void TCPServer::closeClient()
{
	for(int i = 0; i < const_ThreadCount; i++)
	{
		delete m_thread[i];
		
		m_thread[i] = new Thread(m_fileList);
		
		connect(m_thread[i], SIGNAL(clientDisconnect()),
				this, SLOT(cilentDisconnected()));
	}
	
}

void TCPServer::cilentDisconnected()
{
	m_clientCount--;
	emit clientCount(m_clientCount);
	
}

void TCPServer::incomingConnection(int socketId)
{
	/*
	ClientSocket * socket = new ClientSocket(m_fileList, this);
	socket->setSocketDescriptor(socketId);
	
	emit haveMessage(tr("客户端(%1:%2)已连接。")
					 .arg(socket->peerAddress().toString())
					 .arg(socket->peerPort()));
	
	connect(socket, SIGNAL(haveMessage(QString)),
			this, SIGNAL(haveMessage(QString)));
	*/
	
	int minThread = 0;
	for(int i = 1; i < const_ThreadCount; i++)
	{
		if(m_thread[minThread]->curClientCount() > m_thread[i]->curClientCount())
		{
			minThread = i;
		}
	}
	if(!m_thread[minThread]->isRunning())
	{
		m_thread[minThread]->start();
	}
	m_thread[minThread]->addClient(socketId);
	
	m_clientCount++;
	emit clientCount(m_clientCount);
	
}


ClientSocket::ClientSocket(QList<File *> & fileList, QObject * parent) : QTcpSocket(parent)
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
	QString str;
	
	if(isValid())
	{
		str = in.readAll();
	}
	else
	{
		return;
	}
	
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
				
				if(isValid())
				{
					write(byteStr);
					flush();
				}
				else
				{
					return;
				}
				
				if(sendFileToClient(file->filePath()))
				{
					if(isValid())
					{
						write("ReEOF");
						flush();
					}
					else
					{
						return;
					}
					
					emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
									 .arg(peerAddress().toString())
									 .arg(peerPort())
									 .arg(file->filePath()));
				}
				else
				{
					
					if(isValid())
					{
						write("FileError");
						flush();
					}
					else
					{
						return;
					}
					
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
		
		if(isValid())
		{
			write("FND");
			flush();
		}
		else
		{
			return;
		}
		
	}
	else if(QFileInfo(str).exists())
	{
		if(sendFileToClient(str))
		{
			if(isValid())
			{
				write("ReEOF");
				flush();
			}
			else
			{
				return;
			}
			emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
		else
		{
			if(isValid())
			{
				write("FileError");
				flush();
			}
			else
			{
				return;
			}
			
			emit haveMessage(tr("发送失败：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
	}
	else
	{
		if(isValid())
		{
			write("FileError");
			flush();
		}
		else
		{
			return;
		}
		
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
		int n = 0;
		if(isValid())
		{
			n = (int)write(outBlock);
			flush();
		}
		else
		{
			file.close();
			return false;
		}
		
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
	
}
