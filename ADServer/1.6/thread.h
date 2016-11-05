#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QStringList>
#include <QMutex>

class QTimer;
class ClientSocket;
class File;
class ThreadMessageQueue;

/*
 * 线程类
 * 用来接收客户端的请求信息并发送相应文件给客户端。
 * 
*/
class Thread : public QThread
{
	Q_OBJECT
	
public:
	static ThreadMessageQueue m_messageQueue;		// 处理线程产生的消息
	
	Thread(QList<File *> & fileList, 
		   QObject * parent = 0);					// 构造函数
	~Thread();										// 析构函数
	
	void addClient(int socketId);					// 添加客户端
	int curClientCount() const;						// 返回当前线程连接的客户端数
	
protected:
	void run();										// 线程运行函数
	
signals:
	void clientDisconnect();						// 客户端断开连接信号
	void sendMessage();								// 发送消息信号
	
private slots:
	void deleteDisconnectedClient();				// 删除断开连接的客户端
	void addMessage(const QString & msg);			// 添加新的线程消息
	
private:
	QList<ClientSocket *> m_clientList;				// 客户端列表
	QList<File *> m_fileList;						// 广告文件列表
	QMutex m_mutexClientConnect;					// 客户端连接时互斥量
	QTimer * m_timer;								// 定时器
	
};

/*
 * 线程消息类
 * 用来发送来自不同线程所产生出的消息。
 * 
*/
class ThreadMessageQueue : public QObject
{
	Q_OBJECT
	
public:
	void addMessage(const QString & msg);			// 添加消息
	
signals:
	void sendMessage(QString);						// 发送消息信号
	
public slots:
	void timeOut();									// 定时器到时执行函数
	
private:
	QStringList m_messageList;						// 消息队列
	QMutex m_mutex;									// 消息队列互斥量
	
};

#endif // THREAD_H
