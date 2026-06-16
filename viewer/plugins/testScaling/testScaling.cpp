#include "testScaling.h"
#include "glwidget.h"
#include <QCoreApplication>
#include <QGuiApplication>

void TestScaling::onPluginLoad()
{
    QCoreApplication * coreApp = QCoreApplication::instance();
    QGuiApplication * myApp =
          dynamic_cast<QGuiApplication*>(coreApp);
    if (not myApp) { cout << "Did not get a QGuiApplication pointer!" << endl;}
    else {
        cout << "Scale Upper Bound: " << myApp->devicePixelRatio() << endl;
    }
}

void TestScaling::preFrame()
{
    cout << "Scaling: " << glwidget()->devicePixelRatio() << "                 \r";
}

void TestScaling::postFrame()
{
	
}

void TestScaling::onObjectAdd()
{
	
}

bool TestScaling::drawScene()
{
	return false; // return true only if implemented
}

bool TestScaling::drawObject(int)
{
	return false; // return true only if implemented
}

bool TestScaling::paintGL()
{
	return false; // return true only if implemented
}

void TestScaling::keyPressEvent(QKeyEvent *)
{
	
}

void TestScaling::mouseMoveEvent(QMouseEvent *)
{
	
}

