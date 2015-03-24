#include "qneblock.h"

#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <QtXml/QDomDocument>
#include "qneport.h"

QNEBlock::QNEBlock(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);

	horzMargin = 100;
	vertMargin = 20;
	width = horzMargin;
	height = vertMargin;

	label_name = new QGraphicsTextItem(this);
	label_name->setDefaultTextColor(QColor(255,255,255));
	label_type = new QGraphicsTextItem(this);

	label_name->setPlainText("name");
	label_type->setPlainText("type");
	numInputs=0;
	numOutputs=0;
}

void QNEBlock::refresh(void)
{
	QColor port_labels_color(0,0,0);
	QFontMetrics fm(scene()->font());
	int h = fm.height();

	QPixmap pix = QPixmap(width+1, height+vertMargin*2+1); 
	pix.fill(QColor(0,0,0,0));
	QPainter paint(&pix);

	paint.setPen(Qt::black);


	QLinearGradient gradient(0,0,width, height+vertMargin*2);
	gradient.setColorAt(0.0f, QColor(60,60,60,255));
	gradient.setColorAt(0.3f, QColor(30,30,30,255));
	gradient.setColorAt(0.31f, QColor(20,20,20,255));
	gradient.setColorAt(1.0f, QColor(20,20,20,255));
	port_labels_color=Qt::green;
	paint.setBrush(gradient);
	
	
	paint.drawRoundedRect(0,0, width, height+vertMargin*2,5,5);


	if (isSelected())
	{	
	//paint.setBrush(QColor(255,227,160));

	QLinearGradient gradient(0,0,width, vertMargin*2);
	gradient.setColorAt(0.0f, QColor(250,250,60,255));
	gradient.setColorAt(0.4f, QColor(230,230,30,255));
	gradient.setColorAt(0.41f, QColor(220,220,20,255));
	gradient.setColorAt(1.0f, QColor(220,220,20,255));
	port_labels_color=Qt::green;
	paint.setBrush(gradient);


	}else
	{

	QLinearGradient gradient(0,0,width, vertMargin*2);
	gradient.setColorAt(0.0f, QColor(160,160,60,255));
	gradient.setColorAt(0.4f, QColor(130,130,30,255));
	gradient.setColorAt(0.41f, QColor(120,120,20,255));
	gradient.setColorAt(1.0f, QColor(120,120,20,255));
	port_labels_color=Qt::green;
	paint.setBrush(gradient);

	//paint.setBrush(QColor(168,223,244));
	}

    QPainterPath path;
    path.setFillRule( Qt::WindingFill );
    path.addRoundedRect( 0,0, width, vertMargin,5,5);
    path.addRect( QRect( 0, vertMargin-5, width, 5 ) ); // Top right corner not rounded
    paint.drawPath( path.simplified() ); // Only Top left & bottom right corner rounded

	((QGraphicsPixmapItem*)this)->setPixmap(pix);


	label_type->setPlainText(block_type);
	label_name->setPlainText(block_name);

	float w_1 = label_name->boundingRect().size().width()*label_name->scale();
	float h_1 = label_name->boundingRect().size().height()*label_name->scale();

	float w_2 = label_type->boundingRect().size().width()*label_type->scale();
	float h_2 = label_type->boundingRect().size().height()*label_type->scale();

	label_name->setPos((width-w_1), -h_1);
	label_type->setPos((width-w_2)/2, 0);



	foreach(QGraphicsItem *port_, childItems()) 
	{
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;
			port->label->setDefaultTextColor(port_labels_color);
	}


}

QNEPort* QNEBlock::addPort(const QString &name, bool isOutput, int flags, int ptr)
{
	QNEPort *port = new QNEPort(this);
	port->setName(name);
	port->setIsOutput(isOutput);
	port->setNEBlock(this);
	port->setPortFlags(flags);
	port->setPtr(ptr);

	QFontMetrics fm(scene()->font());
	int w = fm.width(name);
	int h = fm.height();

	if(isOutput)
	{
		numOutputs++;
	}else
	{
		numInputs++;
	}

	int PortLines=std::max(numOutputs,numInputs);

	if (w > width - horzMargin)
		width = w + horzMargin;
	
	height=vertMargin+PortLines*h;
	//height += h;

	refresh();

	int yi = vertMargin*2 + port->radius();
	int yo = vertMargin*2 + port->radius();

	foreach(QGraphicsItem *port_, childItems()) 
	{
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;
		if (port->isOutput())
		{
			port->setPos(width, yo);
			yo+=h;
		}
		else
		{
			port->setPos(0, yi);
			yi+=h;
		}
	}

	return port;
}

#include <QStyleOptionGraphicsItem>

void QNEBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
		Q_UNUSED(widget)
		refresh();
		painter->drawPixmap(0,0,pixmap().width(),pixmap().height(),pixmap());
}

void QNEBlock::addInputPort(const QString &name)
{
	addPort(name, false);
}

void QNEBlock::addOutputPort(const QString &name)
{
	addPort(name, true);
}

void QNEBlock::addInputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addInputPort(n);
}

void QNEBlock::addOutputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addOutputPort(n);
}


void QNEBlock::saveToXml(QString blockName,QDomDocument& doc,QDomElement& root)
{

	QDomElement block = doc.createElement("Block");

	QDomElement nameNode = doc.createElement("Name");

	QDomText nameNodeText = doc.createTextNode(block_name);
	nameNode.appendChild(nameNodeText);


	block.appendChild(nameNode);

	QDomElement functionNode = doc.createElement("Function");
	QDomElement functionName = doc.createElement("Name");
	QDomElement functionParameters = doc.createElement("Parameters");


	QDomText functionNameText = doc.createTextNode(block_type);
	functionName.appendChild(functionNameText);

	QDomText functionParametersText = doc.createTextNode(parameters_file_name);
	functionParameters.appendChild(functionParametersText);

	functionNode.appendChild(functionName);
	functionNode.appendChild(functionParameters);
	block.appendChild(functionNode);

	int count(0);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;
		count++;
	}
	
	QDomElement portsNode = doc.createElement("Ports");
	QDomElement numPortsNode = doc.createElement("Count");

	QDomText numPortsNodeText = doc.createTextNode(QString::number(count));
	numPortsNode.appendChild(numPortsNodeText);

    portsNode.appendChild(numPortsNode);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;

		QDomElement portNode = doc.createElement("Port");

		QDomElement portNameNode = doc.createElement("Name");
		QDomElement portTypeNode = doc.createElement("Type");
		QDomElement portFlagsNode = doc.createElement("Flags");
		QDomElement portDirectionNode = doc.createElement("Direction");

		QDomText portNameNodeText = doc.createTextNode(port->portName());
		portNameNode.appendChild(portNameNodeText);

		QDomText portTypeNodeText = doc.createTextNode(port->portType());
		portTypeNode.appendChild(portTypeNodeText);

		QDomText portFlagsNodeText = doc.createTextNode( QString::number(port->portFlags()) );
		portFlagsNode.appendChild(portFlagsNodeText);

		QDomText portDirectionNodeText = doc.createTextNode( port->isOutput()==true ? "out" : "in" );
		portDirectionNode.appendChild(portDirectionNodeText);

		portNode.appendChild(portNameNode);
		portNode.appendChild(portTypeNode);
		portNode.appendChild(portFlagsNode);
		portNode.appendChild(portDirectionNode);
		portsNode.appendChild(portNode);
	}

	block.appendChild(portsNode);
	root.appendChild(block);
}



void QNEBlock::save(QDataStream &ds)
{
	ds << block_name;
	ds << block_type;

	ds << pos();

	int count(0);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;
		count++;
	}

	ds << count;

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;
		ds << (quint64) port;
		ds << port->portName();
		ds << port->isOutput();
		ds << port->portFlags();
	}
}

void QNEBlock::load(QDataStream &ds, QMap<quint64, QNEPort*> &portMap)
{
	ds >> block_name;
	ds >> block_type;
	QPointF p;
	ds >> p;
	setPos(p);
	int count;
	ds >> count;
	for (int i = 0; i < count; i++)
	{
		QString name;
		bool output;
		int flags;
		quint64 ptr;

		ds >> ptr;
		ds >> name;
		ds >> output;
		ds >> flags;
		portMap[ptr] = addPort(name, output, flags, ptr);
	}
}



QNEBlock* QNEBlock::clone()
{
	QNEBlock *b = new QNEBlock(0);
	this->scene()->addItem(b);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
		{
			QNEPort *port = (QNEPort*) port_;
			b->addPort(port->portName(), port->isOutput(), port->portFlags(), port->ptr());
		}
	}

	return b;
}

QVector<QNEPort*> QNEBlock::ports()
{
	QVector<QNEPort*> res;
	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
			res.append((QNEPort*) port_);
	}
	return res;
}

QVariant QNEBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{

	Q_UNUSED(change);

	return value;
}

