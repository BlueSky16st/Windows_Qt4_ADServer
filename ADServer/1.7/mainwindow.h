#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include "ui_mainwindow.h"
#include "tcpserver.h"

class File;
class DatabaseBrowser;

/*
 * 主窗口类
 * 
*/
class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
	
public:
	MainWindow(QWidget * parent = 0);				// 构造函数
	~MainWindow();									// 析构函数
	
protected:
	void closeEvent(QCloseEvent * event);			// 程序关闭事件
	void changeEvent(QEvent * event);				// 语言改变事件
	
private slots:
	// 编辑器栏
	void newFile();									// 创建新文件
	void addFile();									// 添加文件
	void removeFile();								// 移除选中的文件
	void clearList();								// 清空文件列表
	void setFileSendID();							// 设置文件发送ID
	void editFile();								// 编辑选中的文件
	void showFileListContextMenu(QPoint pos);		// 在[文件列表]中显示上下文菜单
	void fileBrowse();								// 对[文件列表]中选中的文件进行浏览
	void conBrowseSignal(int index);				// 连接或断开[文件列表]所产生的文件浏览信号
	void connectOrDisconnectDatabase(bool bo);		// 连接或断开数据库
	void editDatabase();							// 编辑广告数据库
	
	// 服务器栏
	void addMessage(QString msg);					// 添加消息到日志中
	void saveLog();									// 保存日志
	void startOrStopServer(bool bo);				// 启动或关闭服务器
	void repairServerButton(int index);				// 修正[启动或关闭服务器按钮]文字说明
	void setClientCount(int count);					// 更新[客户端连接数]标签
	
	// 程序设置栏
	void setLanguage();								// 切换程序语言
	void setProgramFont();							// 设置程序字体
	void resetSetting();							// 重置程序设置
	
private:
	void createAction();							// 初始化动作
	void initConnect();								// 初始化信号与槽的连接
	
	void refreshList();								// 刷新[文件列表]
	
	void readSettings();							// 读取程序设置。
	void saveSettings();							// 保存程序设置。
	
	QAction * m_newFileAction;						// 创建新文件动作
	QAction * m_addFileAction;						// 添加文件动作
	QAction * m_removeFileAction;					// 移除选中的文件动作
	QAction * m_clearListAction;					// 清空文件列表动作
	QAction * m_setFileSendIDAction;				// 设置文件发送ID动作
	
	QMenu * m_fileListMenu;							// [文件列表]上下文菜单
	
	QList<File *> m_fileList;						// 记录已打开的文件
	
	QTranslator m_appTranslatorCN;					// 中文翻译文件
	QTranslator m_appTranslatorEN;					// 英文翻译文件
	
	TCPServer * m_server;							// 服务器
	
};

#endif // MAINWINDOW_H
