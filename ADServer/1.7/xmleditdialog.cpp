#include "xmleditdialog.h"

#include <QtGui>
#include <QDir>

#include "file.h"
#include "node.h"
#include "databasebrowser.h"

/*
 *  函数名：XMLEditDialog
 *  函数类型：构造函数
 *  时间：2016/10/07 20:21:07
 *  功能：初始化窗口，设置程序语言、预加载程序语言翻译文件、设置窗口UI、
 *		设置部件的属性、创建动作、刷新广告列表、连接信号与槽。
 *  
 *  参数列表
 *	    参数1：当前要做操作的文件。
 *	    参数2：程序字体。
 *		参数3：是否为中文，默认为中文。
 *		参数4：父对象，默认为0。
 *  
 */
XMLEditDialog::XMLEditDialog(File * file, 
							 QFont font, 
							 bool isCN /* = true */, 
							 QWidget * parent /* = 0 */) : QDialog(parent)
{
	setupUi(this);
	
	setFont(font);
	m_file = file;
	m_somethingChanged = false;
	
	m_appTranslatorCN.load(":/qt_zh_cn.qm");
	m_appTranslatorEN.load(":/ts_en_us.qm");
	
	sceneTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	sceneTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	sceneTreeWidget->setColumnWidth(0, 1000);
	
	adTypeWidget->setVisible(false);
	tabWidget->setVisible(false);
	
	createAction();
	setLanguage(isCN);
	refreshList();
	
	connect(sceneTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showContextMenu(QPoint)));
	connect(sceneTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
			this, SLOT(showInfo(QTreeWidgetItem*)));
	connect(sceneTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
			this, SLOT(addNodeOrSceneContextMenu(QTreeWidgetItem*)));
	connect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(changeType(int)));
	connect(OKButton, SIGNAL(clicked(bool)),
			this, SLOT(saveFile()));
	connect(browseFileButton, SIGNAL(clicked(bool)),
			this, SLOT(browseFile()));
	
	connect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setData()));
	connect(adNameLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(beginTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(endTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(intervalSpinBox, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(filePathLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(webLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
}

/*
 * 公有函数
 * 参数1：是否为中文
 * 设置程序语言
 * 
*/
void XMLEditDialog::setLanguage(bool cn)
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
 * 保护函数，重写函数
 * 当发生程序关闭时，如果当前有修改但没有保存，弹出提示进行保存或放弃操作。
 * 
*/
void XMLEditDialog::closeEvent(QCloseEvent * event)
{
	if(m_somethingChanged)
	{
		QMessageBox::StandardButton btn 
				= QMessageBox::information(this,
										   tr("编辑"),
										   tr("文件有修改，是否放弃？"),
										   QMessageBox::Yes | QMessageBox::No);
		if(btn == QMessageBox::No)
		{
			event->ignore();
			return;
		}
		
	}
	
	disconnect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			   this, SLOT(setData()));
	disconnect(adNameLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(beginTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(endTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(intervalSpinBox, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(filePathLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(webLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(textEdit, SIGNAL(textChanged()),
			   this, SLOT(setData()));
	
	m_file->m_fileParse.parse(m_file->filePath());
	QDialog::closeEvent(event);
	
}

/*
 * 保护函数，重写函数
 * 当发生语言改变时，重新设置一次程序各个已存在的文本，以适应新的语言。
 * 
*/
void XMLEditDialog::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::LanguageChange)
	{
		setWindowTitle(tr("编辑"));
		
		OKButton->setText(tr("保存(&S)"));
		
		adTypeComboBox->setItemText(0, tr("文本"));
		adTypeComboBox->setItemText(1, tr("图片"));
		adTypeComboBox->setItemText(2, tr("视频"));
		
		adTypeLabel->setText(tr("节目类型"));
		adNameLabel->setText(tr("广告名称(&N)"));
		beginTimeLabel->setText(tr("开始时间(&B)"));
		endTimeLabel->setText(tr("结束时间(&E)"));
		intervalLabel->setText(tr("时间间隔(&T)"));
		pathLabel->setText(tr("路径(&P)"));
		webLabel->setText(tr("网页(&W)"));
		textLabel->setText(tr("文本(&T)"));
		
	}
	else
	{
		QWidget::changeEvent(event);
	}
	
}

/*
 * 私有槽
 * 参数1：当前选中项
 * 显示当前选中项的详细信息。
 * 
*/
void XMLEditDialog::showInfo(QTreeWidgetItem * item)
{	
	disconnect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			   this, SLOT(setData()));
	disconnect(adNameLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(beginTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(endTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(intervalSpinBox, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(filePathLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(webLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(textEdit, SIGNAL(textChanged()),
			   this, SLOT(setData()));
	
	tabWidget->setVisible(true);
	
	if(item->parent() && item->text(0) != tr("双击添加新的广告"))
	{
		tabWidget->setCurrentIndex(1);
		filePathLineEdit->clear();
		webLineEdit->clear();
		textEdit->clear();
		adTypeWidget->setVisible(true);
		
		QList<Scene *> sc = m_file->m_fileParse.scene();
		
		foreach(Scene * s, sc)
		{
			if(s->name() == item->parent()->text(0))
			{
				Node * node = s->node(sceneTreeWidget->currentIndex().row());
				switch(node->type())
				{
				case Type_text:
					textEdit->setEnabled(true);
					textEdit->setPlainText(static_cast<TextNode *>(node)->text());
					
					filePathLineEdit->setEnabled(false);
					browseFileButton->setEnabled(false);
					webLineEdit->setEnabled(false);
					
					break;
				case Type_image:
				case Type_video:
					filePathLineEdit->setEnabled(true);
					browseFileButton->setEnabled(true);
					webLineEdit->setEnabled(true);
					filePathLineEdit->setText(static_cast<MediaNode *>(node)->file());
					webLineEdit->setText(static_cast<MediaNode *>(node)->url());
					
					textEdit->setEnabled(false);
					
					break;
				}
				adTypeComboBox->setCurrentIndex(static_cast<int>(node->type()));
				break;
			}
		}
	}
	else if(!item->parent() && item->text(0) != tr("双击添加新的广告节目单"))
	{
		tabWidget->setCurrentIndex(0);
		adNameLineEdit->clear();
		beginTimeEdit->setTime(QTime(0, 0, 0));
		endTimeEdit->setTime(QTime(0, 0, 0));
		intervalSpinBox->cleanText();
		adTypeWidget->setVisible(false);
		
		Scene * sc = m_file->m_fileParse.scene().at(sceneTreeWidget->currentIndex().row());
		adNameLineEdit->setText(sc->name());
		beginTimeEdit->setTime(sc->begin());
		endTimeEdit->setTime(sc->end());
		intervalSpinBox->setValue(sc->interval());
	}
	else
	{
		tabWidget->setVisible(false);
		adTypeWidget->setVisible(false);
	}
	
	connect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setData()));
	connect(adNameLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(beginTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(endTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(intervalSpinBox, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(filePathLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(webLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
	
}

/*
 *  函数名：showContextMenu
 *  函数类型：私有槽
 *  时间：2016/10/07 21:00:36
 *  功能：在[广告列表]中使用鼠标右键，显示上下文菜单。
 *		如果在项上鼠标右键，则是项的相关操作。
 *		如果在非项上鼠标右键，则是添加广告节目单的操作。
 *  
 *  参数列表
 *	    参数1：当前鼠标位置。
 *  
 */
void XMLEditDialog::showContextMenu(QPoint pos)
{
	m_sceneMenu->clear();
	
	QTreeWidgetItem * item = sceneTreeWidget->itemAt(pos);
	if(item)
	{
		if(item->text(0) == tr("双击添加新的广告")
				|| item->text(0) == tr("双击添加新的广告节目单"))
			return;
		else if(item->parent())
			m_sceneMenu->addAction(m_deleteNodeAction);
		else
			m_sceneMenu->addAction(m_deleteSceneAction);
	}
	else
	{
		m_sceneMenu->addAction(m_newSceneAction);
	}
	m_sceneMenu->exec(QCursor::pos());
	
}

/*
 *  函数名：addNodeOrSceneContextMenu
 *  函数类型：私有槽
 *  时间：2016/10/07 20:46:27
 *  功能：当双击“双击添加新的广告”或者“双击添加新的广告节目单”时弹出相应上下文菜单。
 * 
 *	参数列表：
 *		参数1：当前被双击的项。
 *  
 */
void XMLEditDialog::addNodeOrSceneContextMenu(QTreeWidgetItem * item)
{
	m_sceneMenu->clear();
	
	if(item->text(0) == tr("双击添加新的广告"))
	{
		m_sceneMenu->addAction(m_addNodeDirectlyAction);
		m_sceneMenu->addAction(m_addFromDatabaseAction);
	}
	else if(item->text(0) == tr("双击添加新的广告节目单"))
	{
		m_sceneMenu->addAction(m_addSceneDirectlyAction);
		m_sceneMenu->addAction(m_addFromDatabaseAction);
	}
	else
	{
		return;
	}
	
	QAction * resAction = m_sceneMenu->exec(QCursor::pos());
	if(resAction)
	{
		bool fromDatabase = (resAction == m_addFromDatabaseAction);
		
		if(fromDatabase && !DatabaseBrowser::isConnected())
		{
			QMessageBox::information(this, tr("编辑"),
									 tr("数据库未连接。"));
			return;
		}
		
		createNewSceneOrNode(fromDatabase);
	}
	
}

/*
 * 私有槽
 * 参数1：[广告类型下拉列表]当前选中的索引
 * 改变当前广告的类型。
 * 
*/
void XMLEditDialog::changeType(int index)
{
	if(index == 0)
	{
		textEdit->setEnabled(true);
		
		filePathLineEdit->setEnabled(false);
		browseFileButton->setEnabled(false);
		webLineEdit->setEnabled(false);
	}
	else
	{
		filePathLineEdit->setEnabled(true);
		browseFileButton->setEnabled(true);
		webLineEdit->setEnabled(true);
		
		textEdit->setEnabled(false);
	}
	
}

/*
 * 私有槽
 * 保存文件。
 * 
*/
void XMLEditDialog::saveFile()
{
	if(m_file->m_fileParse.writeXML(m_file->filePath()))
	{
		emit fileChange();
		QMessageBox::information(this,
								 tr("编辑"),
								 tr("保存成功。"));
		m_somethingChanged = false;
	}
	else
	{
		QMessageBox::information(this,
								 tr("编辑"),
								 tr("保存失败。"));
	}
	
}

/*
 * 私有槽
 * 当编辑完成后会更新相应的广告数据，但不会保存到文件。
 * 
*/
void XMLEditDialog::setData()
{
	QTreeWidgetItem * item = sceneTreeWidget->currentItem();
	int itemRow = sceneTreeWidget->currentIndex().row();
	if(adTypeComboBox->isVisible())
	{
		int parentRow = sceneTreeWidget->currentIndex().parent().row();
		
		switch (adTypeComboBox->currentIndex())
		{
		case 0:
		{
			QString str = textEdit->toPlainText();
			str = str.mid(0, str.indexOf('\n') == -1 ? str.size() : (str.indexOf('\n')));
			
			if(m_file->m_fileParse.scene()[parentRow]->node(itemRow)->type() != Type_text)
				m_file->m_fileParse.scene()[parentRow]->modifyNodeType(itemRow, Type_text, str);
			else
				static_cast<TextNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setText(textEdit->toPlainText());
			
			item->setText(0, str);
			
			break;
		}
			
		case 1:
		{
			if(m_file->m_fileParse.scene()[parentRow]->node(itemRow)->type() != Type_image)
			{
				m_file->m_fileParse.scene()[parentRow]->modifyNodeType(itemRow, Type_image, filePathLineEdit->text(), webLineEdit->text());
			}
			else
			{
				filePathLineEdit->setText(QDir::toNativeSeparators(filePathLineEdit->text()));
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setFile(filePathLineEdit->text());
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setUrl(webLineEdit->text());
			}
			
			item->setText(0, filePathLineEdit->text());
			
			break;
		}
			
		case 2:
		{
			if(m_file->m_fileParse.scene()[parentRow]->node(itemRow)->type() != Type_video)
			{
				m_file->m_fileParse.scene()[parentRow]->modifyNodeType(itemRow, Type_video, filePathLineEdit->text(), webLineEdit->text());
			}
			else
			{
				filePathLineEdit->setText(QDir::toNativeSeparators(filePathLineEdit->text()));
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setFile(filePathLineEdit->text());
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setUrl(webLineEdit->text());
			}
			
			item->setText(0, filePathLineEdit->text());
			
			break;
		}
		}
	}
	else
	{
		item->setText(0, adNameLineEdit->text());
		
		m_file->m_fileParse.scene()[itemRow]->setName(adNameLineEdit->text());
		m_file->m_fileParse.scene()[itemRow]->setBegin(beginTimeEdit->time());
		m_file->m_fileParse.scene()[itemRow]->setEnd(endTimeEdit->time());
		m_file->m_fileParse.scene()[itemRow]->setInterval(intervalSpinBox->value());
		
	}
	m_somethingChanged = true;
	
}

/*
 * 私有槽
 * 浏览图片/视频文件。
 * 
*/
void XMLEditDialog::browseFile()
{
	QString fileFilter;
	if(adTypeComboBox->currentIndex() == 1)
	{
		// 图片
		fileFilter = tr("jpg文件 (*.jpg);;bmp文件 (*.bmp);;png文件 (*.png);;所有文件 (*.*)");
	}
	else
	{
		// 视频
		fileFilter = tr("rmvb文件 (*.rmvb);;mp4文件 (*.mp4);;所有文件 (*.*)");
	}
	
	QString filePath = QFileDialog::getOpenFileName(this,
													tr("打开文件"),
													".",
													fileFilter);
	
	if(filePath.isEmpty())
		return;
	
	filePathLineEdit->setText(filePath);
	setData();
	
}

/*
 * 私有槽
 * 添加新的广告节目单
 * 
*/
void XMLEditDialog::newScene()
{
	QList<QTreeWidgetItem*> item = sceneTreeWidget->findItems(tr("双击添加新的广告节目单"), Qt::MatchExactly);
	
	sceneTreeWidget->setCurrentItem(item.at(0));
	createNewSceneOrNode();
	
}

/*
 * 私有槽
 * 删除选中的广告节目单
 * 
*/
void XMLEditDialog::deleteScene()
{
	QMessageBox::StandardButton btn = QMessageBox::information(this,
															   tr("编辑"),
															   tr("是否确认删除该广告节目单？"),
															   QMessageBox::Ok | QMessageBox::Cancel);
	if(btn == QMessageBox::Cancel)
		return;
	
	m_file->m_fileParse.removeScene(sceneTreeWidget->currentIndex().row());
	refreshList();
	
	adTypeWidget->setVisible(false);
	tabWidget->setVisible(false);
	
	m_somethingChanged = true;
	
}

/*
 * 私有槽
 * 删除选中的广告
 * 
*/
void XMLEditDialog::deleteNode()
{
	QMessageBox::StandardButton btn = QMessageBox::information(this,
															   tr("编辑"),
															   tr("是否确认删除该广告？"),
															   QMessageBox::Ok | QMessageBox::Cancel);
	if(btn == QMessageBox::Cancel)
		return;
	
	int parentRow = sceneTreeWidget->currentIndex().parent().row();
	m_file->m_fileParse.scene()[parentRow]->removeNode(sceneTreeWidget->currentIndex().row());
	refreshList();
	
	adTypeWidget->setVisible(false);
	tabWidget->setVisible(false);
	
	m_somethingChanged = true;
	
}

/*
 * 私有函数
 * 刷新[广告列表]。
 * 
*/
void XMLEditDialog::refreshList()
{
	sceneTreeWidget->clear();
	
	QList<Scene *> sc = m_file->m_fileParse.scene();
	
	foreach (Scene * s, sc)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(sceneTreeWidget->invisibleRootItem());
		item->setText(0, s->name());
		
		foreach(Node * n, s->nodeList())
		{
			QTreeWidgetItem * subItem = new QTreeWidgetItem(item);
			
			switch (n->type())
			{
			case Type_image:
			case Type_video:
				subItem->setText(0, static_cast<MediaNode *>(n)->file());
				
				break;
				
			case Type_text:
				QString str = static_cast<TextNode *>(n)->text();
				str = str.mid(0, str.indexOf('\n') == -1 ? str.size() : (str.indexOf('\n')));
				subItem->setText(0, str);
				
				break;
			}
		}
		QTreeWidgetItem * addNodeItem = new QTreeWidgetItem(item);
		addNodeItem->setText(0, tr("双击添加新的广告"));
		addNodeItem->setForeground(0, QBrush(Qt::red));
		
	}
	
	sceneTreeWidget->expandAll();
	
	QTreeWidgetItem * addSceneItem = new QTreeWidgetItem(sceneTreeWidget->invisibleRootItem());
	addSceneItem->setText(0, tr("双击添加新的广告节目单"));
	addSceneItem->setForeground(0, QBrush(Qt::red));
	
}

/*
 *  函数名：createNewSceneOrNode
 *  函数类型：私有函数
 *  时间：2016/10/07 20:39:02
 *  功能：直接创建或从数据库中选择一项来创建一个广告或广告节目单。
 *  
 *  参数列表
 *	    参数1：是否从数据库中选择广告或广告节目单，默认是false。
 *  
 */
void XMLEditDialog::createNewSceneOrNode(bool fromDatabase /* = false */)
{
	disconnect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			   this, SLOT(setData()));
	disconnect(adNameLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(beginTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(endTimeEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(intervalSpinBox, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(filePathLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(webLineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(textEdit, SIGNAL(textChanged()),
			   this, SLOT(setData()));
	
	tabWidget->setVisible(true);
	
	QTreeWidgetItem * item = sceneTreeWidget->currentItem();
	
	if(item->parent() && item->text(0) == tr("双击添加新的广告"))
	{
		tabWidget->setCurrentIndex(1);
		filePathLineEdit->clear();
		webLineEdit->clear();
		textEdit->clear();
		adTypeWidget->setVisible(true);
		
		QList<Scene *> sc = m_file->m_fileParse.scene();
		
		Node * newNode = NULL;
		foreach(Scene * s, sc)
		{
			if(s->name() == item->parent()->text(0))
			{
				if(!fromDatabase)
				{
					newNode = new TextNode(tr("新广告"));
				}
				else
				{
					newNode = DatabaseBrowser::selectADInfo(windowTitle() == "编辑", font());
					if(newNode == NULL)
					{
						tabWidget->setVisible(false);
						adTypeWidget->setVisible(false);
						return;
					}
				}
				s->addNode(newNode);
				
				break;
			}
		}
		
		QTreeWidgetItem * newItem = new QTreeWidgetItem(item->parent());
		newItem->setText(0, tr("双击添加新的广告"));
		newItem->setForeground(0, QBrush(Qt::red));
		
		item->setForeground(0, QBrush(Qt::black));
		if(!fromDatabase)
		{
			item->setText(0, tr("新广告"));
			textEdit->setFocus();
		}
		else
		{
			if(newNode->type() == Type_text)
			{
				QString str = static_cast<TextNode *>(newNode)->text();
				str = str.mid(0, str.indexOf('\n') == -1 ? str.size() : (str.indexOf('\n')));
				item->setText(0, str);
				
				adTypeComboBox->setCurrentIndex(0);
				textEdit->setText(static_cast<TextNode *>(newNode)->text());
			}
			else
			{
				item->setText(0, static_cast<MediaNode *>(newNode)->file());
				
				adTypeComboBox->setCurrentIndex(newNode->type() == Type_image ? 1 : 2);
				filePathLineEdit->setText(static_cast<MediaNode *>(newNode)->file());
				webLineEdit->setText(static_cast<MediaNode *>(newNode)->url());
				
			}
			
		}
		
	}
	else if(item->text(0) == tr("双击添加新的广告节目单"))
	{
		tabWidget->setCurrentIndex(0);
		adNameLineEdit->clear();
		beginTimeEdit->setTime(QTime(0, 0, 0));
		endTimeEdit->setTime(QTime(0, 0, 0));
		intervalSpinBox->setValue(0);
		adTypeWidget->setVisible(false);
		
		Scene * newScene;
		if(!fromDatabase)
		{
			newScene = new Scene(tr("新节目单"));
			m_file->m_fileParse.addScene(newScene);
			adNameLineEdit->setFocus();
		}
		else
		{
			newScene = DatabaseBrowser::selectADScene(windowTitle() == "编辑", font());
			if(newScene == NULL)
			{
				tabWidget->setVisible(false);
				return;
			}
			m_file->m_fileParse.addScene(newScene);
		}
		
		QTreeWidgetItem * newItem = new QTreeWidgetItem(sceneTreeWidget->invisibleRootItem());
		newItem->setText(0, tr("双击添加新的广告节目单"));
		newItem->setForeground(0, QBrush(Qt::red));
		
		QTreeWidgetItem * subItem = new QTreeWidgetItem(item);
		subItem->setText(0, tr("双击添加新的广告"));
		subItem->setForeground(0, QBrush(Qt::red));
		
		item->setText(0, fromDatabase ? newScene->name() : tr("新节目单"));
		item->setForeground(0, QBrush(Qt::black));
		adNameLineEdit->setText(fromDatabase ? newScene->name() : tr("新节目单"));
		
		beginTimeEdit->setTime(fromDatabase ? newScene->begin() : QTime(0, 0, 0));
		endTimeEdit->setTime(fromDatabase ? newScene->end() : QTime(0, 0, 0));
		intervalSpinBox->setValue(fromDatabase ? newScene->interval() : 0);
		
	}
	
	m_somethingChanged = true;
	
	connect(adTypeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setData()));
	connect(adNameLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(beginTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(endTimeEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(intervalSpinBox, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(filePathLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(webLineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
}

/*
 * 私有函数
 * 初始化上下文菜单的选项。
 * 
*/
void XMLEditDialog::createAction()
{
	m_sceneMenu = new QMenu(this);
	
	m_newSceneAction = new QAction(tr("新广告节目单(&N)"), this);
	m_deleteSceneAction = new QAction(tr("删除广告节目单(&D)"), this);
	m_deleteNodeAction = new QAction(tr("删除广告(&R)"), this);
	
	m_addNodeDirectlyAction = new QAction(tr("直接添加广告"), this);
	m_addSceneDirectlyAction = new QAction(tr("直接添加广告节目单"), this);
	m_addFromDatabaseAction = new QAction(tr("从数据库中添加"), this);
	
	connect(m_newSceneAction, SIGNAL(triggered(bool)),
			this, SLOT(newScene()));
	connect(m_deleteSceneAction, SIGNAL(triggered(bool)),
			this, SLOT(deleteScene()));
	connect(m_deleteNodeAction, SIGNAL(triggered(bool)),
			this, SLOT(deleteNode()));
	
}
