#ifndef PARSEXML_H
#define PARSEXML_H

#include <QList>
#include <QDomDocument>
#include "filerecv.h"

class QString;
class QDomElement;
class QXmlStreamWriter;
class Scene;

class ParseXML
{
public:
	ParseXML(const QString & filePath);
	~ParseXML();
	
	void setFilePath(const QString & filePath);
	QString filePath() const;
	
	bool parse();
	QList<Scene *> scene();
	// void addScene(Scene * sc);
	// void removeScene(int index);
	
	// bool writeXML(const QString & fileName);
	
	void addRecvFile(QString filePath);
	void setRecvFileStatus(QString filePath, FileStatus status);
	QStringList fileRecvList();
	FileStatus getFileStatus(QString & filePath);
	
private:
	void parseScene(const QDomElement & element);
	void parseElement(Scene * scene, const QDomElement & element);
	
	// void writeScene(QXmlStreamWriter * xmlWriter, Scene * scene);
	
	QString m_filePath;
	
	QList<FileRecv *> m_fileRecv;
	QList<Scene *> m_scene;
	QDomDocument m_doc;
};

#endif // PARSEXML_H
