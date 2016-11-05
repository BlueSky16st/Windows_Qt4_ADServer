#ifndef TIMEDELEGATE_H
#define TIMEDELEGATE_H

#include <QItemDelegate>

/*
 *  类名：TimeDelegate
 *  时间：2016/9/29 19:52:09
 *  功能：QTimeEdit委托，对数据库中的时间单元格进行格式化
 */
class TimeDelegate : public QItemDelegate
{
public:
	TimeDelegate(QObject * parent = 0);
	
	QWidget * createEditor(QWidget * parent, 
						   const QStyleOptionViewItem & option, 
						   const QModelIndex & index) const;
	
	void setEditorData(QWidget * editor, 
					   const QModelIndex & index) const;
	
	void setModelData(QWidget * editor, 
					  QAbstractItemModel * model, 
					  const QModelIndex & index) const;
	
};

/*
 *  类名：ADTypeDelegate
 *  时间：2016/10/01 15:44:35
 *  功能：QComboBox委托，对数据库中的节目类型单元格进行格式化
 */
class ADTypeDelegate : public QItemDelegate
{
public:
	ADTypeDelegate(QObject * parent = 0);
	
	QWidget * createEditor(QWidget * parent, 
						   const QStyleOptionViewItem & option, 
						   const QModelIndex & index) const;
	
	void setEditorData(QWidget * editor, 
					   const QModelIndex & index) const;
	
	void setModelData(QWidget * editor, 
					  QAbstractItemModel * model, 
					  const QModelIndex & index) const;
	
};

#endif // TIMEDELEGATE_H
