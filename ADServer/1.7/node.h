#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QList>
#include <QTime>

QTime stringToTime(const QString & time);			// 将字符串转换成时间类型
QString timeToString(const QTime & time);			// 将时间类型转换成字符串

enum NodeType			// 广告类型
{
	Type_text,
	Type_image,
	Type_video
};

/*
 * 广告类
 * 基类
 * 提供基本广告类型。
 * 
*/
class Node
{
public:
	Node(NodeType type = Type_text);				// 构造函数
	
	void setType(const NodeType & newType);			// 设置广告类型
	NodeType type() const;							// 返回广告类型
	
private:
	NodeType m_type;								// 广告类型
	
};

/*
 * 多媒体广告类
 * 继承自：Node
 * 提供文件路径和广告链接。
 * 
*/
class MediaNode : public Node
{
public:
	MediaNode(NodeType type, const QString & file, const QString & url);	// 构造函数
	
	void setFile(const QString & newFile);			// 设置文件路径
	QString file() const;							// 返回文件路径
	
	void setUrl(const QString & newUrl);			// 设置链接
	QString url() const;							// 返回链接
	
private:
	QString m_file;									// 文件路径
	QString m_url;									// 广告链接
	
};

/*
 * 文本广告类
 * 继承自：Node
 * 提供文本广告。
 * 
*/
class TextNode : public Node
{
public:
	TextNode(const QString & text);					// 构造函数
	
	void setText(const QString & newText);			// 设置广告文本
	QString text() const;							// 返回广告文本
	
private:
	QString m_text;									// 广告文本
	
};

/*
 * 广告节目单类
 * 保存广告节目单的属性和拥有的广告。
 * 
*/
class Scene
{
public:
	Scene(const QString & name,
		  const QTime & begin = QTime(0, 0),
		  const QTime & end = QTime(0, 0),
		  const quint16 & interval = 0);			// 构造函数
	~Scene();										// 析构函数
	
	void setName(const QString & newName);			// 设置广告节目单名称
	QString name() const;							// 返回广告节目单名称
	
	void setBegin(const QTime & newBegin);			// 设置广告节目单开始播放时间
	QTime begin() const;							// 返回广告节目单开始播放时间
	
	void setEnd(const QTime & newEnd);				// 设置广告节目单结束播放时间
	QTime end() const;								// 返回广告节目单结束播放时间
	
	void setInterval(const quint16 & newInterval);	// 设置广告节目单广告播放间隔时间
	quint16 interval() const;						// 返回广告节目单广告播放间隔时间
	
	void addNode(Node * newNode);					// 添加一个新的广告
	void modifyNodeType(int index, 
						NodeType newType, 
						QString str, 
						QString media_url = "");	// 修改指定广告的信息
	void removeNode(int index);						// 删除指定广告
	Node * node(int index);							// 返回指定广告的指针
	QList<Node *> nodeList();						// 返回广告列表
	
private:
	QString m_name;									// 广告节目单名称
	QTime m_begin;									// 广告节目单开始播放时间
	QTime m_end;									// 广告节目单结束播放时间
	quint16 m_interval;								// 广告节目单广告播放间隔时间
	
	QList<Node *> m_nodeList;						// 广告列表
	
};

#endif // NODE_H
