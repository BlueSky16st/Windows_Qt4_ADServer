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

/*
 * 析构函数
 * 清空广告节目单列表。
 * 
*/
ParseXML::~ParseXML()
{
	qDeleteAll(m_scene);
	m_scene.clear();
	
}

/*
 * 公有函数
 * 参数1：文件路径。
 * 返回值：如果解析成功，返回true，否则返回false。
 * 开始解析XML文件。
 * 
*/
bool ParseXML::parse(const QString & fileName)
{
	qDeleteAll(m_scene);
	m_scene.clear();
	m_doc.clear();
	
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		cout << qPrintable(QString("错误：读取文件[%1]失败\n原因：%2")
						   .arg(QFileInfo(fileName).fileName())
						   .arg(file.errorString()));
		return false;
	}
	
	if(QFileInfo(fileName).size() == 0)
	{
		writeXML(fileName);
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

/*
 * 公有函数
 * 返回值：返回广告节目单列表。
 * 
*/
QList<Scene *> ParseXML::scene()
{
	return m_scene;
	
}

/*
 * 公有函数
 * 参数1：广告节目单类。
 * 添加新的广告节目单到广告节目单列表中。
 * 
*/
void ParseXML::addScene(Scene * sc)
{
	m_scene.append(sc);
	
}

/*
 * 公有函数
 * 参数1：广告节目单索引。
 * 删除指定索引值的广告节目单。
 * 
*/
void ParseXML::removeScene(int index)
{
	m_scene.removeAt(index);
	
}

/*
 * 公有函数
 * 参数1：文件路径。
 * 返回值：如果写入成功，返回true，否则返回false。
 * 保存当前XML文件到指定文件路径。
 * 
*/
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

/*
 * 私有函数
 * 参数1：根节点
 * 解析所有的广告节目单节点，并添加到广告节目单列表。
 * 
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

/*
 * 私有函数
 * 参数1：广告节目单
 * 参数2：该广告节目单所在节点
 * 解析指定广告节目单节点中所有的广告节点，并添加到相应广告节目单中。
 * 
*/
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

/*
 * 私有函数
 * 参数1：XML写入流。
 * 参数2：广告节目单类。
 * 将广告节目单的属性和其所属的所有广告写入到XML写入流中。
 * 
*/
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
			xmlWriter->writeCharacters(static_cast<MediaNode *>(n)->url());
			
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
