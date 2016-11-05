#ifndef SELECTDATABASEINFODIALOG_H
#define SELECTDATABASEINFODIALOG_H

#include <QDialog>
#include <QTranslator>

class QSqlRelationalTableModel;
class QTableView;
class QPushButton;
class Node;
class Scene;

// 表
enum Table
{
	Table_adscene,
	Table_adlist
};

// 广告节目单表头
enum adscene
{
	adscene_no,
	adscene_name,
	adscene_begin,
	adscene_end,
	adscene_interval
};

// 广告表头
enum adlist
{
	adlist_no,
	adlist_type,
	adlist_textOrFile,
	adlist_adUrl,
	adlist_createTime
};

/*
 *  类名：SelectDatabaseInfoDialog
 *  时间：2016/9/28 10:50:45
 *  功能：显示临时数据库信息列表，在选择某一项并确定之后返回相应数据信息。
 */
class SelectDatabaseInfoDialog : public QDialog
{
	Q_OBJECT
	
public:
	SelectDatabaseInfoDialog(Table table, 
							 QWidget * parent = 0);		// 构造函数
	
	void setLanguage(bool cn);							// 设置数据库选择窗口语言
	
	Scene * getScene();									// 返回广告节目单信息
	Node * getNode();									// 返回广告信息
	
protected:
	void changeEvent(QEvent * event);					// 语言改变事件
	
private slots:
	void select();										// 确定时，设置广告节目单或广告信息
	
private:
	void createPanel();									// 创建数据库列表视图
	
	QTranslator m_appTranslatorCN;						// 中文翻译文件
	QTranslator m_appTranslatorEN;						// 英文翻译文件
	
	Node * m_node;										// 广告
	Scene * m_scene;									// 广告节目单
	
	Table m_table;										// 保存要进行操作的数据库表
	
	QSqlRelationalTableModel * m_model;					// 数据库模型
	QWidget * m_panel;									// 主窗口部件
	QTableView * m_view;								// 数据库视图
	QPushButton * m_okButton;							// 确定按钮
	QPushButton * m_cancelButton;						// 取消按钮
	
};

#endif // SELECTDATABASEINFODIALOG_H
