#include "thread.h"
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTimer>
#include "tcpserver.h"

// 定义静态成员变量
ThreadMessageQueue Thread::m_messageQueue;

/*
 * 构造函数
 * 参数1：引用，广告文件列表。
 * 参数2：父对象，默认为0.
 * 
*/
Thread::Thread(QList<File *> & fileList, 
			   QObject * parent /* = 0 */) : QThread(parent)
{
	m_fileList = fileList;
	
}

/*
 * 析构函数
 * 停止线程，在停止之前等待其它函数操作完成。
 * 
*/
Thread::~Thread()
{
	quit();
	wait();
	
}

/*
 * 公有函数
 * 参数1：客户端socket标识。
 * 连接客户端，将客户端移至线程进行操作。
 * 
*/
void Thread::addClient(int socketId)
{
	ClientSocket * socket = new ClientSocket(m_fileList);
	socket->setSocketDescriptor(socketId);
	
	m_messageQueue.addMessage(tr("客户端(%1:%2)已连接。")
							  .arg(socket->peerAddress().toString())
							  .arg(socket->peerPort()));
	
	connect(socket, SIGNAL(haveMessage(QString)),
			this, SLOT(addMessage(QString)), Qt::DirectConnection);
	connect(socket, SIGNAL(disconnected()),
			this, SLOT(deleteDisconnectedClient()));
	
	socket->moveToThread(this);
	
	QMutexLocker locker(&m_mutexClientConnect);
	m_clientList.append(socket);
	
}

/*
 * 公有函数
 * 返回当前线程连接的客户端数。
 * 
*/
int Thread::curClientCount() const
{
	return m_clientList.size();
	
}

/*
 * 保护函数，重写函数
 * 启用线程运行的函数。
 * 当线程调用quit()时该函数会断开所有的客户端连接。
 * 
*/
void Thread::run()
{
	m_timer = new QTimer;
	
	connect(m_timer, SIGNAL(timeout()),
			&m_messageQueue, SLOT(timeOut()));
	
	m_timer->start(300);
	
	exec();
	
	foreach(ClientSocket * socket, m_clientList)
	{
		socket->disconnectFromHost();
		socket->close();
	}
		
	m_clientList.clear();
	
}

/*
 * 私有槽
 * 删除已断开连接的客户端socket。
 * 
*/
void Thread::deleteDisconnectedClient()
{
	ClientSocket * socket = static_cast<ClientSocket *>(sender());
	
	socket->abort();
	
	QMutexLocker locker(&m_mutexClientConnect);
	for(QList<ClientSocket *>::iterator it = m_clientList.begin();
		it != m_clientList.end();)
	{
		if(!(*it)->isValid())
		{
			it = m_clientList.erase(it);
			
			break;
		}
		it++;
	}
	
	emit clientDisconnect();
	
}

/*
 * 私有槽
 * 添加线程消息。
 * 
*/
void Thread::addMessage(const QString & msg)
{
	m_messageQueue.addMessage(msg);
	
}

/*
 * 公有函数
 * 添加消息到队列。
 * 
*/
void ThreadMessageQueue::addMessage(const QString & msg)
{
	QMutexLocker locker(&m_mutex);
	m_messageList.append(msg);
	
}

/*
 * 私有槽
 * 定时器到时执行函数，将消息发送到主窗口。
 * 
*/
void ThreadMessageQueue::timeOut()
{
	if(m_messageList.isEmpty())
		return;
	
	QMutexLocker locker(&m_mutex);
	
	foreach(QString msg, m_messageList)
	{
		emit sendMessage(msg);
	}
	m_messageList.clear();
	
}
