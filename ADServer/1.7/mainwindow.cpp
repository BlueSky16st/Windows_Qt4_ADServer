#include "mainwindow.h"

#include <QtGui>
#include <QMap>
#include "file.h"
#include "node.h"
#include "xmleditdialog.h"
#include "tcpserver.h"
#include "databasebrowser.h"

/*
 * 构造函数
 * 参数1：父对象，默认为0
 * 设置程序编码、预加载程序语言翻译文件、设置主窗口UI、
 * 设置默认字体大小、创建动作、连接信号与槽、设置部件的属性、
 * 读取已保存的程序设置。
 * 
*/
MainWindow::MainWindow(QWidget * parent /* = 0 */) : QMainWindow(parent)
{
	QTextCodec * codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);
	
	m_appTranslatorCN.load(":/qt_zh_cn.qm");
	m_appTranslatorEN.load(":/ts_en_us.qm");
	
	setupUi(this);
	
	QFont font = QApplication::font();
	font.setPointSize(10);
	setFont(font);
	
	createAction();
	initConnect();
	
	fileListTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	fileListTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
	fileListTreeWidget->setColumnWidth(2, 1000);
	
	fileBrowseTreeWidget->setColumnWidth(0, 1000);
	
	settingsTableWidget->item(0, 0)->setSelected(true);
	
	QPalette pa;
	pa.setColor(QPalette::WindowText, Qt::red);
	databaseState->setPalette(pa);
	
	m_server = NULL;
	
	setWindowState(Qt::WindowMaximized);
	
	readSettings();
	
}

/*
 * 析构函数
 * 删除所有文件列表的内容。
 * 
*/
MainWindow::~MainWindow()
{
	qDeleteAll(m_fileList);
	m_fileList.clear();
	
}

/*
 * 保护函数，重写函数
 * 当发生程序关闭时，检测服务器是否正在开启，并保存程序的相关设置。
 * 
*/
void MainWindow::closeEvent(QCloseEvent * event)
{
	if(m_server != NULL && m_server->isListening())
	{
		m_server->close();
		delete m_server;
		addMessage(tr("服务器已关闭。"));
	}
	
	DatabaseBrowser::closeConnection();
	
	saveSettings();
	QMainWindow::closeEvent(event);
	
}

/*
 * 保护函数，重写函数
 * 当发生语言改变时，重新设置一次程序各个已存在的文本，以适应新的语言。
 * 
*/
void MainWindow::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::LanguageChange)
	{
		setWindowTitle(tr("广告服务器"));
		
		tabWidget->setTabText(0, tr("编辑器"));
		tabWidget->setTabText(1, tr("服务器"));
		tabWidget->setTabText(2, tr("程序设置"));
		
		databaseStateLabel->setText(tr("数据库状态："));
		if(!connectOrDisconnectDatabaseButton->isChecked())
		{
			databaseState->setText(tr("未连接"));
			connectOrDisconnectDatabaseButton->setText(tr("连接数据库(&B)"));
		}
		else
		{
			databaseState->setText(tr("已连接"));
			connectOrDisconnectDatabaseButton->setText(tr("关闭数据库(&B)"));
		}
		
		editDatabaseButton->setText(tr("编辑数据库(&I)"));
		
		serverLogLabel->setText(tr("服务器日志"));
		fileListLabel->setText(tr("文件列表"));
		fileBrowseLabel->setText(tr("文件浏览"));
		languageLabel->setText(tr("语言："));
		fontLabel->setText(tr("字体："));
		
		fileListTreeWidget->headerItem()->setText(0, tr("文件名"));
		fileListTreeWidget->headerItem()->setText(1, tr("发送ID"));
		fileListTreeWidget->headerItem()->setText(2, tr("文件路径"));
		
		newFileButton->setText(tr("新建文件(&N)"));
		newFileButton->setToolTip(tr("创建一个新的文件。"));
		newFileButton->setStatusTip(tr("创建一个新的文件。"));
		
		addFileButton->setText(tr("添加文件(&A)"));
		addFileButton->setToolTip(tr("添加adm文件到文件列表中。"));
		addFileButton->setStatusTip(tr("添加adm文件到文件列表中。"));
		
		clearListButton->setText(tr("清空列表(&C)"));
		clearListButton->setToolTip(tr("清空文件列表。"));
		clearListButton->setStatusTip(tr("清空文件列表。"));
		
		editFileButton->setText(tr("编辑文件(&E)"));
		editFileButton->setToolTip(tr("编辑当前选中的文件。"));
		editFileButton->setStatusTip(tr("编辑当前选中的文件。"));
		
		removeFileButton->setText(tr("移除文件(&R)"));
		removeFileButton->setToolTip(tr("将文件列表中选中的文件移除。"));
		removeFileButton->setStatusTip(tr("将文件列表中选中的文件移除。"));
		
		setIDButton->setText(tr("设置ID(&S)"));
		setIDButton->setToolTip(tr("设置选中的文件发送ID，用于将该文件发送到对应ID的客户端。"));
		setIDButton->setStatusTip(tr("设置选中的文件发送ID，用于将该文件发送到对应ID的客户端。"));
		
		fileBrowseTreeWidget->headerItem()->setText(0, tr("浏览"));
		
		clearLogButton->setText(tr("清空日志(&C)"));
		clearLogButton->setToolTip(tr("清空当前日志。"));
		clearLogButton->setStatusTip(tr("清空当前日志。"));
		
		saveLogButton->setText(tr("保存日志(&L)"));
		saveLogButton->setToolTip(tr("将当前日志保存到指定文本文件中。"));
		saveLogButton->setStatusTip(tr("将当前日志保存到指定文本文件中。"));
		
		serverButton->setText(tr("启动服务(&S)"));
		serverButton->setToolTip(tr("启动服务，接收来自客户端的请求。"));
		serverButton->setStatusTip(tr("启动服务，接收来自客户端的请求。"));
		
		curClientCountLabel->setText(tr("当前客户端连接数："));
		
		resetButton->setText(tr("重置(&R)"));
		resetButton->setToolTip(tr("重置所有设置。"));
		resetButton->setStatusTip(tr("重置所有设置。"));
		
		selectLanguageComboBox->setToolTip(tr("设置程序语言。"));
		selectLanguageComboBox->setStatusTip(tr("设置程序语言。"));
		
		setFontButton->setText(tr("设置字体"));
		setFontButton->setToolTip(tr("设置程序字体。"));
		setFontButton->setStatusTip(tr("设置程序字体。"));
		
		saveFileListCheckBox->setText(tr("自动保存文件列表"));
		saveFileListCheckBox->setToolTip(tr("设置在程序退出时是否自动保存文件列表。"));
		saveFileListCheckBox->setStatusTip(tr("设置在程序退出时是否自动保存文件列表。"));
		
		saveGeneralSettingsCheckBox->setText(tr("自动保存程序一般设置"));
		saveGeneralSettingsCheckBox->setToolTip(tr("设置在程序退出时是否自动保存常用设置，例如程序大小、位置、字体、语言等。"));
		saveGeneralSettingsCheckBox->setStatusTip(tr("设置在程序退出时是否自动保存常用设置，例如程序大小、位置、字体、语言等。"));
		
		saveLogCheckBox->setText(tr("自动保存服务器日志"));
		saveLogCheckBox->setToolTip(tr("设置在程序退出时是否自动保存服务器日志。"));
		saveLogCheckBox->setStatusTip(tr("设置在程序退出时是否自动保存服务器日志。"));
		
		settingsTableWidget->item(0, 0)->setText(tr("常规"));
		settingsTableWidget->item(1, 0)->setText(tr("保存"));
		settingsTableWidget->item(2, 0)->setText(tr("关于"));
		
		interfaceGroupBox->setTitle(tr("界面"));
		saveSettingGroupBox->setTitle(tr("保存设置"));
		
	}
	else
	{
		QWidget::changeEvent(event);
	}
	
}

/*
 * 私有槽
 * 创建一个新的adm文件。
 * 
*/
void MainWindow::newFile()
{
	QString filePath = QFileDialog::getSaveFileName(this,
													tr("保存文件"),
													".",
													tr("adm文件 (*.adm);;所有文件 (*.*)"));
	if(filePath.isEmpty())
		return;
	
	QFile * newFile = new QFile(filePath);
	if(!newFile->open(QFile::WriteOnly))
	{
		QMessageBox::information(this,
								 tr("广告服务器"),
								 tr("文件保存失败\n原因：") + newFile->errorString());
		return;
	}
	newFile->close();
	File * file = new File(filePath);
	
	m_fileList.append(file);
	refreshList();
	
}

/*
 * 私有槽
 * 添加一个现有的adm文件。
 * 
*/
void MainWindow::addFile()
{
	QString filePath = QFileDialog::getOpenFileName(this,
													tr("打开文件"),
													".",
													tr("adm文件 (*.adm)"));
	if(filePath.isEmpty())
		return;
	
	foreach(File * f, m_fileList)
	{
		if(f->filePath() == filePath)
		{
			QMessageBox::information(this,
									 tr("广告服务器"),
									 tr("列表中已存在相应文件。"));
			return;
		}
	}
	
	File * file = new File(filePath);
	m_fileList.append(file);
	refreshList();
	
}

/*
 * 私有槽
 * 移除选中的文件。
 * 
*/
void MainWindow::removeFile()
{
	m_fileList.removeAt(fileListTreeWidget->currentIndex().row());
	refreshList();
	
}

/*
 * 私有槽
 * 清除[文件列表]的内容。
 * 
*/
void MainWindow::clearList()
{
	m_fileList.clear();
	refreshList();
	
}

/*
 * 私有槽
 * 创建新模态对话框，用于接收用户输入的发送ID。
 * 发送ID需要符合一定规则才会成功设置ID。
 * 
*/
void MainWindow::setFileSendID()
{
	int index = fileListTreeWidget->currentIndex().row();
	if(index < 0)
		return;
	
	File * file = m_fileList.at(index);
	QString id = file->sendID();
	bool ok;
	
	
	forever
	{
		id = QInputDialog::getText(this,
								   tr("设置发送ID"),
								   tr("ID只能为数字，有多个ID时使用\"空格\"分隔\n"
									  "如果为空则为清空所有ID号\n"
									  "样式：2 4-6 7\n"
									  "根据样式最后得到的ID号为：2 4 5 6 7\n"),
								   QLineEdit::Normal,
								   id,
								   &ok,
								   Qt::Popup);
		
		
		if(!ok)
			return;
		
		if(file->setSendID(id))
		{
			fileListTreeWidget->currentItem()->setText(1, file->sendID().isEmpty() ? tr("无") : file->sendID());
			break;
		}
		else
		{
			QMessageBox::information(this,
									 tr("设置发送ID"),
									 tr("格式有误，请重新输入"));
		}
		
	}
	
}

/*
 * 私有槽
 * 创建新模态对话框，用于编辑当前[文件列表]中选中的文件的内容。
 * 如果该对话框进行了保存，会通知主窗口对[文件浏览]的内容进行更新。
 * 
*/
void MainWindow::editFile()
{
	int index = fileListTreeWidget->currentIndex().row();
	if(index < 0)
		return;
	
	XMLEditDialog dlg(m_fileList.at(index), 
					  font(),
					  selectLanguageComboBox->currentIndex() == 0,
					  this);
	
	connect(&dlg, SIGNAL(fileChange()),
			this, SLOT(fileBrowse()));
	
	dlg.exec();
	
}

/*
 * 私有槽
 * 参数1：当前鼠标位置
 * 在[文件列表]中使用鼠标右键，显示上下文菜单。
 * 鼠标位置不同会使菜单选项不同。
 * 如果在项上鼠标右键，则会有关于项的相关操作。
 * 如果在非项上鼠标右键，则会有关于文件的相关操作。
 * 
*/
void MainWindow::showFileListContextMenu(QPoint pos)
{
	m_fileListMenu->clear();
	
	QTreeWidgetItem * item = fileListTreeWidget->itemAt(pos);
	if(item)
	{
		m_fileListMenu->addAction(m_removeFileAction);
		m_fileListMenu->addAction(m_setFileSendIDAction);
	}
	else
	{
		m_fileListMenu->addAction(m_newFileAction);
		m_fileListMenu->addAction(m_addFileAction);
		m_fileListMenu->addAction(m_clearListAction);
	}
	m_fileListMenu->exec(QCursor::pos());
	
}

/*
 * 私有槽
 * 对当前[文件列表]选中的文件加载内容并显示到[文件浏览]中。
 * 加载前会对该文件的路径进行检测，如果不存在该文件，会直接移除该文件。
 * 如果是文本类广告，只会显示它的第一行。
 * 
*/
void MainWindow::fileBrowse()
{
	fileBrowseTreeWidget->clear();
	
	if(fileListTreeWidget->currentIndex().row() < 0)
		return;
	
	if(!QFileInfo(m_fileList[fileListTreeWidget->currentIndex().row()]->filePath()).exists())
	{
		QMessageBox::information(this,
								 tr("广告服务器"),
								 tr("文件不存在"));
		m_fileList.removeAt(fileListTreeWidget->currentIndex().row());
		refreshList();
		return;
	}
	
	QList<Scene *> sc = m_fileList[fileListTreeWidget->currentIndex().row()]->m_fileParse.scene();
	
	foreach (Scene * s, sc)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(fileBrowseTreeWidget->invisibleRootItem());
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
	}
	
	fileBrowseTreeWidget->expandAll();
	
}

/*
 * 私有槽
 * 参数1：[选项卡部件]当前索引值
 * 如果索引值不为1（不在编辑器栏），当[文件列表]项目改变时断开相应的信号和槽，否则连接相应的信号和槽。
 * 
*/
void MainWindow::conBrowseSignal(int index)
{
	if(index == 0)
		connect(fileListTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
				this, SLOT(fileBrowse()));
	else
		disconnect(fileListTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
				   this, SLOT(fileBrowse()));
	
}

/*
 * 私有槽
 * 参数1：[bo]为true时连接数据库，[false]时断开数据库。
 * 
*/
void MainWindow::connectOrDisconnectDatabase(bool bo)
{
	QString errString;
	QPalette pa;
	
	if(bo)
	{
		errString = DatabaseBrowser::createConnection();
		if(!errString.isEmpty())
		{
			QMessageBox::information(this,
									 tr("广告服务器"),
									 tr("连接数据库失败。\n原因：%1")
									 .arg(errString));
			
			connectOrDisconnectDatabaseButton->setChecked(false);
			return;
		}
		pa.setColor(QPalette::WindowText, Qt::blue);
		databaseState->setPalette(pa);
		databaseState->setText(tr("已连接"));
		connectOrDisconnectDatabaseButton->setText(tr("关闭数据库(&B)"));
		connectOrDisconnectDatabaseButton->setToolTip(tr("关闭数据库连接。"));
		connectOrDisconnectDatabaseButton->setStatusTip(tr("关闭数据库连接。"));
		
		editDatabaseButton->setEnabled(true);
	}
	else
	{
		DatabaseBrowser::closeConnection();
		
		pa.setColor(QPalette::WindowText, Qt::red);
		databaseState->setPalette(pa);
		databaseState->setText(tr("未连接"));
		connectOrDisconnectDatabaseButton->setChecked(false);
		connectOrDisconnectDatabaseButton->setText(tr("连接数据库(&B)"));
		connectOrDisconnectDatabaseButton->setToolTip(tr("连接数据库。"));
		connectOrDisconnectDatabaseButton->setStatusTip(tr("连接数据库。"));
		
		editDatabaseButton->setEnabled(false);
	}
	
}

/*
 *  函数名：editDatabase
 *  函数类型：私有槽
 *  时间：2016/9/28 10:10:20
 *  功能：编辑广告数据库。
 *  
 */
void MainWindow::editDatabase()
{
	DatabaseBrowser browser(this);
	
	browser.setFont(font());
	browser.setLanguage(selectLanguageComboBox->currentIndex() == 0);
	
	browser.exec();
	
}

/*
 * 私有槽
 * 参数1：消息
 * 添加一个消息到日志中。
 * 
*/
void MainWindow::addMessage(QString msg)
{
	QTextCursor cursor = logTextEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	
	cursor.insertText(QString("[%1]%2\n")
					  .arg(QDateTime::currentDateTime().toString("yyyy/M/d hh:mm:ss"))
					  .arg(msg));
	
	cursor.movePosition(QTextCursor::End);
	logTextEdit->setTextCursor(cursor);
	
}

/*
 * 私有槽
 * 保存日志。
 * 
*/
void MainWindow::saveLog()
{
	QString fileName = QFileDialog::getSaveFileName(this,
													tr("保存日志文件"),
													".",
													tr("文件文件 (*.txt);;所有文件 (*.*)"));
	if(fileName.isEmpty())
		return;
	
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::information(this,
								 tr("广告服务器"),
								 QString(tr("保存日志文件失败\n原因："))
									+ file.errorString());
		return;
	}
	
	QTextStream out(&file);
	out << logTextEdit->toPlainText();
	file.close();
	QMessageBox::information(this,
							 tr("广告服务器"),
							 tr("日志文件保存成功"));
	
}

/*
 * 私有槽
 * 参数1：[bo]为true时启动服务器，[bo]为false时关闭服务器。
 * 
*/
void MainWindow::startOrStopServer(bool bo)
{
	if(bo)
	{
		if(m_server == NULL)
		{
			m_server = new TCPServer(m_fileList);
		}
		
		connect(m_server, SIGNAL(haveMessage(QString)),
				this, SLOT(addMessage(QString)));
		connect(m_server, SIGNAL(clientCount(int)),
				this, SLOT(setClientCount(int)));
		
		if(!m_server->listen(QHostAddress::Any, 5300))
		{
			QMessageBox::information(this,
									 tr("广告服务器"),
									 tr("启动服务器失败\n原因：") + m_server->errorString());
			return;
		}
		
		serverButton->setText(tr("关闭服务(&S)"));
		serverButton->setToolTip(tr("关闭服务，停止接收客户端的请求。"));
		serverButton->setStatusTip(tr("关闭服务，停止接收客户端的请求。"));
		
		addMessage(tr("服务器启动成功。"));
	}
	else
	{
		m_server->closeClient();
		m_server->close();
		
		disconnect(m_server, SIGNAL(haveMessage(QString)),
				   this, SLOT(addMessage(QString)));
		disconnect(m_server, SIGNAL(clientCount(int)),
				   this, SLOT(setClientCount(int)));
		
		serverButton->setText(tr("启动服务(&S)"));
		serverButton->setToolTip(tr("启动服务，接收来自客户端的请求。"));
		serverButton->setStatusTip(tr("启动服务，接收来自客户端的请求。"));
		
		addMessage(tr("服务器已关闭。"));
		
		setClientCount(0);
	}
	
}

/*
 * 私有槽
 * 参数1：[选项卡部件]当前所在的选项卡索引号
 * 当[选项卡部件]转到[服务器栏]时修正[启动或关闭服务器按钮]文字说明是否有错误。
 * 
*/
void MainWindow::repairServerButton(int index)
{
	if(index != 1)
		return;
	
	if(serverButton->isChecked())
	{
		serverButton->setText(tr("关闭服务(&S)"));
		serverButton->setToolTip(tr("关闭服务，停止接收客户端的请求。"));
		serverButton->setStatusTip(tr("关闭服务，停止接收客户端的请求。"));
	}
	else
	{
		serverButton->setText(tr("启动服务(&S)"));
		serverButton->setToolTip(tr("启动服务，接收来自客户端的请求。"));
		serverButton->setStatusTip(tr("启动服务，接收来自客户端的请求。"));
	}
	
}

/*
 * 私有槽
 * 参数1：当前客户端数量
 * 更新[客户端连接数]标签。
 * 
*/
void MainWindow::setClientCount(int count)
{
	curClientCount->setText(QString::number(count));
	
}

/*
 * 私有槽
 * 切换程序语言。不同的选择载入不同翻译文件。
 * 
*/
void MainWindow::setLanguage()
{
	int index = selectLanguageComboBox->currentIndex();
	if(index == 0)
	{
		qApp->installTranslator(&m_appTranslatorCN);
		qApp->removeTranslator(&m_appTranslatorEN);
	}
	else if(index == 1)
	{
		qApp->installTranslator(&m_appTranslatorEN);
		qApp->removeTranslator(&m_appTranslatorCN);
	}
	
}

/*
 * 私有槽
 * 设置程序字体。
 * 
*/
void MainWindow::setProgramFont()
{
	bool ok;
	QFont newFont = QFontDialog::getFont(&ok, font(), this);
	if(ok)
	{
		setFont(newFont);
		statusWindowBar->setFont(newFont);
	}
	
}

/*
 * 私有槽
 * 重置程序设置。
 * 
*/
void MainWindow::resetSetting()
{
	int index = selectLanguageComboBox->currentIndex();
	selectLanguageComboBox->setCurrentIndex(0);
	if(index == 1)
	{
		qApp->installTranslator(&m_appTranslatorCN);
		qApp->removeTranslator(&m_appTranslatorEN);
	}
	
	QFont font = QApplication::font();
	font.setPointSize(10);
	setFont(font);
	statusWindowBar->setFont(QApplication::font("QStatusBar"));
	if(!saveFileListCheckBox->isChecked())
		saveFileListCheckBox->setChecked(true);
	if(!saveLogCheckBox->isChecked())
		saveLogCheckBox->setChecked(true);
	if(!saveGeneralSettingsCheckBox->isChecked())
		saveGeneralSettingsCheckBox->setChecked(true);
	setWindowState(Qt::WindowMaximized);
	
}

/*
 * 私有函数
 * 初始化上下文菜单的选项。
 * 
*/
void MainWindow::createAction()
{
	m_fileListMenu = new QMenu(this);
	
	m_newFileAction = new QAction(tr("新建文件(&N)"), this);
	m_newFileAction->setStatusTip(tr("创建一个新的文件。"));
	
	m_addFileAction = new QAction(tr("添加文件(&A)"), this);
	m_addFileAction->setStatusTip(tr("添加adm文件到文件列表中。"));
	
	m_removeFileAction = new QAction(tr("移除文件(&R)"), this);
	m_removeFileAction->setStatusTip(tr("将文件列表中选中的文件移除。"));
	
	m_clearListAction = new QAction(tr("清空列表(&C)"), this);
	m_clearListAction->setStatusTip(tr("清空文件列表。"));
	
	m_setFileSendIDAction = new QAction(tr("设置ID(&S)"), this);
	m_setFileSendIDAction->setStatusTip(tr("设置选中的文件发送ID，用于将该文件发送到对应ID的客户端。"));
	
	connect(m_newFileAction, SIGNAL(triggered(bool)),
			this, SLOT(newFile()));
	connect(m_addFileAction, SIGNAL(triggered(bool)),
			this, SLOT(addFile()));
	connect(m_removeFileAction, SIGNAL(triggered(bool)),
			this, SLOT(removeFile()));
	connect(m_clearListAction, SIGNAL(triggered(bool)),
			this, SLOT(clearList()));
	connect(m_setFileSendIDAction, SIGNAL(triggered(bool)),
			this, SLOT(setFileSendID()));
	
}

/*
 * 私有函数
 * 初始化信号与槽的连接。
 * 
*/
void MainWindow::initConnect()
{
	connect(tabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(conBrowseSignal(int)));
	
	connect(newFileButton, SIGNAL(clicked(bool)),
			this, SLOT(newFile()));
	connect(addFileButton, SIGNAL(clicked(bool)),
			this, SLOT(addFile()));
	connect(removeFileButton, SIGNAL(clicked(bool)),
			this, SLOT(removeFile()));
	connect(clearListButton, SIGNAL(clicked(bool)),
			this, SLOT(clearList()));
	connect(setIDButton, SIGNAL(clicked(bool)),
			this, SLOT(setFileSendID()));
	connect(editFileButton, SIGNAL(clicked(bool)),
			this, SLOT(editFile()));
	connect(fileListTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showFileListContextMenu(QPoint)));
	connect(fileListTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			this, SLOT(fileBrowse()));
	connect(connectOrDisconnectDatabaseButton, SIGNAL(toggled(bool)),
			this, SLOT(connectOrDisconnectDatabase(bool)));
	connect(editDatabaseButton, SIGNAL(clicked(bool)),
			this, SLOT(editDatabase()));
	
	connect(saveLogButton, SIGNAL(clicked(bool)),
			this, SLOT(saveLog()));
	connect(clearLogButton, SIGNAL(clicked(bool)),
			logTextEdit, SLOT(clear()));
	connect(serverButton, SIGNAL(toggled(bool)),
			this, SLOT(startOrStopServer(bool)));
	connect(tabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(repairServerButton(int)));
	
	connect(selectLanguageComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setLanguage()));
	connect(setFontButton, SIGNAL(clicked(bool)),
			this, SLOT(setProgramFont()));
	connect(resetButton, SIGNAL(clicked(bool)),
			this, SLOT(resetSetting()));
	
}

/*
 * 私有函数
 * 刷新[文件列表]。
 * 
*/
void MainWindow::refreshList()
{
	fileListTreeWidget->clear();
	
	foreach (File * file, m_fileList) 
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(fileListTreeWidget);
		item->setText(0, file->fileName().mid(0, file->fileName().indexOf('.')));
		item->setText(1, file->sendID().isEmpty() ? tr("无") : file->sendID());
		item->setText(2, file->filePath());
	}
	
}

/*
 * 私有函数
 * 读取程序设置。
 * 
*/
void MainWindow::readSettings()
{
	QSettings settings("Software", "AD Server");
	
	settings.beginGroup("MainWindow");
	
	saveFileListCheckBox->setChecked(settings.value("autoSaveFileList", true).toBool());
	saveLogCheckBox->setChecked(settings.value("autoSaveLog", true).toBool());
	saveGeneralSettingsCheckBox->setChecked(settings.value("autoSaveGeneralSettings", true).toBool());
	
	if(saveGeneralSettingsCheckBox->isChecked())
	{
		restoreGeometry(settings.value("geometry").toByteArray());
		
		int index = settings.value("language", 0).toInt();
		selectLanguageComboBox->setCurrentIndex(index);
		if(index == 0)
			qApp->installTranslator(&m_appTranslatorCN);
		else if(index == 1)
			qApp->installTranslator(&m_appTranslatorEN);
		
		setFont(settings.value("font").value<QFont>());
		statusWindowBar->setFont(settings.value("statusFont").value<QFont>());
		
	}
	
	if(saveFileListCheckBox->isChecked())
	{
		QVariantMap fileMap = settings.value("fileList").toMap();
		
		for(QVariantMap::iterator it = fileMap.begin();
				it != fileMap.end(); it++)
		{
			if(!QFileInfo(it.key()).exists())
				continue;
			
			File * file = new File(it.key());
			file->setSendID(it.value().toString());
			m_fileList.append(file);
		}
		refreshList();
		
	}
	
	if(saveLogCheckBox->isChecked())
	{
		logTextEdit->setPlainText(settings.value("serverLog").toString());
		
		QTextCursor cursor = logTextEdit->textCursor();
		cursor.movePosition(QTextCursor::End);
		logTextEdit->setTextCursor(cursor);
	}
	
	settings.endGroup();
	
}

/*
 * 私有函数
 * 保存程序设置。
 * 
*/
void MainWindow::saveSettings()
{
	QSettings settings("Software", "AD Server");
	
	settings.beginGroup("MainWindow");
	
	settings.setValue("autoSaveFileList", saveFileListCheckBox->isChecked());
	settings.setValue("autoSaveLog", saveLogCheckBox->isChecked());
	settings.setValue("autoSaveGeneralSettings", saveGeneralSettingsCheckBox->isChecked());
	
	if(saveGeneralSettingsCheckBox->isChecked())
	{
		settings.setValue("geometry", saveGeometry());
		
		settings.setValue("language", selectLanguageComboBox->currentIndex());
		settings.setValue("font", QVariant::fromValue(font()));
		settings.setValue("statusFont", QVariant::fromValue(statusWindowBar->font()));
		
	}
	else
	{
		settings.remove("geometry");
		
		settings.remove("language");
		settings.remove("font");
	}
	
	if(saveFileListCheckBox->isChecked())
	{
		QVariantMap fileMap;
		foreach(File * file, m_fileList)
		{
			if(!QFileInfo(file->filePath()).exists())
				continue;
			
			fileMap.insert(file->filePath(), QVariant(file->sendID()));
		}

		settings.setValue("fileList", fileMap);
	}
	else
	{
		settings.remove("fileList");
	}
	
	if(saveLogCheckBox->isChecked())
		settings.setValue("serverLog", logTextEdit->toPlainText());
	else
		settings.remove("serverLog");
	
	settings.endGroup();
	
}
