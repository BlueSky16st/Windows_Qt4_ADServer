#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QStringList>
#include <QMutex>

class QTimer;
class ClientSocket;
class File;
class ThreadMessageQueue;

class Thread : public QThread
{
	Q_OBJECT
	
public:
	static ThreadMessageQueue m_messageQueue;
	
	Thread(QList<File *> & fileList, QObject * parent = 0);
	~Thread();
	
	void addClient(int socketId);
	
	int curClientCount() const;	// 返回当前线程连接的客户端数
	
protected:
	void run();
	
signals:
	void clientDisconnect();
	void sendMessage();
	
private slots:
	void deleteDisconnectedClient();
	void addMessage(const QString & msg);
	
private:
	QList<ClientSocket *> m_clientList;
	QList<File *> m_fileList;
	QMutex m_mutexClientConnect;
	
	QTimer * m_timer;
	
};

class ThreadMessageQueue : public QObject
{
	Q_OBJECT
	
public:
	void addMessage(const QString & msg);
	
signals:
	void sendMessage(QString);
	
public slots:
	void timeOut();
	
private:
	QStringList m_messageList;
	QMutex m_mutex;
	
};

#endif // THREAD_H
