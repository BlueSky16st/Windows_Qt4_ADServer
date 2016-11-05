#include "parsexml.h"
#include "node.h"

#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QTextCodec>
#include <QtXml>

#include <iostream>

using std::cout;
using std::endl;

ParseXML::ParseXML(const QString & filePath) : m_filePath(filePath)
{
	
}

ParseXML::~ParseXML()
{
	qDeleteAll(m_scene);
	m_scene.clear();
	
}

void ParseXML::setFilePath(const QString & filePath)
{
	m_filePath = filePath;
}

QString ParseXML::filePath() const
{
	return m_filePath;
}

bool ParseXML::parse()
{
	qDeleteAll(m_scene);
	m_scene.clear();
	m_doc.clear();
	
	QFile file(m_filePath);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		cout << qPrintable(QString("错误：读取文件[%1]失败\n原因：%2")
						   .arg(QFileInfo(m_filePath).fileName())
						   .arg(file.errorString()));
		return false;
	}
	
	QString errorStr;
	int errorLine;
	int errorColumn;
	
	if(!m_doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
	{
		cout << qPrintable(QString("错误：在第%1行，第%2列\n原因：%2")
							.arg(errorLine)
							.arg(errorColumn)
							.arg(errorStr));
		return false;
	}
	
	QDomElement root = m_doc.documentElement();
	if(root.tagName() != "menu")
	{
		cout << qPrintable(QString("错误：该文件不是一个ADM文件"));
		return false;
	}
	
	parseScene(root);
	
	return true;
	
}

QList<Scene *> ParseXML::scene()
{
	return m_scene;
	
}

void ParseXML::addRecvFile(QString filePath, QString fileDir)
{
	FileRecv * fileRecv = new FileRecv(filePath, fileDir);
	m_fileRecv.append(fileRecv);
	
}

void ParseXML::setRecvFileStatus(QString filePath, FileStatus status)
{
	foreach(FileRecv * file, m_fileRecv)
	{
		if(file->filePath() == filePath
				&& file->status() == FILE_Not_Receive)
		{
			file->setStatus(status);
			return;
		}
	}
	
}

QStringList ParseXML::fileRecvList()
{
	QStringList strList;
	
	foreach(FileRecv * file, m_fileRecv)
	{
		strList << file->filePath();
	}
	
	return strList;
	
}

FileStatus ParseXML::getFileStatus(QString & filePath)
{
	FileStatus res = File_Received;
	foreach(FileRecv * file, m_fileRecv)
	{
		if(file->filePath() == filePath)
		{
			res = file->status();
		}
	}
	return res;
	
}

QString ParseXML::getFileDir(QString & filePath)
{
	foreach(FileRecv * file, m_fileRecv)
	{
		if(file->filePath() == filePath)
		{
			return file->fileDir();
		}
	}
	return QString();
	
}

void ParseXML::parseScene(const QDomElement & element)
{
	QDomNode child = element.firstChild();
	while(!child.isNull())
	{
		if(child.toElement().tagName() == "scene")
		{
			Scene * scene = new Scene(child.toElement().attribute("name"),
									  stringToTime(child.toElement().attribute("begin")),
									  stringToTime(child.toElement().attribute("end")),
									  child.toElement().attribute("interval").toInt());
			
			parseElement(scene, child.toElement());
			
			m_scene.append(scene);
		}
		child = child.nextSibling();
	}
	
}

void ParseXML::parseElement(Scene * scene, const QDomElement & element)
{	
	QDomNode child = element.firstChild();
	while(!child.isNull())
	{
		if(child.toElement().tagName() == "element")
		{
			QDomElement childNode = child.toElement();
			QString type = childNode.attribute("type");
			Node * node;
			if(type == "image")
			{
				node = new MediaNode(Type_image, childNode.attribute("file"), childNode.text());
			}
			else if(type == "video")
			{
				node = new MediaNode(Type_video, childNode.attribute("file"), childNode.text());
			}
			else if(type == "text")
			{
				node = new TextNode(childNode.text());
			}
			else
			{
				child.nextSibling();
				continue;
			}
			scene->addNode(node);
		}
		child = child.nextSibling();
	}
	
}
