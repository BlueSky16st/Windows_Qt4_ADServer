#include "mainwindow.h"
#include <QtGui>
#include <QtNetwork>
#include <QTextCodec>
#include <QFile>
#include <phonon>
#include <QTextStream>

#include "parsexml.h"
#include "node.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent)
{
	setupUi(this);
	
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	m_recvFileING = false;
	m_fileNumUnRecv = 0;
	m_ADPlayIndex = 0;
	m_url.clear();
	
	m_timer.setInterval(300);
	
	m_mediaObject = new Phonon::MediaObject(this);
    m_videoWidget = new Phonon::VideoWidget(this);
    Phonon::createPath(m_mediaObject, m_videoWidget);
    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory,
                                          this);
    Phonon::createPath(m_mediaObject, m_audioOutput);	
	
	videoPage->layout()->addWidget(m_videoWidget);
	
	statusLabel = new QLabel;
	windowStatusBar->addWidget(statusLabel);
	
	QFile configFile("config");
	if(!configFile.exists())
	{
		QMessageBox::information(this, tr("广告客户端"),
								 tr("未找到客户端配置文件。"));
	}
	else
	{
		if(!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::information(this, tr("广告客户端"),
									 tr("无法打开客户端配置文件。"));
		}
		else
		{
			QTextStream in(&configFile);
			while(!in.atEnd())
			{
				QString text = in.readLine();
				if(text.startsWith('#'))
					continue;
				if(text.startsWith("ip"))
					m_ip = text.mid(text.indexOf('=') + 1).simplified();
				if(text.startsWith("id"))
					m_id = text.mid(text.indexOf('=') + 1).simplified();
			}
			configFile.close();
		}
		
	}
	
	m_tcpSocket = new QTcpSocket(this);
	m_tcpSocket->connectToHost(m_ip, 5300);
	
	connect(m_tcpSocket, SIGNAL(connected()),
			this, SLOT(sendID()));
	connect(m_tcpSocket, SIGNAL(readyRead()),
			this, SLOT(recvFile()));
	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(error()));
	connect(&m_timer, SIGNAL(timeout()),
			this, SLOT(ADTimeOut()));
	connect(&m_ADPlayTimer, SIGNAL(timeout()),
			this, SLOT(ADPlayTimeOut()));
	connect(m_mediaObject, SIGNAL(aboutToFinish()),
			this, SLOT(playVideo()));
	
}

void MainWindow::showMessage(QString msg)
{
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	//QTextCodec * code= QTextCodec::codecForName("GBK");
	//QString str = code->fromUnicode(msg);
	//statusLabel->setText(str);
	
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	statusLabel->setText(msg);
	
}

void MainWindow::recvFile()
{
	QByteArray inBlock = m_tcpSocket->readAll();
	
	if(inBlock.startsWith("adm"))
	{
		//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
		QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
		
		inBlock.remove(0, 3);
		QByteArray fileName = inBlock.mid(0, inBlock.indexOf(".adm") + 4);
		inBlock.remove(0, fileName.size());
		
		QDir dir(QDir::currentPath());
		if(!dir.exists("cache"))
		{
			dir.mkdir("cache");
		}
		
		if(QFile("cache\\" + fileName).exists())
			QFile("cache\\" + fileName).remove();
		
		m_file = new QFile("cache\\" + fileName);
		m_file->open(QIODevice::WriteOnly);
		
		QString file = fileName;
		
		QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
		showMessage("接收文件:" + file);
		
		if(inBlock.size() > fileName.size())
		{
			if(inBlock.endsWith("ReEOF"))
			{
				inBlock.remove(inBlock.indexOf("ReEOF"), 5);
				
				m_file->write(inBlock);
				m_file->close();
				
				ParseXML * parse = new ParseXML(QFileInfo(*m_file).filePath());
				m_parseFileList.append(parse);
				
				showMessage("接收文件成功:" + file);
				
			}
			else if(inBlock.endsWith("ReEOFFND"))
			{
				inBlock.remove(inBlock.indexOf("ReEOFFND"), 8);
				
				m_file->write(inBlock);
				m_file->close();
				
				ParseXML * parse = new ParseXML(QFileInfo(*m_file).filePath());
				m_parseFileList.append(parse);
				
				showMessage("接收文件成功:" + file);
				
				parseXML();
			}
			else
			{
				m_file->write(inBlock);
			}
		}
		
		return;
	}
	else if(inBlock.endsWith("ReEOF") && inBlock.size() > 5)
	{
		inBlock.remove(inBlock.indexOf("ReEOF"), 5);
		m_file->write(inBlock);
		
		if(m_file->fileName().contains(".adm"))
		{
			ParseXML * parse = new ParseXML(QFileInfo(*m_file).filePath());
			m_parseFileList.append(parse);
		}
		
		m_file->close();
		
		showMessage("接收文件成功:" + QFileInfo(*m_file).fileName());
		
		if(m_recvFileING)
		{
			m_recvFileING = false;
			sendRequest();			
		}
		
		return;
	}
	else if(inBlock.endsWith("ReEOF"))
	{
		
		if(m_file->fileName().contains(".adm"))
		{
			ParseXML * parse = new ParseXML(QFileInfo(*m_file).filePath());
			m_parseFileList.append(parse);
		}
		
		m_file->close();
		
		showMessage("接收文件成功:" + QFileInfo(*m_file).fileName());
		
		if(m_recvFileING)
		{
			m_recvFileING = false;
			sendRequest();			
		}
		
		return;
	}
	else if(inBlock == "FND")
	{
		parseXML();
		return;
	}
	else if(inBlock.endsWith("ReEOFFND"))
	{
		inBlock.remove(inBlock.indexOf("ReEOFFND"), 8);
		
		m_file->write(inBlock);
		m_file->close();
		
		ParseXML * parse = new ParseXML(QFileInfo(*m_file).filePath());
		m_parseFileList.append(parse);
		
		showMessage("接收文件成功:" + QFileInfo(*m_file).fileName());
		
		parseXML();
		
		return;
	}
	else if(inBlock.contains("FileError"))
	{
		if(m_file->isOpen())
		{
			if(!QFileInfo(*m_file).fileName().endsWith(".adm"))
			{
				QStringList fileList = m_curOpXML->fileRecvList();
				
				foreach(QString str, fileList)
				{
					if(str.contains(QFileInfo(*m_file).fileName()))
					{
						m_curOpXML->setRecvFileStatus(str, FILE_Receive_FAIL);
						break;
					}
				}
			}
			
			showMessage("接收文件失败:" + QFileInfo(*m_file).fileName());
			
			m_file->close();
			m_file->remove();
			
			if(m_recvFileING)
			{
				m_recvFileING = false;
				m_fileNumUnRecv++;
				
				sendRequest();			
			}
			
		}
		
		return;
	}
	
	m_file->write(inBlock);
}

bool MainWindow::sendRequest()
{
	QTime time = QTime::currentTime();
	QTime now;
	do
	{
		now = QTime::currentTime();
	} while(time.msecsTo(now) <= 700);
	
	QString filePath = "";
	QString fileDir = "";
	
	bool haveNotReceiveFile = false;
	foreach(ParseXML * parse, m_parseFileList)
	{
		m_curOpXML = parse;
		
		foreach(QString path, m_curOpXML->fileRecvList())
		{
			fileDir = "cache\\" + m_curOpXML->getFileDir(path);
			
			QDir dir(QDir::currentPath());
			if(!dir.exists(fileDir))
			{
				dir.mkdir(fileDir);
			}
			fileDir += "\\";
			path = path.mid(path.indexOf('\\') + 1);
			
			if(!(QFileInfo(fileDir + QFileInfo(path).fileName()).exists()))
			{
				filePath = path;
				haveNotReceiveFile = true;
				break;
			}
		}
		if(haveNotReceiveFile)
			break;
	}
	
	if(!haveNotReceiveFile)
	{
		QString str = "文件已接收完毕。" + (m_fileNumUnRecv != 0 
				? QString("但有%1个文件接收失败。").arg(m_fileNumUnRecv)
				: "");
		
		showMessage(str);
		
		m_curOpXML = NULL;
		playAD();
		
		return true;
	}
	
	QTextCodec * code= QTextCodec::codecForName("GBK");
	QByteArray byteStr = code->fromUnicode(filePath);
	
	m_file = new QFile(fileDir + QFileInfo(filePath).fileName());
	if(!m_file->open(QIODevice::WriteOnly))
	{
		m_curOpXML->setRecvFileStatus(filePath, FILE_Receive_FAIL);
		return false;
	}
	
	m_recvFileING = true;
	m_curOpXML->setRecvFileStatus(filePath, File_Received);
	
	m_tcpSocket->write(byteStr);
	m_tcpSocket->flush();
	
	showMessage("发送文件请求:" + filePath);
	
	return true;
}

void MainWindow::sendID()
{
	showMessage("发送客户端ID。");
	
	QByteArray arr;
	arr.append(m_id);
	
	if(m_tcpSocket->write(arr) == -1)
	{
		QMessageBox::information(0,
								 tr("广告客户端"),
								 tr("发送ID失败"));
		showMessage("");
	}
}

void MainWindow::error()
{
	QMessageBox::information(this,
							 tr("广告客户端"),
							 m_tcpSocket->errorString());
	
}

void MainWindow::ADTimeOut()
{
	ADWidget->hide();
	
	QTime time = QTime::currentTime();
	foreach(ParseXML * parse, m_parseFileList)
	{
		m_curOpXML = parse;
		QList<Scene *> scene = m_curOpXML->scene();
		foreach(Scene * sc, scene)
		{
			if(sc->begin() < time && sc->end() > time)
			{
				m_curPlayScene = sc;
				ADPlayTimeOut();
				m_ADPlayTimer.start(sc->interval() * 1000);
				
				m_timer.stop();
				return;
			}
		}
	}
	
	showMessage("当前无广告。");
	
}

void MainWindow::ADPlayTimeOut()
{
	QTime time = QTime::currentTime();
	ADWidget->show();
	
	m_mediaObject->stop();
	m_mediaObject->clearQueue();
	
	if(time > m_curPlayScene->end())
	{
		m_curPlayScene = NULL;
		m_ADPlayIndex = 0;
		showADLabel->clear();
		
		m_ADPlayTimer.stop();
		m_timer.start();
		return;
	}
	
	QList<Node *> nodeList = m_curPlayScene->nodeList();
	Node * node = nodeList.at(m_ADPlayIndex);
	switch(node->type())
	{
	case Type_text:
	{
		ADWidget->setCurrentIndex(0);
		
		showADLabel->setText(static_cast<TextNode *>(node)->text());
		
		showMessage("正在播放：文本广告。");
		setCursor(QCursor(Qt::ArrowCursor));
		windowCentralWidget->setToolTip("");
		
		{
			QMutexLocker locker(&m_urlMutex);
			m_url.clear();
		}
		
		break;
	}
		
	case Type_image:
	{
		ADWidget->setCurrentIndex(0);
		
		if(!QFileInfo("cache\\" + m_curPlayScene->name() + "\\" 
					  + QFileInfo(
						  static_cast<MediaNode *>(node)
						  ->file()).fileName()).exists())
		{
			m_ADPlayIndex = (m_ADPlayIndex + 1) % nodeList.size();
			return;
		}
		
		QPixmap pixmap = QPixmap("cache\\" + m_curPlayScene->name() + "\\" 
								 + QFileInfo(
									 static_cast<MediaNode *>(node)
									 ->file()).fileName());
		pixmap = pixmap.scaled(width(), height() - windowStatusBar->height());
		showADLabel->setPixmap(pixmap);
		
		showMessage("正在播放：图片广告：" + QFileInfo(
						static_cast<MediaNode *>(node)
						->file()).fileName());
		setCursor(QCursor(Qt::ArrowCursor));
		
		{
			QMutexLocker locker(&m_urlMutex);
			m_url = static_cast<MediaNode *>(node)->url();
		}
		
		break;
	}
		
	case Type_video:
	{
		if(!QFileInfo("cache\\" + m_curPlayScene->name() + "\\" 
					  + QFileInfo(
						  static_cast<MediaNode *>(node)
						  ->file()).fileName()).exists())
		{
			m_ADPlayIndex = (m_ADPlayIndex + 1) % nodeList.size();
			return;
		}
		
		m_mediaObject->setCurrentSource("cache\\" + m_curPlayScene->name() + "\\" 
										+ QFileInfo(
											static_cast<MediaNode *>(node)
											->file()).fileName());
		if(!m_mediaObject->hasVideo())
		{
			QEventLoop eventLoop;
			QTimer timer;
			timer.setSingleShot(true);
			timer.setInterval(3000);
			connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
			connect(m_mediaObject, SIGNAL(hasVideoChanged(bool)),
					&eventLoop, SLOT(quit()));
			timer.start();
			eventLoop.exec();
		}
		
		if(m_mediaObject->hasVideo())
		{
			ADWidget->setCurrentIndex(1);
			m_mediaObject->play();
			
			showMessage("正在播放：视频广告：" + QFileInfo(
							static_cast<MediaNode *>(node)
							->file()).fileName());
		}
		
		setCursor(QCursor(Qt::ArrowCursor));
		
		{
			QMutexLocker locker(&m_urlMutex);
			m_url = static_cast<MediaNode *>(node)->url();
		}
		
		break;
	}
		
	}
	
	m_ADPlayIndex = (m_ADPlayIndex + 1) % nodeList.size();
	
}

void MainWindow::playVideo()
{
	m_mediaObject->enqueue(m_mediaObject->currentSource());
	
}

void MainWindow::mouseMoveEvent(QMouseEvent *)
{
	QMutexLocker locker(&m_urlMutex);
	if(QUrl(m_url).isValid())
		setCursor(QCursor(Qt::PointingHandCursor));
	
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
	if(event->button() == Qt::LeftButton && !m_url.isEmpty())
	{
		QDesktopServices::openUrl(QUrl(m_url)); 
	}
	
}

void MainWindow::parseXML()
{
	foreach(ParseXML * parse, m_parseFileList)
	{
		parse->parse();
		m_curOpXML = parse;
		
		foreach(Scene * sc, m_curOpXML->scene())
		{
			foreach(Node * node, sc->nodeList())
			{
				if(node->type() == Type_text)
					continue;
				
				m_curOpXML->addRecvFile(static_cast<MediaNode *>(node)->file(),
										sc->name());
			}
		}
	}
	sendRequest();
	
}

void MainWindow::playAD()
{
	if(!m_timer.isActive())
	{
		m_timer.start();
	
		showMessage("开始播放广告。");
	}
	
}


