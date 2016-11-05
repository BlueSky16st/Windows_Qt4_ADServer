#include "filerecv.h"
#include <QString>

FileRecv::FileRecv(const QString & filePath, 
				   const FileStatus & status) : m_filePath(filePath), m_status(status)
{
}

void FileRecv::setStatus(FileStatus status)
{
	m_status = status;
	
}

FileStatus FileRecv::status() const
{
	return m_status;
	
}

QString FileRecv::filePath() const
{
	return m_filePath;
	
}

void FileRecv::setfilePath(const QString & filePath)
{
	m_filePath = filePath;
	
}
