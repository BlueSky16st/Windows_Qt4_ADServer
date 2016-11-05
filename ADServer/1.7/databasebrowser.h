#ifndef DATABASEBROWSER_H
#define DATABASEBROWSER_H

#include <QDialog>
#include <QSqlDatabase>
#include <QTranslator>

class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QLabel;
class QTableView;
class QSplitter;
class QPushButton;
class QDialogButtonBox;
class Scene;
class Node;

/*
 *  类名：DatabaseBrowser
 *  时间：2016/9/27 21:06:56
 *  功能：显示并可以对数据库的广告或广告节目单信息进行编辑，
 *			同时可以单独获得广告或广告节目单的信息。
 */
class DatabaseBrowser : public QDialog
{
	Q_OBJECT
	
public:
	DatabaseBrowser(QWidget * parent = 0);		// 构造函数
	~DatabaseBrowser();							// 析构函数
	
	void setLanguage(bool cn);					// 设置数据库编辑窗口语言
	
	static QString createConnection();			// 创建数据库连接
	static void closeConnection();				// 关闭数据库连接
	static bool isConnected();					// 检查数据库是否已连接
	static Scene * selectADScene(bool cn, QFont font);		// 从数据库中获取一个广告节目单
	static Node * selectADInfo(bool cn, QFont font);		// 从数据库中获取一条广告
	
protected:
	void changeEvent(QEvent * event);			// 语言改变事件
	
public slots:
	void addScene();							// 添加广告节目单
	void deleteScene();							// 删除广告节目单
	void addNode();								// 添加广告
	void deleteNode();							// 删除广告
	
private:
	void createScenePanel();					// 创建广告节目单列表视图   
	void createNodePanel();						// 创建广告列表视图       
	
	QTranslator m_appTranslatorCN;				// 中文翻译文件
	QTranslator m_appTranslatorEN;				// 英文翻译文件
	
	QSqlRelationalTableModel * m_sceneModel;	// 广告节目单数据库模型
	QSqlRelationalTableModel * m_nodeModel;		// 广告数据库模型
	
	QWidget * m_scenePanel;						// 广告节目单窗口部件
	QWidget * m_nodePanel;						// 广告窗口部件
	
	QLabel * m_sceneLabel;						// 广告节目单标签
	QLabel * m_nodeLabel;						// 广告标签
	
	QTableView * m_sceneView;					// 广告节目单数据库视图
	QTableView * m_nodeView;					// 广告数据库视图
	
	QSplitter * m_splitter;						// 分隔器
	
	QPushButton * m_addSceneButton;				// 添加广告节目单按钮
	QPushButton * m_deleteSceneButton;			// 删除广告节目单按钮
	QPushButton * m_addNodeButton;				// 添加广告按钮
	QPushButton * m_deleteNodeButton;			// 删除广告按钮
	
	QDialogButtonBox * m_buttonBox;				// 按钮组
	
	static bool m_isConnected;					// 标识已连接上数据库
	static QSqlDatabase m_db;					// 数据库
	
};

#endif // DATABASEBROWSER_H
