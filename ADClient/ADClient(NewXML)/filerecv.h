#ifndef FILERECV_H
#define FILERECV_H

#include <QString>

enum FileStatus
{
	File_Received,
	FILE_Receive_FAIL,
	FILE_Not_Receive
};

class FileRecv
{
public:
	FileRecv(const QString & filePath,
			 const QString & fileDir,
			 const FileStatus & status = FILE_Not_Receive);
	
	void setStatus(FileStatus status);
	FileStatus status() const;
	
	QString filePath() const;
	void setfilePath(const QString & filePath);
	
	QString fileDir() const;
	void setfileDir(const QString & fileDir);
	
private:
	QString m_filePath;
	QString m_fileDir;		// 文件所在目录
	FileStatus m_status;
	
};

#endif // FILERECV_H
