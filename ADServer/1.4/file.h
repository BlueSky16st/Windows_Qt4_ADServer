#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QSet>
#include "parsexml.h"

class File
{
public:
	ParseXML m_fileParse;
	
	File(const QString & filePath = "");
	
	void setFilePath(const QString & filePath);
	QString filePath() const;
	QString fileName() const;
	
	bool setSendID(QString strID);
	void removeSendID(int id);
	void removeAllID();
	QString sendID() const;
	
private:
	QString m_filePath;
	QSet<int> m_sendID;
	
};

#endif // FILE_H
