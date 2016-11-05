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

void ParseXML::addRecvFile(QString filePath)
{
	FileRecv * fileRecv = new FileRecv(filePath);
	m_fileRecv.append(fileRecv);
	
}

void ParseXML::setRecvFileStatus(QString filePath, FileStatus status)
{
	foreach(FileRecv * file, m_fileRecv)
	{
		if(file->filePath() == filePath)
		{
			file->setStatus(status);
			break;
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

/*
void ParseXML::addScene(Scene * sc)
{
	m_scene.append(sc);
	
}

void ParseXML::removeScene(int index)
{
	m_scene.removeAt(index);
	
}

bool ParseXML::writeXML(const QString & fileName)
{
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		cout << qPrintable(QString("错误：无法写入文件[%1]\n原因：%2")
						   .arg(QFileInfo(fileName).fileName())
						   .arg(file.errorString())) << endl;
		return false;
	}
	
	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setCodec(QTextCodec::codecForName("GB2312"));
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("menu");
	foreach (Scene * s, m_scene)
	{
		writeScene(&xmlWriter, s);
	}
	xmlWriter.writeEndDocument();
	file.close();
	if(file.error())
	{
		cout << qPrintable(QString("错误：无法写入文件[%1]\n原因：%2")
						   .arg(QFileInfo(fileName).fileName())
						   .arg(file.errorString())) << endl;
		return false;
	}
	return true;
	
}
*/

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
				node = new MediaNode(Type_image, childNode.attribute("file"));
			}
			else if(type == "video")
			{
				node = new MediaNode(Type_video, childNode.attribute("file"));
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

/*
void ParseXML::writeScene(QXmlStreamWriter * xmlWriter, Scene * scene)
{
	xmlWriter->writeStartElement("scene");
	xmlWriter->writeAttribute("name", scene->name());
	xmlWriter->writeAttribute("begin", timeToString(scene->begin()));
	xmlWriter->writeAttribute("end", timeToString(scene->end()));
	xmlWriter->writeAttribute("interval", QString::number(scene->interval()));
	
	foreach(Node * n, scene->nodeList())
	{
		xmlWriter->writeStartElement("element");
		
		switch (n->type())
		{
		case Type_image:
		case Type_video:
			xmlWriter->writeAttribute("type", (n->type() == Type_image ? "image" : "video"));
			xmlWriter->writeAttribute("file", static_cast<MediaNode *>(n)->file());
			
			break;
			
		case Type_text:
			xmlWriter->writeAttribute("type", "text");
			xmlWriter->writeCharacters(static_cast<TextNode *>(n)->text());
			
			break;
		}
		xmlWriter->writeEndElement();
		
	}
	
	xmlWriter->writeEndElement();
	
}
*/
