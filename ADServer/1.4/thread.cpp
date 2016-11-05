#include "thread.h"
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTimer>
#include "tcpserver.h"

ThreadMessageQueue Thread::m_messageQueue;

Thread::Thread(QList<File *> & fileList, QObject * parent) : QThread(parent)
{
	m_fileList = fileList;
	
}

Thread::~Thread()
{
	quit();
	wait();
	
}

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

int Thread::curClientCount() const
{
	return m_clientList.size();
	
}

void Thread::run()
{
	m_timer = new QTimer;
	
	connect(m_timer, SIGNAL(timeout()),
			&m_messageQueue, SLOT(timeOut()));
	
	m_timer->start(100);
	
	exec();
	
	foreach(ClientSocket * socket, m_clientList)
	{
		socket->disconnectFromHost();
		socket->close();
	}
		
	m_clientList.clear();
	
}

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

void Thread::addMessage(const QString & msg)
{
	m_messageQueue.addMessage(msg);
	
}

void ThreadMessageQueue::addMessage(const QString & msg)
{
	QMutexLocker locker(&m_mutex);
	m_messageList.append(msg);
	
}

void ThreadMessageQueue::timeOut()
{
	if(m_messageList.isEmpty())
		return;
	
	QMutexLocker locker(&m_mutex);
	
	QString msg = m_messageList.takeFirst();
	emit sendMessage(msg);
	
}
