#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QList>
#include <QTime>

QTime stringToTime(const QString & time);
QString timeToString(const QTime & time);

enum NodeType
{
	Type_text,
	Type_image,
	Type_video
};

class Node
{
public:
	Node(NodeType type = Type_text);
	
	void setType(const NodeType & newType);
	NodeType type() const;
	
private:
	NodeType m_type;
	
};

class MediaNode : public Node
{
public:
	MediaNode(NodeType type, const QString & file);
	
	void setFile(const QString & newFile);
	QString file() const;
	
private:
	QString m_file;
	
};

class TextNode : public Node
{
public:
	TextNode(const QString & text);
	
	void setText(const QString & newText);
	QString text() const;
	
private:
	QString m_text;
	
};


class Scene
{
public:
	Scene(const QString & name,
		  const QTime & begin = QTime(0, 0),
		  const QTime & end = QTime(0, 0),
		  const quint16 & interval = 0);
	~Scene();
	
	void setName(const QString & newName);
	QString name() const;
	
	void setBegin(const QTime & newBegin);
	QTime begin() const;
	
	void setEnd(const QTime & newEnd);
	QTime end() const;
	
	void setInterval(const quint16 & newInterval);
	quint16 interval() const;
	
	void addNode(Node * newNode);
	void removeNode(int index);
	Node * node(int index);
	QList<Node *> nodeList();
	
private:
	QString m_name;
	QTime m_begin;
	QTime m_end;
	quint16 m_interval;
	
	QList<Node *> m_nodeList;
	
};

#endif // NODE_H
