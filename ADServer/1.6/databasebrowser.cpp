#include "databasebrowser.h"

#include <QtGui>
#include <QtSql>

#include "selectdatabaseinfodialog.h"
#include "node.h"
#include "tableDelegate.h"

// 定义静态成员变量
QSqlDatabase DatabaseBrowser::m_db;

/*
 *  函数名：DatabaseBrowser
 *  函数类型：构造函数
 *  时间：2016/9/27 21:38:08
 *  功能：初始化数据库部件。
 * 
 *  参数1：父窗口。
 *  
 */
DatabaseBrowser::DatabaseBrowser(QWidget * parent /* = 0 */) : QDialog(parent)
{
	m_appTranslatorCN.load(":/qt_zh_cn.qm");
	m_appTranslatorEN.load(":/ts_en_us.qm");
	
	createScenePanel();
	createNodePanel();
	
	m_addSceneButton = new QPushButton(tr("添加广告节目单"));
	m_deleteSceneButton = new QPushButton(tr("删除广告节目单"));
	m_addNodeButton = new QPushButton(tr("添加广告"));
	m_deleteNodeButton = new QPushButton(tr("删除广告"));
	
	QWidget * ADSceneWidget = new QWidget;
	
	QHBoxLayout * ADSceneButtonLayout = new QHBoxLayout;
	ADSceneButtonLayout->addStretch();
	ADSceneButtonLayout->addWidget(m_addSceneButton);
	ADSceneButtonLayout->addWidget(m_deleteSceneButton);
	
	QVBoxLayout * ADSceneLayout = new QVBoxLayout;
	ADSceneLayout->addWidget(m_scenePanel);
	ADSceneLayout->addLayout(ADSceneButtonLayout);
	
	ADSceneWidget->setLayout(ADSceneLayout);
	
	QWidget * ADListWidget = new QWidget;
	
	QHBoxLayout * ADListButtonLayout = new QHBoxLayout;
	ADListButtonLayout->addStretch();
	ADListButtonLayout->addWidget(m_addNodeButton);
	ADListButtonLayout->addWidget(m_deleteNodeButton);
	
	QVBoxLayout * ADListLayout = new QVBoxLayout;
	ADListLayout->addWidget(m_nodePanel);
	ADListLayout->addLayout(ADListButtonLayout);
	
	ADListWidget->setLayout(ADListLayout);
	
	m_splitter = new QSplitter(Qt::Vertical);
	m_splitter->setFrameStyle(QFrame::StyledPanel);
	m_splitter->addWidget(ADSceneWidget);
	m_splitter->addWidget(ADListWidget);
	
	connect(m_addSceneButton, SIGNAL(clicked(bool)),
			this, SLOT(addScene()));
	connect(m_deleteSceneButton, SIGNAL(clicked(bool)),
			this, SLOT(deleteScene()));
	connect(m_addNodeButton, SIGNAL(clicked(bool)),
			this, SLOT(addNode()));
	connect(m_deleteNodeButton, SIGNAL(clicked(bool)),
			this, SLOT(deleteNode()));
	
	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_splitter);
	setLayout(mainLayout);
	
	resize(600, 500);
	setWindowTitle(tr("编辑数据库"));
	
}

/*
 *  函数名：~DatabaseBrower
 *  函数类型：析构函数
 *  时间：2016/9/28 12:59:38
 *  功能：关闭数据库。
 *  
 */
DatabaseBrowser::~DatabaseBrowser()
{
	if(m_db.isOpen())
		m_db.close();
	
}

/*
 *  函数名：setLanguage
 *  函数类型：公有函数
 *  时间：2016/9/28 12:59:59
 *  功能：设备窗口语言。
 *  
 *  参数列表
 *	    参数1：true为中文，false为英文。
 * 
 */
void DatabaseBrowser::setLanguage(bool cn)
{
	if(cn)
	{
		qApp->installTranslator(&m_appTranslatorCN);
		qApp->removeTranslator(&m_appTranslatorEN);
	}
	else
	{
		qApp->installTranslator(&m_appTranslatorEN);
		qApp->removeTranslator(&m_appTranslatorCN);
	}
	
}

/*
 *  函数名：createConnection
 *	函数类型：公有函数，静态函数
 *  时间：2016/9/27 21:24:09
 *  功能：连接数据库，连接信息由程序所在目录下的DatabaseConfig文件提供。
 *  
 *  返回值：如果连接数据库失败，返回错误信息，否则返回空字符串。
 * 
 */
QString DatabaseBrowser::createConnection()
{
	if(m_db.isOpen())
		return QString();
	
	if(m_db.databaseName().isEmpty())
	{
		m_db = QSqlDatabase::addDatabase("QMYSQL");
		
		QFile file("DatabaseConfig");
		if(!file.open(QFile::ReadOnly | QFile::Text))
		{
			return QString(tr("无法打开数据库配置文件。"));
		}
		QTextStream in(&file);
		
		while(!in.atEnd())
		{
			QString text = in.readLine();
			if(text.startsWith('#'))
				continue;
			
			if(text.startsWith("HostName"))
				m_db.setHostName(text.mid(text.indexOf('=') + 2));
			if(text.startsWith("DatabaseName"))
				m_db.setDatabaseName(text.mid(text.indexOf('=') + 2));
			if(text.startsWith("UserName"))
				m_db.setUserName(text.mid(text.indexOf('=') + 2));
			if(text.startsWith("Password"))
				m_db.setPassword(text.mid(text.indexOf('=') + 2));
		}
		file.close();
	}
	
	if(!m_db.open())
	{
		m_db.removeDatabase("QMYSQL");
		return m_db.lastError().text();
	}
	
	return QString();
}

/*
 *  函数名：closeConnection
 *  函数类型：公有函数，静态函数
 *  时间：2016/9/27 21:34:10
 *  功能：关闭数据库。
 *  
 */
void DatabaseBrowser::closeConnection()
{
	if(m_db.isOpen())
		m_db.close();
	
}

/*
 *  函数名：selectADScene
 *  函数类型：公有函数，静态函数
 *  时间：2016/9/28 10:47:38
 *  功能：从数据库中选择广告节目单。
 *  
 *	参数1：标识程序显示语言，true为中文，false为英文。
 * 
 *  返回值：广告节目单信息。
 */
Scene * DatabaseBrowser::selectADScene(bool cn, QFont font)
{
	SelectDatabaseInfoDialog dialog(Table_adscene);
	
	dialog.setFont(font);
	dialog.setLanguage(cn);
	
	dialog.exec();
	
	return dialog.getScene();
	
}

/*
 *  函数名：selectADInfo
 *  函数类型：公有函数，静态函数
 *  时间：2016/9/28 10:48:36
 *  功能：从数据库中选择广告。
 * 
 *	参数1：标识程序显示语言，true为中文，false为英文。
 *  
 *  返回值：广告信息。
 */
Node * DatabaseBrowser::selectADInfo(bool cn, QFont font)
{
	SelectDatabaseInfoDialog dialog(Table_adlist);
	
	dialog.setFont(font);
	dialog.setLanguage(cn);
	
	dialog.exec();
	
	return dialog.getNode();
	
}

/*
 *  函数名：closeEvent
 *  函数类型：保护函数，重写函数
 *  时间：2016/9/28 12:47:39
 *  功能：设置窗口各部件的语言。
 *  
 *  参数列表
 *	    参数1：事件
 *  
 */
void DatabaseBrowser::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::LanguageChange)
	{
		setWindowTitle(tr("编辑数据库"));
		
		m_addSceneButton->setText(tr("添加广告节目单"));
		m_deleteSceneButton->setText(tr("删除广告节目单"));
		m_addNodeButton->setText(tr("添加广告"));
		m_deleteNodeButton->setText((tr("删除广告")));
		
		m_sceneModel->setHeaderData(adscene_name, Qt::Horizontal, tr("广告名称"));
		m_sceneModel->setHeaderData(adscene_begin, Qt::Horizontal, tr("开始时间"));
		m_sceneModel->setHeaderData(adscene_end, Qt::Horizontal, tr("结束时间"));
		m_sceneModel->setHeaderData(adscene_interval, Qt::Horizontal, tr("时间间隔"));
		
		m_sceneLabel->setText(tr("广告节目单列表"));
		
		m_nodeModel->setHeaderData(adlist_type, Qt::Horizontal, tr("广告类型"));
		m_nodeModel->setHeaderData(adlist_textOrFile, Qt::Horizontal, tr("广告文本/文件路径"));
		m_nodeModel->setHeaderData(adlist_adUrl, Qt::Horizontal, tr("广告链接"));
		m_nodeModel->setHeaderData(adlist_createTime, Qt::Horizontal, tr("创建时间"));
		
		m_nodeLabel->setText(tr("广告列表"));
		
	}
	else
	{
		QWidget::changeEvent(event);
	}
	
}

/*
 *  函数名：addScene
 *  函数类型：私有槽
 *  时间：2016/9/28 10:43:41
 *  功能：添加广告节目单。
 *  
 */
void DatabaseBrowser::addScene()
{
	int row = m_sceneModel->rowCount();
	m_sceneModel->insertRow(row);
	QModelIndex index = m_sceneModel->index(row, adscene_name);
	m_sceneView->setCurrentIndex(index);
	m_sceneView->edit(index);
	
}

/*
 *  函数名：deleteScene
 *  函数类型：私有槽
 *  时间：2016/9/28 10:43:21
 *  功能：删除当前选中的广告节目单。
 *  
 */
void DatabaseBrowser::deleteScene()
{
	QModelIndex index = m_sceneView->currentIndex();
	if(!index.isValid())
		return;
	
	QSqlDatabase::database().transaction();
	QSqlRecord record = m_sceneModel->record(index.row());
	int id = record.value(adscene_no).toInt();
	
	QSqlQuery query(QString("delete from adscene "
							"where no = %1").arg(id));
	query.exec();
	
	m_sceneModel->removeRow(index.row());
	m_sceneModel->submitAll();
	QSqlDatabase::database().commit();
	
}

/*
 *  函数名：addNode
 *  函数类型：私有槽
 *  时间：2016/9/28 10:43:00
 *  功能：添加新广告。
 *  
 */
void DatabaseBrowser::addNode()
{
	int row = m_nodeModel->rowCount();
	m_nodeModel->insertRow(row);
	QModelIndex index = m_nodeModel->index(row, adlist_type);
	m_nodeView->setCurrentIndex(index);
	m_nodeView->edit(index);
	
}

/*
 *  函数名：deleteNode
 *  函数类型：私有槽
 *  时间：2016/9/28 10:42:39
 *  功能：删除当前选中的广告。
 *  
 */
void DatabaseBrowser::deleteNode()
{
	QModelIndex index = m_nodeView->currentIndex();
	if(!index.isValid())
		return;
	
	QSqlDatabase::database().transaction();
	QSqlRecord record = m_nodeModel->record(index.row());
	int id = record.value(adscene_no).toInt();
	
	QSqlQuery query(QString("delete from adlist "
							"where no = %1").arg(id));
	query.exec();
	
	m_nodeModel->removeRow(index.row());
	m_nodeModel->submitAll();
	QSqlDatabase::database().commit();
	
}

/*
 *  函数名：createScenePanel
 *  函数类型：私有函数
 *  时间：2016/9/28 09:46:52
 *  功能：初始化广告节目单数据库列表视图。
 *  
 */
void DatabaseBrowser::createScenePanel()
{
	m_scenePanel = new QWidget;
	
	m_sceneModel = new QSqlRelationalTableModel(this);
	m_sceneModel->setTable("adscene");
	m_sceneModel->setSort(adscene_no, Qt::AscendingOrder);
	m_sceneModel->setHeaderData(adscene_name, Qt::Horizontal, tr("广告名称"));
	m_sceneModel->setHeaderData(adscene_begin, Qt::Horizontal, tr("开始时间"));
	m_sceneModel->setHeaderData(adscene_end, Qt::Horizontal, tr("结束时间"));
	m_sceneModel->setHeaderData(adscene_interval, Qt::Horizontal, tr("时间间隔"));
	m_sceneModel->select();
	
	m_sceneView = new QTableView;
	m_sceneView->setModel(m_sceneModel);
	m_sceneView->setItemDelegate(new QSqlRelationalDelegate(this));
	m_sceneView->setItemDelegateForColumn(adscene_begin, new TimeDelegate);
	m_sceneView->setItemDelegateForColumn(adscene_end, new TimeDelegate);
	m_sceneView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_sceneView->setColumnHidden(adscene_no, true);
	m_sceneView->horizontalHeader()->setStretchLastSection(true);
	m_sceneView->setAutoScroll(false);
	
	m_sceneView->setColumnWidth(1, 150);
	m_sceneView->setColumnWidth(2, 150);
	m_sceneView->setColumnWidth(3, 150);
	m_sceneView->setColumnWidth(4, 50);
	
	m_sceneLabel = new QLabel(tr("广告节目单列表"));
	
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(m_sceneLabel);
	layout->addWidget(m_sceneView);
	m_scenePanel->setLayout(layout);
	
}

/*
 *  函数名：createNodePanel
 *  函数类型：私有函数
 *  时间：2016/9/28 09:47:26
 *  功能：初始化广告列表视图。
 *  
 */
void DatabaseBrowser::createNodePanel()
{
	m_nodePanel = new QWidget;
	
	m_nodeModel = new QSqlRelationalTableModel(this);
	m_nodeModel->setTable("adlist");
	m_nodeModel->setSort(adlist_no, Qt::AscendingOrder);
	m_nodeModel->setHeaderData(adlist_type, Qt::Horizontal, tr("广告类型"));
	m_nodeModel->setHeaderData(adlist_textOrFile, Qt::Horizontal, tr("广告文本/文件路径"));
	m_nodeModel->setHeaderData(adlist_adUrl, Qt::Horizontal, tr("广告链接"));
	m_nodeModel->setHeaderData(adlist_createTime, Qt::Horizontal, tr("创建时间"));
	m_nodeModel->select();
	
	m_nodeView = new QTableView;
	m_nodeView->setModel(m_nodeModel);
	m_nodeView->setItemDelegate(new QSqlRelationalDelegate(this));
	m_nodeView->setItemDelegateForColumn(adlist_type, new ADTypeDelegate);
	m_nodeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_nodeView->setColumnHidden(adlist_no, true);
	m_nodeView->horizontalHeader()->setStretchLastSection(true);
	m_nodeView->setAutoScroll(false);
	
	m_nodeView->setColumnWidth(2, 500);
	m_nodeView->setColumnWidth(3, 250);
	m_nodeView->setColumnWidth(4, 150);
	
	m_nodeLabel = new QLabel(tr("广告列表"));
	
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(m_nodeLabel);
	layout->addWidget(m_nodeView);
	m_nodePanel->setLayout(layout);
	
}
