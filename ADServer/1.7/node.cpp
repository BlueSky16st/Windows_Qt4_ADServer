#include "node.h"

#include <QString>
#include <QStringList>
#include <QDir>

/*
 * node.h全局函数
 * 参数1：QString类型的时间。
 * 返回值：返回QTime类型的时间。
 * 将字符串类型转换成时间类型。
 * 
*/
QTime stringToTime(const QString & time)
{	
	QStringList str = time.split(':');
	return QTime(str[0].toInt(), str[1].toInt());
	
}

/*
 * node.h全局函数
 * 参数1：QTime类型的时间。
 * 返回值：返回QSTring类型的时间。
 * 将时间类型转换成字符串类型。
 * 
*/
QString timeToString(const QTime & time)
{
	return time.toString("hh:mm:ss");
	
}

/*
 * 构造函数
 * 参数1：广告类型，默认为Type_text。
 * 
*/
Node::Node(NodeType type /* = Type_text */)
{
	m_type = type;
	
}

/*
 * 公有函数
 * 参数1：新的广告类型。
 * 设置新的广告类型。
 * 
*/
void Node::setType(const NodeType & newType)
{
	m_type = newType;
	
}

/*
 * 公有函数
 * 返回值：返回广告类型。
 * 
*/
NodeType Node::type() const
{
	return m_type;
	
}

/*
 * 构造函数
 * 参数1：广告类型。
 * 参数2：广告文件路径。
 * 参数3：广告链接。
 * 
*/
MediaNode::MediaNode(NodeType type, const QString & file, const QString & url) : Node(type)
{
	m_file = QDir::toNativeSeparators(file);
	m_url = url;
	
}

/*
 * 公有函数
 * 参数1：新的文件路径。
 * 设置新的广告文件路径。
 * 
*/
void MediaNode::setFile(const QString & newFile)
{
	m_file = QDir::toNativeSeparators(newFile);
	
}

/*
 * 公有函数
 * 返回值：返回广告文件路径。
 * 
*/
QString MediaNode::file() const
{
	return m_file;
	
}

/*
 * 公有函数
 * 参数1：新的链接。
 * 设置新的广告链接。
 * 
*/
void MediaNode::setUrl(const QString & newUrl)
{
	m_url = newUrl;
	
}

/*
 * 公有函数
 * 返回值：返回广告链接。
 * 
*/
QString MediaNode::url() const
{
	return m_url;
	
}

/*
 * 构造函数
 * 参数1：广告文本。
 * 
*/
TextNode::TextNode(const QString & text) : Node(Type_text)
{
	m_text = text;
	
}

/*
 * 公有函数
 * 参数1：新的广告文本。
 * 设置新的广告文本。
 * 
*/
void TextNode::setText(const QString & newText)
{
	m_text = newText;
	
}

/*
 * 公有函数
 * 返回值：返回广告文本。
 * 
*/
QString TextNode::text() const
{
	return m_text;
	
}

/*
 * 构造函数。
 * 参数1：广告节目单名称
 * 参数2：广告节目单开始播放时间，默认值为0时0分。
 * 参数3：广告节目单结束播放时间，默认值为0时0分。
 * 参数4：广告节目单广告播放间隔时间。
 * 
*/
Scene::Scene(const QString & name,
			 const QTime & begin /* = QTime(0, 0) */,
			 const QTime & end /* = QTime(0, 0) */,
			 const quint16 & interval /* = 0 */)
{
	m_name = name;
	m_begin = begin;
	m_end = end;
	m_interval = interval;
	
}

/*
 * 析构函数。
 * 删除所有广告。
 * 
*/
Scene::~Scene()
{
	qDeleteAll(m_nodeList);
	m_nodeList.clear();
	
}

/*
 * 公有函数
 * 参数1：新的广告节目单名称。
 * 设置新的广告节目单名称。
 * 
*/
void Scene::setName(const QString & newName)
{
	m_name = newName;
	
}

/*
 * 公有函数
 * 返回值：返回广告节目单名称。
 * 
*/
QString Scene::name() const
{
	return m_name;
	
}

/*
 * 公有函数
 * 参数1：新的广告节目单开始播放时间。
 * 设置新的广告节目单开始播放时间。
 * 
*/
void Scene::setBegin(const QTime & newBegin)
{
	m_begin = newBegin;
	
}

/*
 * 公有函数
 * 返回值：返回广告节目单开始播放时间。
 * 
*/
QTime Scene::begin() const
{
	return m_begin;
	
}

/*
 * 公有函数
 * 参数1：新的广告节目单结束播放时间。
 * 设置新的广告节目单结束播放时间。
 * 
*/
void Scene::setEnd(const QTime & newEnd)
{
	m_end = newEnd;
	
}

/*
 * 公有函数
 * 返回值：返回广告节目单结束播放时间。
 * 
*/
QTime Scene::end() const
{
	return m_end;
	
}

/*
 * 公有函数
 * 参数1：新的广告节目单广告播放间隔时间。
 * 设置新的广告节目单广告播放间隔时间。
 * 
*/
void Scene::setInterval(const quint16 & newInterval)
{
	m_interval = newInterval;
	
}

/*
 * 公有函数
 * 返回值：返回广告节目单广告播放间隔时间。
 * 
*/
quint16 Scene::interval() const
{
	return m_interval;
	
}

/*
 * 公有函数
 * 参数1：新的广告。
 * 添加新的广告到广告列表中。
 * 
*/
void Scene::addNode(Node * newNode)
{
	m_nodeList.append(newNode);
	
}

/*
 * 公有函数
 * 参数1：广告索引。
 * 参数2：新的广告类型。
 * 参数3：如果类型为text，该参数为文本；如果类型为media，该参数为文件路径。
 * 参数4：如果类型为text，该参数无效；如果类型为media，该参数为广告链接。
 * 修改指定索引值的广告内容。
 * 
*/
void Scene::modifyNodeType(int index, 
						   NodeType newType, 
						   QString str, 
						   QString media_url /* = "" */)
{
	Node * oldNode = m_nodeList.at(index);
	
	Node * newNode = NULL;
	
	switch (newType)
	{
	case Type_text:
		newNode = new TextNode(str);
		
		break;
		
	case Type_image:
	case Type_video:
		newNode = new MediaNode(newType, QDir::toNativeSeparators(str), media_url);
		
		break;
		
	}
	 m_nodeList[index] = newNode;
	
	delete oldNode;
}

/*
 * 公有函数
 * 参数1：广告索引。
 * 删除指定索引值的广告。
 * 
*/
void Scene::removeNode(int index)
{
	m_nodeList.removeAt(index);
	
}

/*
 * 公有函数
 * 参数1：广告索引。
 * 返回值：广告的指针。
 * 返回指定索引值的广告的指针。
 * 
*/
Node * Scene::node(int index)
{
	return m_nodeList.at(index);
	
}

/*
 * 公有函数
 * 返回值：广告列表。
 * 返回广告列表。
 * 
*/
QList<Node *> Scene::nodeList()
{
	return m_nodeList;
	
}
