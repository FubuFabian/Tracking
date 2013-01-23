#include "Scene3DWidget.h"
#include "ui_Scene3DWidget.h"
#include "Scene3D.h"

#include <QLayout>
#include <QString>
#include <QMainWindow>
#include <QtGui>

Scene3DWidget::Scene3DWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scene3DWidget)
{
    ui->setupUi(this);

    this->View = igstk::View3D::New();
    this->qtDisplay = new igstk::QTWidget();
    this->qtDisplay->RequestSetView( this->View );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->QLayout::addWidget(this->qtDisplay);
    ui->Display->setLayout(layout);

	ui->initLoggetBt->setEnabled(false);
	ui->startTrackingBt->setEnabled(false);

    this->quit =  false;
}

Scene3DWidget::~Scene3DWidget()
{
    delete ui;
}

void Scene3DWidget::Show()
{
    this->quit = false;
    this->show();
    this->View->RequestStart();
}

void Scene3DWidget::Quit()
{
    this->View->RequestStop();
	this->m_Tracker->RequestStopTracking();
    this->quit = true;
    this->hide();
}

void Scene3DWidget::startTracking()
{
    this->m_Tracker->RequestStartTracking();
	scene3D->startTracking();
}

void Scene3DWidget::configTracker()
{
    QString qtReferenceToolFilename = QFileDialog::getOpenFileName(this, tr("Open Reference Tool Rom"),
        QDir::currentPath(),tr("Rom Files (*.rom)"));
    QString qtUltrasoundProbeFilename = QFileDialog::getOpenFileName(this, tr("Open Ultrasound Probe Tool Rom"),
        QDir::currentPath(),tr("Rom Files (*.rom)"));
    QString qtNeedleFilename = QFileDialog::getOpenFileName(this, tr("Open Needle Tool Rom"),
        QDir::currentPath(),tr("Rom Files (*.rom)"));
	QString qtPointerFilename = QFileDialog::getOpenFileName(this, tr("Open Pointer Tool Rom"),
        QDir::currentPath(),tr("Rom Files (*.rom)"));
	QString probeCalibrationFilename = QFileDialog::getOpenFileName(this, tr("Open Probe Calibration File"),
        QDir::currentPath(),tr("Text (*.txt)"));

    std::string referenceToolFilename = std::string(qtReferenceToolFilename.toAscii().data());
    std::string ultrasoundProbeFilename = std::string(qtUltrasoundProbeFilename.toAscii().data());
    std::string needleFilename = std::string(qtNeedleFilename.toAscii().data());
	std::string pointerFilename = std::string(qtPointerFilename.toAscii().data());

	scene3D->configTracker(referenceToolFilename, ultrasoundProbeFilename, needleFilename, 
							pointerFilename, probeCalibrationFilename);

	ui->initLoggetBt->setEnabled(true);
	ui->startTrackingBt->setEnabled(true);
}

void Scene3DWidget::initLogger()
{
	scene3D->initLogger();
}

bool Scene3DWidget::HasQuitted()
{
    return this->quit;
}

void Scene3DWidget::setCoords(std::vector<double> coords)
{
	QString str0 = QString::number(coords[0]);
	ui->usProbeX->setText(str0);

	QString str1 = QString::number(coords[1]);
	ui->usProbeY->setText(str1);

	QString str2 = QString::number(coords[2]);
	ui->usProbeZ->setText(str2);

	QString str3 = QString::number(coords[3]);
	ui->needleX->setText(str3);

	QString str4 = QString::number(coords[4]);
	ui->needleY->setText(str4);

	QString str5 = QString::number(coords[5]);
	ui->needleZ->setText(str5);
}

void Scene3DWidget::SetTracker( TrackerType * tracker)
{
	this->m_Tracker = tracker;
}

void Scene3DWidget::setScene3D(Scene3D* scene3D)
{
    this->scene3D = scene3D;
}