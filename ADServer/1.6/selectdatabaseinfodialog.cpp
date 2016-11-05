#include "selectdatabaseinfodialog.h"

#include <QtGui>
#include <QtSql>

#include "node.h"

/*
 *  函数名：SelectDatabaseInfoDialog
 *  函数类型：构造函数
 *  时间：2016/9/28 11:11:51
 *  功能：初始化部件。
 *  
 */
SelectDatabaseInfoDialog::SelectDatabaseInfoDialog(Table table, QWidget * parent)
	: QDialog(parent)
{
	m_appTranslatorCN.load(":/qt_zh_cn.qm");
	m_appTranslatorEN.load(":/ts_en_us.qm");
	m_table = table;
	
	createPanel();
	
	m_okButton = new QPushButton(tr("确定(&O)"));
	m_cancelButton = new QPushButton(tr("取消(&C)"));
	
	QHBoxLayout * buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(m_okButton);
	buttonLayout->addWidget(m_cancelButton);
	
	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_panel);
	mainLayout->addLayout(buttonLayout);
	
	connect(m_okButton, SIGNAL(clicked(bool)),
			this, SLOT(select()));
	connect(m_cancelButton, SIGNAL(clicked(bool)),
			this, SLOT(close()));
	
	m_node = NULL;
	m_scene = NULL;
	
	resize(600, 300);
	setWindowTitle(tr("选择"));
	setLayout(mainLayout);
	
}

/*
 *  函数名：setLanguage
 *  函数类型：公有函数
 *  时间：2016/9/29 16:31:49
 *  功能：设备窗口语言。
 *  
 *  参数列表
 *	    参数1：true为中文，false为英文。
 */
void SelectDatabaseInfoDialog::setLanguage(bool cn)
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
 *  函数名：getScene
 *  函数类型：公有函数
 *  时间：2016/9/28 11:13:00
 *  功能：返回广告节目单。
 *  
 *  返回值：return
 */
Scene * SelectDatabaseInfoDialog::getScene()
{
	return m_scene;
	
}

/*
 *  函数名：getNode
 *  函数类型：公有函数
 *  时间：2016/9/28 11:12:11
 *  功能：返回广告信息。
 *  
 *  返回值：广告信息。
 */
Node * SelectDatabaseInfoDialog::getNode()
{
	return m_node;
	
}

/*
 *  函数名：changeEvent
 *  函数类型：保护函数，重写函数
 *  时间：2016/9/29 16:32:42
 *  功能：设置窗口各部件的语言。
 *  
 *  参数列表
 *	    参数1：事件
 * 
 */
void SelectDatabaseInfoDialog::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::LanguageChange)
	{
		setWindowTitle(tr("选择"));
		
		m_okButton->setText(tr("确定(&O)"));
		m_cancelButton->setText(tr("取消(&C)"));
		
		if(m_table == Table_adscene)
		{
			m_model->setHeaderData(adscene_name, Qt::Horizontal, tr("广告名称"));
			m_model->setHeaderData(adscene_begin, Qt::Horizontal, tr("开始时间"));
			m_model->setHeaderData(adscene_end, Qt::Horizontal, tr("结束时间"));
			m_model->setHeaderData(adscene_interval, Qt::Horizontal, tr("时间间隔"));
			
		}
		else
		{
			m_model->setHeaderData(adlist_type, Qt::Horizontal, tr("广告类型"));
			m_model->setHeaderData(adlist_textOrFile, Qt::Horizontal, tr("广告文本/文件路径"));
			m_model->setHeaderData(adlist_adUrl, Qt::Horizontal, tr("广告链接"));
			m_model->setHeaderData(adlist_createTime, Qt::Horizontal, tr("创建时间"));
			
		}
		
	}
	else
	{
		QWidget::changeEvent(event);
	}
	
}

/*
 *  函数名：select
 *  函数类型：私有槽
 *  时间：2016/9/28 11:16:51
 *  功能：按确定按钮之后将信息添加到相应的广告类中。
 *  
 */
void SelectDatabaseInfoDialog::select()
{
	QModelIndex index = m_view->currentIndex();
	if(!index.isValid())
		close();
	
	QSqlRecord record = m_model->record(index.row());
	
	if(m_table == Table_adscene)
	{
		QString name = record.value(adscene_name).toString();
		QString begin = record.value(adscene_begin).toString();
		QString end = record.value(adscene_end).toString();
		QString interval = record.value(adscene_interval).toString();
		m_scene = new Scene(name,
							stringToTime(begin),
							stringToTime(end),
							interval.toInt());
		
	}
	else
	{
		QString type = record.value(adlist_type).toString();
		QString textOrFile = record.value(adlist_textOrFile).toString();
		QString adUrl = record.value(adlist_adUrl).toString();
		
		if(type == "text")
			m_node = new TextNode(textOrFile);
		else if(type == "video")
			m_node = new MediaNode(Type_video, textOrFile, adUrl);
		else if(type == "image")
			m_node = new MediaNode(Type_image, textOrFile, adUrl);
	}
	
	close();
	
}

/*
 *  函数名：createPanel
 *  函数类型：私有函数
 *  时间：2016/9/28 11:17:58
 *  功能：创建数据库列表视图。
 *  
 */
void SelectDatabaseInfoDialog::createPanel()
{
	m_panel = new QWidget;
	m_view = new QTableView;
	m_model = new QSqlRelationalTableModel(this);
	
	if(m_table == Table_adscene)
	{
		m_model->setTable("adscene");
		m_model->setSort(adscene_no, Qt::AscendingOrder);
		m_model->setHeaderData(adscene_name, Qt::Horizontal, tr("广告名称"));
		m_model->setHeaderData(adscene_begin, Qt::Horizontal, tr("开始时间"));
		m_model->setHeaderData(adscene_end, Qt::Horizontal, tr("结束时间"));
		m_model->setHeaderData(adscene_interval, Qt::Horizontal, tr("时间间隔"));
		
	}
	else
	{
		m_model->setTable("adlist");
		m_model->setSort(adlist_no, Qt::AscendingOrder);
		m_model->setHeaderData(adlist_type, Qt::Horizontal, tr("广告类型"));
		m_model->setHeaderData(adlist_textOrFile, Qt::Horizontal, tr("广告文本/文件路径"));
		m_model->setHeaderData(adlist_adUrl, Qt::Horizontal, tr("广告链接"));
		m_model->setHeaderData(adlist_createTime, Qt::Horizontal, tr("创建时间"));
		
	}
	
	m_model->select();
	
	m_view->setModel(m_model);
	m_view->setColumnHidden(0, true);
	m_view->setItemDelegate(new QSqlRelationalDelegate(this));
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
	m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_view->horizontalHeader()->setStretchLastSection(true);
	m_view->setAutoScroll(false);
	
	if(m_table == Table_adscene)
	{
		m_view->setColumnHidden(adscene_no, true);
			  
		m_view->setColumnWidth(1, 150);
		m_view->setColumnWidth(2, 150);
		m_view->setColumnWidth(3, 150);
		m_view->setColumnWidth(4, 50);
		
	}
	
	else
	{
		m_view->setColumnHidden(adlist_no, true);
		
		m_view->setColumnWidth(2, 500);
		m_view->setColumnWidth(3, 250);
		m_view->setColumnWidth(4, 150);
		
	}
	
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(m_view);
	m_panel->setLayout(layout);
	
}
