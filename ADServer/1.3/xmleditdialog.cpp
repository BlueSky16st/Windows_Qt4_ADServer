#include "xmleditdialog.h"

#include <QtGui>

#include "file.h"
#include "node.h"

XMLEditDialog::XMLEditDialog(File * file, QFont font, bool isCN, QWidget * parent) : QDialog(parent)
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
	connect(sceneTreeWidget, SIGNAL(doubleClicked(QModelIndex)),
			this, SLOT(createNewAD()));
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
	connect(lineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
}

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
	
	m_file->m_fileParse.parse(m_file->filePath());
	QDialog::closeEvent(event);
	
}

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
	disconnect(lineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(textEdit, SIGNAL(textChanged()),
			   this, SLOT(setData()));
	
	tabWidget->setVisible(true);
	if(item->parent() && item->text(0) != tr("双击添加新的广告节目"))
	{
		tabWidget->setCurrentIndex(1);
		lineEdit->clear();
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
					
					lineEdit->setEnabled(false);
					browseFileButton->setEnabled(false);
					
					break;
				case Type_image:
				case Type_video:
					lineEdit->setEnabled(true);
					browseFileButton->setEnabled(true);
					lineEdit->setText(static_cast<MediaNode *>(node)->file());
					
					textEdit->setEnabled(false);
					
					break;
				}
				adTypeComboBox->setCurrentIndex(static_cast<int>(node->type()));
				break;
			}
		}
	}
	else if(!item->parent() && item->text(0) != tr("双击添加新的广告节点"))
	{
		tabWidget->setCurrentIndex(0);
		adNameLineEdit->clear();
		beginTimeEdit->clear();
		endTimeEdit->clear();
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
	connect(lineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
}

void XMLEditDialog::createNewAD()
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
	disconnect(lineEdit, SIGNAL(editingFinished()),
			   this, SLOT(setData()));
	disconnect(textEdit, SIGNAL(textChanged()),
			   this, SLOT(setData()));
	
	tabWidget->setVisible(true);
	QTreeWidgetItem * item = sceneTreeWidget->currentItem();
	
	if(item->parent() && item->text(0) == tr("双击添加新的广告节目"))
	{
		tabWidget->setCurrentIndex(1);
		lineEdit->clear();
		textEdit->clear();
		adTypeWidget->setVisible(true);
		
		QList<Scene *> sc = m_file->m_fileParse.scene();
		
		QTreeWidgetItem * newItem = new QTreeWidgetItem(item->parent());
		newItem->setText(0, tr("双击添加新的广告节目"));
		newItem->setForeground(0, QBrush(Qt::red));
		
		item->setText(0, tr("新节目"));
		item->setForeground(0, QBrush(Qt::black));
		
		foreach(Scene * s, sc)
		{
			if(s->name() == item->parent()->text(0))
			{
				Node * newNode = new TextNode(tr("新节目"));
				s->addNode(newNode);
				
				break;
			}
		}
		
		adTypeComboBox->setFocus();
		
	}
	else if(item->text(0) == tr("双击添加新的广告节点"))
	{
		tabWidget->setCurrentIndex(0);
		adNameLineEdit->clear();
		beginTimeEdit->clear();
		endTimeEdit->clear();
		intervalSpinBox->cleanText();
		adTypeWidget->setVisible(false);
		
		QTreeWidgetItem * newItem = new QTreeWidgetItem(sceneTreeWidget->invisibleRootItem());
		newItem->setText(0, tr("双击添加新的广告节点"));
		newItem->setForeground(0, QBrush(Qt::red));
		
		QTreeWidgetItem * subItem = new QTreeWidgetItem(item);
		subItem->setText(0, tr("双击添加新的广告节目"));
		subItem->setForeground(0, QBrush(Qt::red));
		
		item->setText(0, tr("新广告"));
		item->setForeground(0, QBrush(Qt::black));
		adNameLineEdit->setText(tr("新广告"));
		
		m_file->m_fileParse.addScene(new Scene(tr("新广告")));
		
		adNameLineEdit->setFocus();
		
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
	connect(lineEdit, SIGNAL(editingFinished()),
			this, SLOT(setData()));
	connect(textEdit, SIGNAL(textChanged()),
			this, SLOT(setData()));
	
}

void XMLEditDialog::showContextMenu(QPoint pos)
{
	m_sceneMenu->clear();
	
	QTreeWidgetItem * item = sceneTreeWidget->itemAt(pos);
	if(item)
	{
		if(item->parent() && item->text(0) != tr("双击添加新的广告节目"))
			m_sceneMenu->addAction(m_deleteNodeAction);
		else if(item->text(0) != tr("双击添加新的广告节点") 
				&& item->text(0) != tr("双击添加新的广告节目"))
			m_sceneMenu->addAction(m_deleteSceneAction);
	}
	else
	{
		m_sceneMenu->addAction(m_newSceneAction);
	}
	m_sceneMenu->exec(QCursor::pos());
	
}

void XMLEditDialog::changeType(int index)
{
	if(index == 0)
	{
		textEdit->setEnabled(true);
		
		lineEdit->setEnabled(false);
		browseFileButton->setEnabled(false);
	}
	else
	{
		lineEdit->setEnabled(true);
		browseFileButton->setEnabled(true);
		
		textEdit->setEnabled(false);
	}
	
}

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
				m_file->m_fileParse.scene()[parentRow]->modifyNodeType(itemRow, Type_image, lineEdit->text());
			}
			else
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setFile(lineEdit->text());
			
			item->setText(0, lineEdit->text());
			
			break;
		}
			
		case 2:
		{
			if(m_file->m_fileParse.scene()[parentRow]->node(itemRow)->type() != Type_video)
			{
				m_file->m_fileParse.scene()[parentRow]->modifyNodeType(itemRow, Type_video, lineEdit->text());
			}
			else
				static_cast<MediaNode *>(m_file->m_fileParse.scene()[parentRow]->node(itemRow))->setFile(lineEdit->text());
			
			item->setText(0, lineEdit->text());
			
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

void XMLEditDialog::browseFile()
{
	QString fileFilter;
	if(adTypeComboBox->currentIndex() == 1)
	{
		// 图片
		fileFilter = tr("jpeg文件 (*.jpeg *.jpg);;bmp文件 (*.bmp);;所有文件 (*.*)");
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
	
	lineEdit->setText(filePath);
	setData();
	
}

void XMLEditDialog::newScene()
{
	QList<QTreeWidgetItem*> item = sceneTreeWidget->findItems(tr("双击添加新的广告节点"), Qt::MatchExactly);
	
	sceneTreeWidget->setCurrentItem(item.at(0));
	createNewAD();
	
}

void XMLEditDialog::deleteScene()
{
	QMessageBox::StandardButton btn = QMessageBox::information(this,
															   tr("编辑"),
															   tr("是否确认删除该广告节点？"),
															   QMessageBox::Ok | QMessageBox::Cancel);
	if(btn == QMessageBox::Cancel)
		return;
	
	m_file->m_fileParse.removeScene(sceneTreeWidget->currentIndex().row());
	refreshList();
	
	adTypeWidget->setVisible(false);
	tabWidget->setVisible(false);
	
	m_somethingChanged = true;
	
}

void XMLEditDialog::deleteNode()
{
	QMessageBox::StandardButton btn = QMessageBox::information(this,
															   tr("编辑"),
															   tr("是否确认删除该广告节目？"),
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
		textLabel->setText(tr("文本(&T)"));
		
	}
	else
	{
		QWidget::changeEvent(event);
	}
	
}

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
				//subItem->setText(0, static_cast<TextNode *>(n)->text());
				
				QString str = static_cast<TextNode *>(n)->text();
				str = str.mid(0, str.indexOf('\n') == -1 ? str.size() : (str.indexOf('\n')));
				subItem->setText(0, str);
				
				break;
			}
		}
		QTreeWidgetItem * addNodeItem = new QTreeWidgetItem(item);
		addNodeItem->setText(0, tr("双击添加新的广告节目"));
		addNodeItem->setForeground(0, QBrush(Qt::red));
		
	}
	
	sceneTreeWidget->expandAll();
	
	QTreeWidgetItem * addSceneItem = new QTreeWidgetItem(sceneTreeWidget->invisibleRootItem());
	addSceneItem->setText(0, tr("双击添加新的广告节点"));
	addSceneItem->setForeground(0, QBrush(Qt::red));
	
}

void XMLEditDialog::createAction()
{
	m_sceneMenu = new QMenu(this);
	
	m_newSceneAction = new QAction(tr("新广告节点(&N)"), this);
	m_deleteSceneAction = new QAction(tr("删除广告节点(&D)"), this);
	m_deleteNodeAction = new QAction(tr("删除节目(&R)"), this);
	
	connect(m_newSceneAction, SIGNAL(triggered(bool)),
			this, SLOT(newScene()));
	connect(m_deleteSceneAction, SIGNAL(triggered(bool)),
			this, SLOT(deleteScene()));
	connect(m_deleteNodeAction, SIGNAL(triggered(bool)),
			this, SLOT(deleteNode()));
	
}
