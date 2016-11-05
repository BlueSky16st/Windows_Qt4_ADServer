#include "timedelegate.h"

#include <QTimeEdit>

/*
 *  函数名：TimeDelegate
 *  函数类型：构造函数
 *  时间：2016/9/29 19:53:11
 *  功能：初始化构造函数。
 *  
 *  参数列表
 *	    参数1：父对象，默认为0。
 */
TimeDelegate::TimeDelegate(QObject * parent /* = 0 */) : QItemDelegate(parent)
{
	
}

/*
 *  函数名：createEditor
 *  函数类型：公有函数，重写函数
 *  时间：2016/9/29 19:55:43
 *  功能：创建部件。
 */
QWidget * TimeDelegate::createEditor(QWidget * parent, 
									 const QStyleOptionViewItem & , 
									 const QModelIndex & ) const
{
	QTimeEdit * editor = new QTimeEdit(parent);
	
	return editor;
	
}

/*
 *  函数名：setEditorData
 *  函数类型：公有函数，重写函数
 *  时间：2016/9/29 19:56:21
 *  功能：设置部件数据。
 */
void TimeDelegate::setEditorData(QWidget * editor, 
								 const QModelIndex & index) const
{
	QStringList text = index.model()->data(index, Qt::EditRole).toString().split(':');
	QTimeEdit * timeEditor = static_cast<QTimeEdit *>(editor);
	
	while(text.size() < 3)
	{
		text.append("0");
	}
	
	timeEditor->setTime(QTime(text.at(0).toInt(), text.at(1).toInt(), text.at(2).toInt()));
	
}

/*
 *  函数名：setModelData
 *  函数类型：公有函数，重写函数
 *  时间：2016/9/29 19:56:42
 *  功能：设置模型数据。
 */
void TimeDelegate::setModelData(QWidget * editor, 
								QAbstractItemModel * model, 
								const QModelIndex & index) const
{
	QTimeEdit * timeEditor = static_cast<QTimeEdit *>(editor);
	QString text = timeEditor->text();
	model->setData(index, text, Qt::EditRole);
	
}
