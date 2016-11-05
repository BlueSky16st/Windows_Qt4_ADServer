#include "file.h"
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>
#include <QDir>

/*
 * 构造函数
 * 设置文件路径，解析文件。
 * 
*/
File::File(const QString & filePath) 
{
	m_filePath = QDir::toNativeSeparators(filePath);
	m_fileParse.parse(m_filePath);
	
}

/*
 * 公有函数
 * 设置新的文件路径，解析文件。
 * 
*/
void File::setFilePath(const QString & filePath)
{
	m_filePath = QDir::toNativeSeparators(filePath);
	m_fileParse.parse(m_filePath);
	
}

/*
 * 公有函数
 * 返回值：返回文件路径。
 * 
*/
QString File::filePath() const
{
	return m_filePath;
	
}

/*
 * 公有函数
 * 返回值：返回文件名。
 * 
*/
QString File::fileName() const
{
	return QFileInfo(m_filePath).fileName();
	
}

/*
 * 公有函数
 * 参数1：发送ID字符串
 * 返回值：如果符合规则并设置成功，返回true，否则返回false。
 * 如果符合正则表达式或可以转换成数字，并且全部成功，则设置发送ID，否则视为不符合规则。
 * 
*/
bool File::setSendID(QString strID)
{
	QSet<int> setID;
	bool ok;
	
	QStringList idList = strID.split(' ', QString::SkipEmptyParts);
	
	if(idList.isEmpty())
	{
		m_sendID.clear();
		return true;
	}
	
	foreach(QString i, idList)
	{
		int iID = i.toInt(&ok);
		if(ok)
		{
			setID.insert(iID);
			continue;
		}
		else
		{
			QRegExp regExp("[0-9]+-[0-9]+");
			if(regExp.exactMatch(i))
			{
				QStringList rangeID = i.split('-');
				int rangeLeft = rangeID[0].toInt();
				int rangeRight = rangeID[1].toInt();
				if(rangeLeft < rangeRight)
				{
					for(int i = rangeLeft; i <= rangeRight; i++)
					{
						setID.insert(i);
					}
					ok = true;
				}
				if(ok)
					continue;
			}
		}
		
		return false;
	}
	m_sendID = setID;
	
	return true;
	
}

/*
 * 公有函数
 * 参数1：要删除的发送ID号
 * 删除发送ID。
 * 
*/
void File::removeSendID(int id)
{
	m_sendID.remove(id);
	
}

/*
 * 公有函数
 * 清空所有发送ID。
 * 
*/
void File::removeAllID()
{
	m_sendID.clear();
	
}

/*
 * 公有函数
 * 返回值：返回发送ID字符串，每个ID号用空格分隔。
 * 
*/
QString File::sendID() const
{
	QString str;
	foreach (const int i, m_sendID)
	{
		str += QString::number(i) + " ";
	}
	return str;
	
}
