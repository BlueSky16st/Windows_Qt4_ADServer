#ifndef PARSEXML_H
#define PARSEXML_H

#include <QList>
#include <QDomDocument>

class QString;
class QDomElement;
class QXmlStreamWriter;
class Scene;

class ParseXML
{
public:
	~ParseXML();
	
	bool parse(const QString & fileName);
	QList<Scene *> scene();
	void addScene(Scene * sc);
	void removeScene(int index);
	
	bool writeXML(const QString & fileName);
	
private:
	void parseScene(const QDomElement & element);
	void parseElement(Scene * scene, const QDomElement & element);
	
	void writeScene(QXmlStreamWriter * xmlWriter, Scene * scene);
	
	QList<Scene *> m_scene;
	QDomDocument m_doc;
};

#endif // PARSEXML_H
