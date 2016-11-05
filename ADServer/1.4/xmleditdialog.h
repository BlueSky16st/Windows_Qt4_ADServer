#ifndef XMLEDITDIALOG_H
#define XMLEDITDIALOG_H

#include <QDialog>
#include <QTranslator>
#include "ui_xmleditdialog.h"

class File;

class XMLEditDialog : public QDialog, private Ui::XMLEditDialog
{
	Q_OBJECT
	
public:
	XMLEditDialog(File * file, QFont font, bool isCN = true, QWidget * parent = 0);
	
	void setLanguage(bool cn);
	
protected:
	void closeEvent(QCloseEvent * event);
	
signals:
	void fileChange();
	
private slots:
	void showInfo(QTreeWidgetItem * item);
	void createNewAD();
	void showContextMenu(QPoint pos);
	void changeType(int index);
	void saveFile();
	void setData();
	void browseFile();
	void newScene();
	void deleteScene();
	void deleteNode();
	
protected:
	void changeEvent(QEvent * event);
	
private:
	void refreshList();
	
	void createAction();
	
	File * m_file;
	
	QMenu * m_sceneMenu;
	
	QAction * m_newSceneAction;
	QAction * m_deleteSceneAction;
	QAction * m_deleteNodeAction;
	
	QTranslator m_appTranslatorCN;
	QTranslator m_appTranslatorEN;
	
	bool m_somethingChanged;
	
};

#endif // XMLEDITDIALOG_H
