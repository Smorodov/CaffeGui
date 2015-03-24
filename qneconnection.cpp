#include "qneconnection.h"
#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QPen>
#include <QGraphicsScene>

QNEConnection::QNEConnection(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
	setFlag(QGraphicsItem::ItemIsSelectable);
	setPen(QPen(Qt::black, 2));
	setBrush(Qt::NoBrush);
	setZValue(-1);
	m_port1 = 0;
	m_port2 = 0;
}

QNEConnection::~QNEConnection()
{
	if (m_port1)
		m_port1->connections().remove(m_port1->connections().indexOf(this));
	if (m_port2)
		m_port2->connections().remove(m_port2->connections().indexOf(this));
}

void QNEConnection::setPos1(const QPointF &p)
{
	pos1 = p;
}

void QNEConnection::setPos2(const QPointF &p)
{
	pos2 = p;
}

void QNEConnection::setPort1(QNEPort *p)
{
	m_port1 = p;

	m_port1->connections().append(this);
}

void QNEConnection::setPort2(QNEPort *p)
{
	m_port2 = p;

	m_port2->connections().append(this);
}

void QNEConnection::updatePosFromPorts()
{
	pos1 = m_port1->scenePos();
	pos2 = m_port2->scenePos();
}

void QNEConnection::updatePath()
{
	QPainterPath p;


	p.moveTo(pos1);

	qreal dx = pos2.x() - pos1.x();
	qreal dy = pos2.y() - pos1.y();


	QPointF ctr1(pos1.x() + dx * 0.7, pos1.y() + dy * 0.0);
	QPointF ctr2(pos1.x() + dx * 0.3, pos1.y() + dy * 1);

	p.cubicTo(ctr1, ctr2, pos2);


	setPath(p);
}

#include <QStyleOptionGraphicsItem>

void QNEConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
		Q_UNUSED(widget)
		if(isSelected())
		{
			painter->setPen(QPen(Qt::red, 2));
			painter->setBrush(Qt::NoBrush);
		}else
		{
			painter->setPen(QPen(Qt::blue, 2));
			painter->setBrush(Qt::NoBrush);	
		}

		painter->drawPath(path());
}

QNEPort* QNEConnection::port1() const
{
	return m_port1;
}

QNEPort* QNEConnection::port2() const
{
	return m_port2;
}

void QNEConnection::save(QDataStream &ds)
{
	ds << (quint64) m_port1;
	ds << (quint64) m_port2;
}

void QNEConnection::saveToXml(QString name, QDomDocument& doc, QDomElement& root)
{
	QDomElement connNode = doc.createElement("Connection");

	QDomElement nameNode = doc.createElement("Name");
		QDomText nameNodeText = doc.createTextNode(name);
		nameNode.appendChild(nameNodeText);
	
	connNode.appendChild(nameNode);

	QNEBlock* b1=m_port1->block();
	QNEBlock* b2=m_port2->block();

	QDomElement sourceNode = doc.createElement("Source");
	QDomElement destinationNode = doc.createElement("Destination");

	QDomElement sourceBlockNameNode = doc.createElement("Block");
	QDomElement sourcePortNameNode = doc.createElement("Port");
	
		QDomText sourceBlockNameNodeText = doc.createTextNode( b1->block_name );
		sourceBlockNameNode.appendChild(sourceBlockNameNodeText);

		QDomText sourcePortNameNodeText = doc.createTextNode(m_port1->name);
		sourcePortNameNode.appendChild(sourcePortNameNodeText);

	sourceNode.appendChild(sourceBlockNameNode);
	sourceNode.appendChild(sourcePortNameNode);

	QDomElement destBlockNameNode = doc.createElement("Block");
	QDomElement destPortNameNode = doc.createElement("Port");

		QDomText destinationBlockNameNodeText = doc.createTextNode( b2->block_name );
		destBlockNameNode.appendChild(destinationBlockNameNodeText);

		QDomText destinationPortNameNodeText = doc.createTextNode(m_port2->name);
		destPortNameNode.appendChild(destinationPortNameNodeText);


	destinationNode.appendChild(destBlockNameNode);
	destinationNode.appendChild(destPortNameNode);

	connNode.appendChild(sourceNode);
	connNode.appendChild(destinationNode);
	root.appendChild(connNode);
}

void QNEConnection::load(QDataStream &ds, const QMap<quint64, QNEPort*> &portMap)
{
	quint64 ptr1;
	quint64 ptr2;
	ds >> ptr1;
	ds >> ptr2;

	setPort1(portMap[ptr1]);
	setPort2(portMap[ptr2]);
	updatePosFromPorts();
	updatePath();
}
