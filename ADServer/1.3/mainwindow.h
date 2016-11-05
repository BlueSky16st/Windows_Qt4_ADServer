#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include "ui_mainwindow.h"
#include "tcpserver.h"

class File;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
	
public:
	MainWindow(QWidget * parent = 0);
	~MainWindow();
	
protected:
	void closeEvent(QCloseEvent * event);
	void changeEvent(QEvent * event);
	
private slots:
	
	// 编辑器栏
	void newFile();
	void addFile();
	void removeFile();
	void clearList();
	void setFileSendID();
	void editFile();
	void showFileListContextMenu(QPoint pos);
	void fileBrowse();
	void conBrowseSignal(int index);
	
	// 服务器栏
	void addMessage(QString msg);
	void saveLog();
	void startOrStopServer(bool bo);
	void repairServerButton(int index);
	
	// 程序设置栏
	void setLanguage();
	void setProgramFont();
	void resetSetting();
	
	
private:
	void createAction();
	void initConnect();
	
	void refreshList();
	
	void readSettings();
	void saveSettings();
	
	QAction * m_newFileAction;
	QAction * m_addFileAction;
	QAction * m_removeFileAction;
	QAction * m_clearListAction;
	QAction * m_setFileSendIDAction;
	
	QMenu * m_fileListMenu;
	
	QList<File *> m_fileList;
	
	QTranslator m_appTranslatorCN;
	QTranslator m_appTranslatorEN;
	
	TCPServer * server;
	
};

#endif // MAINWINDOW_H
