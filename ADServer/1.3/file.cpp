#include "file.h"
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

File::File(const QString & filePath) 
{
	m_filePath = filePath;
	m_fileParse.parse(m_filePath);
	
}

void File::setFilePath(const QString & filePath)
{
	m_filePath = filePath;
	m_fileParse.parse(m_filePath);
	
}

QString File::filePath() const
{
	return m_filePath;
	
}

QString File::fileName() const
{
	return QFileInfo(m_filePath).fileName();
	
}

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

void File::removeSendID(int id)
{
	m_sendID.remove(id);
	
}

void File::removeAllID()
{
	m_sendID.clear();
	
}

QString File::sendID() const
{
	QString str;
	foreach (const int i, m_sendID)
	{
		str += QString::number(i) + " ";
	}
	return str;
	
}
