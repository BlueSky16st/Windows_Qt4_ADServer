#ifndef PARSEXML_H
#define PARSEXML_H

#include <QList>
#include <QDomDocument>

class QString;
class QDomElement;
class QXmlStreamWriter;
class Scene;

/*
 * 解析XML类
 * 
*/
class ParseXML
{
public:
	~ParseXML();								// 析构函数
	
	bool parse(const QString & fileName);		// 解析文件
	QList<Scene *> scene();						// 返回广告节目单列表
	void addScene(Scene * sc);					// 添加广告节目单
	void removeScene(int index);				// 删除指定广告节目单
	
	bool writeXML(const QString & fileName);	// 将广告节目单写入指定文件
	
private:
	void parseScene(const QDomElement & element);	// 解析广告节目单
	void parseElement(Scene * scene, 
					  const QDomElement & element);	// 解析广告
	
	void writeScene(QXmlStreamWriter * xmlWriter, 
					Scene * scene);					// 写入广告节目单
	
	QList<Scene *> m_scene;						// 广告节目单列表
	QDomDocument m_doc;							// XML主节点
};

#endif // PARSEXML_H
