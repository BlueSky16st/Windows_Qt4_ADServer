#include "tcpserver.h"
#include <QtNetwork>
#include <QTextStream>
#include <QFile>
#include "file.h"
#include "thread.h"

/*
 * 构造函数
 * 参数1：广告文件列表。
 * 参数2：父对象，默认为0。
 * 创建线程来处理客户端。
 * 
*/
TCPServer::TCPServer(QList<File *> fileList, 
					 QObject * parent /* = 0 */) : QTcpServer(parent)
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

/*
 * 公有函数
 * 关闭现有线程并重新创建新线程。
 * 
*/
void TCPServer::closeClient()
{
	for(int i = 0; i < const_ThreadCount; i++)
	{
		delete m_thread[i];
		
		m_thread[i] = new Thread(m_fileList);
		
		connect(m_thread[i], SIGNAL(clientDisconnect()),
				this, SLOT(cilentDisconnected()));
	}
	m_clientCount = 0;
	
}

/*
 * 私有槽
 * 客户端断开连接时，更新客户端连接数并发送信号。
 * 
*/
void TCPServer::cilentDisconnected()
{
	m_clientCount--;
	emit clientCount(m_clientCount);
	
}

/*
 * 私有函数
 * 客户端断开连接时，更新客户端连接数。
 * 
*/
void TCPServer::incomingConnection(int socketId)
{
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

/*
 * 构造函数
 * 参数1：引用，广告文件列表
 * 参数2：父对象，默认为0
 * 
*/
ClientSocket::ClientSocket(QList<File *> & fileList, 
						   QObject * parent /* = 0 */) : QTcpSocket(parent)
{
	m_fileList = fileList;
	
	connect(this, SIGNAL(readyRead()),
			this, SLOT(readClient()));
	connect(this, SIGNAL(disconnected()),
			this, SLOT(clientDisconnect()));
	
}

/*
 * 私有槽
 * 读取客户端的数据。
 * 
*/
void ClientSocket::readClient()
{
	QTextStream in(this);
	in.setCodec(QTextCodec::codecForName("GBK"));
	QString str;
	
	if(isValid())
	{
		str = in.readAll();
		
		// debug
		// emit haveMessage(str);
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
				
				if(write(byteStr) == -1)
					return;
				flush();
				
				/*
				if(isValid())
				{
					write(byteStr);
					flush();
				}
				else
				{
					return;
				}
				*/
				
				if(sendFileToClient(file->filePath()))
				{
					if(write("ReEOF") == -1)
						return;
					flush();
					
					/*
					if(isValid())
					{
						write("ReEOF");
						flush();
					}
					else
					{
						return;
					}
					*/
					
					emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
									 .arg(peerAddress().toString())
									 .arg(peerPort())
									 .arg(file->filePath()));
				}
				else
				{
					if(write("FileError") == -1)
						return;
					flush();
					
					/*
					if(isValid())
					{
						write("FileError");
						flush();
					}
					else
					{
						return;
					}
					*/
					
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
		
		if(write("FND") == -1)
			return;
		flush();
		
		/*
		if(isValid())
		{
			write("FND");
			flush();
		}
		else
		{
			return;
		}
		*/
		
	}
	else if(QFileInfo(str).exists())
	{
		if(sendFileToClient(str))
		{
			if(write("ReEOF") == -1)
				return;
			flush();
			
			/*
			if(isValid())
			{
				write("ReEOF");
				flush();
			}
			else
			{
				return;
			}
			*/
			
			emit haveMessage(tr("已发送：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
		else
		{
			if(write("FileError") == -1)
				return;
			flush();
			
			/*
			if(isValid())
			{
				write("FileError");
				flush();
			}
			else
			{
				return;
			}
			*/
			
			emit haveMessage(tr("发送失败：客户端(%1:%2)请求文件[%3]。")
							 .arg(peerAddress().toString())
							 .arg(peerPort())
							 .arg(str));
		}
	}
	else
	{
		if(write("FileError") == -1)
			return;
		flush();
		
		/*
		if(isValid())
		{
			write("FileError");
			flush();
		}
		else
		{
			return;
		}
		*/
	}
	
}

/*
 * 私有槽
 * 客户端断开连接时发送消息。
 * 
*/
void ClientSocket::clientDisconnect()
{
	emit haveMessage(tr("客户端(%1:%2)已断开连接。")
					 .arg(peerAddress().toString())
					 .arg(peerPort()));
	
}

/*
 * 私有函数
 * 参数1：文件路径。
 * 返回值：如果发送成功，返回true，否则返回false。
 * 发送文件数据给指定客户端。
 * 
*/
bool ClientSocket::sendFileToClient(const QString & filePath)
{
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
		if((n = (int)write(outBlock)) == -1)
		{
			file.close();
			return false;
		}
		
		/*
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
		*/
		
		bytesToWrite -= n;
		
		//清空发送缓冲区
		outBlock.resize(0);
		
	}  
	
	//如果没有发送任何数据，则关闭文件
	file.close();
	return true;
	
}

