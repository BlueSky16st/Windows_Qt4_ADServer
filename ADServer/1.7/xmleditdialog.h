#ifndef XMLEDITDIALOG_H
#define XMLEDITDIALOG_H

#include <QDialog>
#include <QTranslator>
#include "ui_xmleditdialog.h"

class File;

/*
 * 编辑窗口类
 * 
*/
class XMLEditDialog : public QDialog, private Ui::XMLEditDialog
{
	Q_OBJECT
	
public:
	XMLEditDialog(File * file, QFont font, bool isCN = true, QWidget * parent = 0);		// 构造函数
	
	void setLanguage(bool cn);								// 设置编辑窗口语言
	
protected:
	void closeEvent(QCloseEvent * event);					// 编辑窗口关闭事件
	void changeEvent(QEvent * event);						// 语言改变事件
	
signals:
	void fileChange();										// 文件已改变信号
	
private slots:
	void showInfo(QTreeWidgetItem * item);					// 显示广告节目内容
	void showContextMenu(QPoint pos);						// 在[广告列表]中显示上下文菜单
	void addNodeOrSceneContextMenu(QTreeWidgetItem * item);	// 添加广告或广告节目单的上下文菜单
	void changeType(int index);								// 改变广告类型
	void saveFile();										// 保存文件
	void setData();											// 更新广告数据
	void browseFile();										// 浏览图片/视频文件
	void newScene();										// 添加新的广告节目单
	void deleteScene();										// 删除选中的广告节目单
	void deleteNode();										// 删除选中的广告
	
private:
	void refreshList();										// 刷新[广告列表]
	void createNewSceneOrNode(bool fromDatabase = false);	// 创建一个新的广告或广告节目单
	
	void createAction();									// 初始化动作
	
	File * m_file;											// 保存当前正在操作的文件
	
	QMenu * m_sceneMenu;									// [广告列表]上下文菜单
	
	QAction * m_newSceneAction;								// 创建新的广告节目单动作
	QAction * m_deleteSceneAction;							// 删除广告节目单动作
	QAction * m_deleteNodeAction;							// 删除广告动作
	QAction * m_addNodeDirectlyAction;						// 直接添加广告节目动作
	QAction * m_addSceneDirectlyAction;						// 直接添加广告节目单动作
	QAction * m_addFromDatabaseAction;						// 从数据库中添加动作	
	
	QTranslator m_appTranslatorCN;							// 中文翻译文件
	QTranslator m_appTranslatorEN;							// 英文翻译文件
	
	bool m_somethingChanged;								// 标识当前文件是否已被修改
	
};

#endif // XMLEDITDIALOG_H
