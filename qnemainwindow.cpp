#include "qnemainwindow.h"
#include "ui_qnemainwindow.h"

#include "qneblock.h"
#include "qnodeseditor.h"

#include <QGraphicsScene>
#include <QFileDialog>

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

#include "qneport.h"

// http://doc-snapshots.qt.io/4.8/draganddrop-puzzle.html

QNEMainWindow::QNEMainWindow(QWidget *parent) :  QMainWindow(parent)
{
	// -------------------------------------
	// central widget
	// -------------------------------------
	QWidget *window = new QWidget();
	setCentralWidget(window);
	QHBoxLayout *layout = new QHBoxLayout;
	window->setLayout(layout);	
	// -------------------------------------
	// Main menu
	// ------------------------------------
	createMenuBar();
	layout->setMenuBar(menuBar);


	// -------------------------------------
	// Tools
	// ------------------------------------

	QVBoxLayout *Tools_Layout = new QVBoxLayout;
    layout->addLayout(Tools_Layout);
	// -------------------------------------
	// Tree
	// ------------------------------------
	QTreeWidget *treeWidget = new QTreeWidget();
	treeWidget->setColumnCount(1);
	treeWidget->setHeaderHidden(true);
	treeWidget->setFixedWidth(250);
	// создаем новый итем (пусть сначала базовый)
	QTreeWidgetItem *topLevelItem=new QTreeWidgetItem(treeWidget);
	// вешаем его на наше дерево в качестве топ узла.
	treeWidget->addTopLevelItem(topLevelItem);
	// укажем текст итема
	topLevelItem->setText(0,"Item");
	// создаем новый итем и сразу вешаем его на наш базовый
	QTreeWidgetItem *item=new QTreeWidgetItem(topLevelItem);
	// укажем текст итема
	item->setText(0,"SubItem");
	Tools_Layout->addWidget(treeWidget);

	// -------------------------------------
	// Property Browser
	// ------------------------------------	
 
	QtIntPropertyManager *intManager;
    QtProperty *priority;
    
    intManager = new QtIntPropertyManager;
    priority = intManager->addProperty("Priority");   
    priority->setToolTip("Task Priority");
    intManager->setRange(priority, 1, 5);
    intManager->setValue(priority, 3);

    QtEnumPropertyManager *enumManager=new QtEnumPropertyManager();
    QtProperty *reportType;
	reportType = enumManager->addProperty("Report Type");  
    QStringList types;   
    types << "Bug" << "Suggestion" << "To Do";
    enumManager->setEnumNames(reportType, types);
    enumManager->setValue(reportType, 1); // "Suggestion"

    QtGroupPropertyManager *groupManager;
    QtProperty *task1;

    groupManager = new QtGroupPropertyManager;
    task1 = groupManager->addProperty("Task 1");
    
    task1->addSubProperty(priority);
    task1->addSubProperty(reportType);

    QtSpinBoxFactory *spinBoxFactory;
    QtEnumEditorFactory *enumFactory;
    
    spinBoxFactory = new QtSpinBoxFactory;
    enumFactory = new QtEnumEditorFactory;
    
    QtTreePropertyBrowser *browser;
    browser = new QtTreePropertyBrowser;
    browser->setFactoryForManager(intManager, spinBoxFactory);
    browser->setFactoryForManager(enumManager, enumFactory);
	browser->setFixedWidth(250);

    browser->addProperty(task1);
   // browser->show();

	Tools_Layout->addWidget(browser);

	// -------------------------------------
	// Graphic view
	// ------------------------------------
	view = new QGraphicsView(this);
	view->setInteractive(true);
	view->setBackgroundBrush(QBrush(QColor(60,60,60), Qt::SolidPattern));
	scene = new QGraphicsScene();
	view->setScene(scene);
	view->setRenderHint(QPainter::Antialiasing, true);
	layout->addWidget(view);

	// -------------------------------------
	// Nodes editor
	// -----------------------------------
	nodesEditor = new QNodesEditor(this);
	nodesEditor->install(scene);


	QNEBlock *b1 = new QNEBlock(0);
	scene->addItem(b1);
	b1->block_name="U1";
	b1->block_type="Source";
	b1->addOutputPort("img");
	b1->setPos(0, 0);

	QNEBlock *b2 = new QNEBlock(0);
	scene->addItem(b2);
	b2->block_name="U2";
	b2->block_type="Threshold";
	b2->addInputPort("img");
	b2->addOutputPort("thr");
	b2->setPos(150, 0);


}

void QNEMainWindow::createMenuBar(void)
{
	QAction *saveXMLAct = new QAction(tr("&Export XML"), this);
	saveXMLAct->setShortcuts(QKeySequence::SaveXML);
	saveXMLAct->setStatusTip(tr("Save a file"));
	connect(saveXMLAct, SIGNAL(triggered()), this, SLOT(saveXML()));

	QAction *quitAct = new QAction(tr("&Quit"), this);
	quitAct->setShortcuts(QKeySequence::Quit);
	quitAct->setStatusTip(tr("Quit the application"));
	connect(quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

	QAction *loadAct = new QAction(tr("&Load"), this);
	loadAct->setShortcuts(QKeySequence::Open);
	loadAct->setStatusTip(tr("Open a file"));
	connect(loadAct, SIGNAL(triggered()), this, SLOT(loadFile()));

	QAction *saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save a file"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(saveFile()));

	QAction *addAct = new QAction(tr("&Add"), this);
	addAct->setStatusTip(tr("Add a block"));
	connect(addAct, SIGNAL(triggered()), this, SLOT(addBlock()));

	fileMenu = new QMenu(tr("&File"),this);
	menuBar = new QMenuBar;
	fileMenu->addAction(addAct);
	fileMenu->addAction(loadAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveXMLAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);
	setWindowTitle(tr("Node Editor"));
	menuBar->addMenu(fileMenu);
}

QNEMainWindow::~QNEMainWindow()
{

}

void QNEMainWindow::saveXML()
{
	QString fname = QFileDialog::getSaveFileName();
	if (fname.isEmpty())
		return;

	QFile f(fname);
	f.open(QFile::WriteOnly);
	QTextStream ds(&f);
	nodesEditor->saveXML(ds);
}

void QNEMainWindow::saveFile()
{
	QString fname = QFileDialog::getSaveFileName();
	if (fname.isEmpty())
		return;

	QFile f(fname);
	f.open(QFile::WriteOnly);
	QDataStream ds(&f);
	nodesEditor->save(ds);
}

void QNEMainWindow::loadFile()
{
	QString fname = QFileDialog::getOpenFileName();
	if (fname.isEmpty())
		return;

	QFile f(fname);
	f.open(QFile::ReadOnly);
	QDataStream ds(&f);
	nodesEditor->load(ds);
}

void QNEMainWindow::addBlock()
{
	static int n_item=3;
	QNEBlock *b = new QNEBlock(0);

	scene->addItem(b);
	b->block_name="U"+QString::number(n_item);
	b->block_type="Source";
	static const char* names[] = {"Vin", "Voutsadfasdf", "Imin", "Imax", "mul", "add", "sub", "div", "Conv", "FFT"};
	for (int i = 0; i < 4 + rand() % 3; i++)
	{
		b->addPort(names[rand() % 10], rand() % 2, 0, 0);
		b->setPos(view->sceneRect().center().toPoint());
	}
	n_item++;
}
