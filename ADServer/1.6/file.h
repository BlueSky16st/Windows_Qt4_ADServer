#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QSet>
#include "parsexml.h"

/*
 * 文件类
 * 记录adm文件的内容、路径、发送ID号。
 * 
*/
class File
{
public:
	ParseXML m_fileParse;							// 解析adm文件
	
	File(const QString & filePath = "");			// 构造函数
	
	void setFilePath(const QString & filePath);		// 设置新的文件路径
	QString filePath() const;						// 返回文件路径
	QString fileName() const;						// 返回文件名
	
	bool setSendID(QString strID);					// 设置发送ID
	void removeSendID(int id);						// 移除指定发送ID
	void removeAllID();								// 清空发送ID
	QString sendID() const;							// 返回发送ID字符串
	
private:
	QString m_filePath;								// 文件路径
	QSet<int> m_sendID;								// 发送ID
	
};

#endif // FILE_H
