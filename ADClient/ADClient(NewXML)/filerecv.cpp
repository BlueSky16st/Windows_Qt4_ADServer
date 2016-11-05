#include "filerecv.h"
#include <QString>

FileRecv::FileRecv(const QString & filePath,
				   const QString & fileDir,
				   const FileStatus & status) 
	: m_filePath(fileDir + "\\" + filePath), m_fileDir(fileDir), m_status(status)
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

QString FileRecv::fileDir() const
{
	return m_fileDir;
	
}

void FileRecv::setfileDir(const QString & fileDir)
{
	m_fileDir = fileDir;
	
}
