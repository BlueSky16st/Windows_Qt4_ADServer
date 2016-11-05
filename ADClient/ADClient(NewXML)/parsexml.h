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
	
	void addRecvFile(QString filePath, QString fileDir);
	void setRecvFileStatus(QString filePath, FileStatus status);
	QStringList fileRecvList();
	FileStatus getFileStatus(QString & filePath);
	QString getFileDir(QString & filePath);
	
private:
	void parseScene(const QDomElement & element);
	void parseElement(Scene * scene, const QDomElement & element);
	
	QString m_filePath;
	
	QList<FileRecv *> m_fileRecv;
	QList<Scene *> m_scene;
	QDomDocument m_doc;
};

#endif // PARSEXML_H
