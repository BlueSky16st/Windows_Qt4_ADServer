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
			 const FileStatus & status = FILE_Not_Receive);
	
	void setStatus(FileStatus status);
	FileStatus status() const;
	
	QString filePath() const;
	void setfilePath(const QString & filePath);
	
private:
	QString m_filePath;
	FileStatus m_status;
	
};

#endif // FILERECV_H
