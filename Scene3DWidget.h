#ifndef SCENE3DWIDGET_H
#define SCENE3DWIDGET_H

#include <QWidget>
#include "igstkView3D.h"
#include "igstkQTWidget.h"
#include "igstkTracker.h"
#include "igstkConfigure.h"
//

class Scene3D;

namespace Ui {
class Scene3DWidget;
}

//!Window for tracker display
/*!
  This class has the main interaction function to track and display the biopsy instruments.
*/
class Scene3DWidget : public QWidget
{
    Q_OBJECT
    
public:

	/** \brief Class constructor*/
    explicit Scene3DWidget(QWidget *parent = 0);
    ~Scene3DWidget();
    
    typedef igstk::Tracker TrackerType;
    typedef TrackerType::Pointer TrackerPointer;

    igstk::QTWidget * qtDisplay; ///<A qt widget
    igstk::View3D::Pointer View; ///<The IGSTK View

	/** \brief Show the widget nd start displaying the scene*/
    void Show();

	/** \brief Returns the quit flag*/
	bool HasQuitted();

	/** \brief Set the tracker for the instruments
	* \param[in] Polaris tracker*/
    void SetTracker( TrackerType * tracker);

	/** \brief Set the 3DScene*/
    void setScene3D(Scene3D *);

	/** \brief Set the instruments coords in the widget*/
	void setCoords(std::vector<double>);

private:

    Ui::Scene3DWidget *ui; ///<The User Interface
    bool quit; ///<Quit Flag
    TrackerPointer m_Tracker; ///<Polaris Tracker
	Scene3D* scene3D; ///<Scene3D object with the IGSTK objects

private slots:

	/** \brief Set the quit flag to TRUE*/
    void Quit();

	/** \brief Start tracking the instruments*/
    void startTracking();

	/** \brief Config the polaris tracker*/
	void configTracker();

	/** \brief Init the tracker logger*/
	void initLogger();

	/** \brief Open ultrasound volume*/
	void openVolume();
};

#endif // SCENE3DWIDGET_H
