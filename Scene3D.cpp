#include <QtTest/QTest>
#include <QErrorMessage>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "Scene3D.h"

#include "PolarisTracker.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"


void Scene3D::configTracker(std::string referenceToolFilename, std::string ultrasoundProbeFilename, 
							std::string needleFilename, std::string pointerFilename, QString probeCalibrationFilename)
{

	PolarisTracker* polarisTracker = PolarisTracker::New();
	polarisTracker->setLoggerOn(false);
	std::cout<<"-Initializing SerialCommunication"<<std::endl;
	polarisTracker->initializeSerialCommunication(3);
	std::cout<<"-Initializing Tracker"<<std::endl;
	polarisTracker->initializeTracker();
	std::cout<<"-Initializing Reference Tracker Tool"<<std::endl;
	polarisTracker->initializeTrackerTool(referenceToolFilename);
	std::cout<<"-Initializing Ulatrasound Probe Tracker Tool"<<std::endl;
	polarisTracker->initializeTrackerTool(ultrasoundProbeFilename);
	std::cout<<"-Initializing Needle Tracker Tool"<<std::endl;
	polarisTracker->initializeTrackerTool(needleFilename);
	std::cout<<"-Initializing Pointer Tracker Tool"<<std::endl;
	polarisTracker->initializeTrackerTool(pointerFilename);
	std::cout<<"-Attaching all tools"<<std::endl;
	polarisTracker->attachAllTools();
	std::cout<<"-Creating observers for all tools"<<std::endl;
	polarisTracker->createToolsObervers();

	tracker = polarisTracker->getTracker();

	referenceTool = polarisTracker->getTools().at(0);
	ultrasoundProbeTool = polarisTracker->getTools().at(1);
	needleTool = polarisTracker->getTools().at(2);
	pointerTool = polarisTracker->getTools().at(3);

	coordSystemAObserverReferenceTool = polarisTracker->getObservers().at(0);
	coordSystemAObserverReferenceTool->ObserveTransformEventsFrom(referenceTool);
	coordSystemAObserverUltrasoundProbe = polarisTracker->getObservers().at(1);
	coordSystemAObserverUltrasoundProbe->ObserveTransformEventsFrom(ultrasoundProbeTool);
	coordSystemAObserverNeedle = polarisTracker->getObservers().at(2);
	coordSystemAObserverNeedle->ObserveTransformEventsFrom(needleTool);
	coordSystemAObserverPointer = polarisTracker->getObservers().at(3);
	coordSystemAObserverPointer->ObserveTransformEventsFrom(pointerTool);

	std::cout<<std::endl<<"-Reference Tracker Tool ID: "<<referenceTool->GetTrackerToolIdentifier()<<std::endl;
	std::cout<<"-Ultrasound Tracker Probe Tool ID: "<<ultrasoundProbeTool->GetTrackerToolIdentifier()<<std::endl;
	std::cout<<"-Needle Tracker Tool ID: "<<needleTool->GetTrackerToolIdentifier()<<std::endl;
	std::cout<<"-Pointer Tracker Tool ID: "<<pointerTool->GetTrackerToolIdentifier()<<std::endl;


	std::cout<<std::endl;
	std::cout<<"Loading Calibration Data"<<std::endl;

	std::vector<double> probeCalibrationData;
	probeCalibrationData.reserve(8);

	if (!probeCalibrationFilename.isEmpty())
    {
        QFile file(probeCalibrationFilename);
        if (!file.open(QIODevice::ReadOnly))
           return;

		QTextStream stream(&file);
        QString line;

		for(int i=0;i<8;i++)
        {
			line = stream.readLine();       
			probeCalibrationData.push_back(line.toDouble());
        }
		 file.close(); // when your done.	
	}else
	{
		 std::cout<<"No Probe Calibration Data Loaded"<<std::endl;
	}

	igstk::Transform probeCalibrationTransform;

	igstk::Transform::VectorType probeCalibrationTranslation;
	probeCalibrationTranslation[0] = probeCalibrationData[0];
	probeCalibrationTranslation[1] = probeCalibrationData[1];
	probeCalibrationTranslation[2] = probeCalibrationData[2];

	igstk::Transform::VersorType probeCalibrationRotation;
	probeCalibrationRotation.Set(probeCalibrationData[3], probeCalibrationData[4], probeCalibrationData[5], 0.0);

	igstk::Transform::ErrorType errorValue;
	errorValue=10;

	double validityTimeInMilliseconds;
	validityTimeInMilliseconds = igstk::TimeStamp::GetLongestPossibleTime();

	probeCalibrationTransform.SetTranslationAndRotation(probeCalibrationTranslation, probeCalibrationRotation, errorValue, validityTimeInMilliseconds);


	identityTransform.SetToIdentity(
                      igstk::TimeStamp::GetLongestPossibleTime());



	referenceAxes->RequestSetTransformAndParent(identityTransform, referenceTool);
	usProbe->RequestSetTransformAndParent(identityTransform, ultrasoundProbeTool);
	needle->RequestSetTransformAndParent(identityTransform, needleTool);
	pointer->RequestSetTransformAndParent(identityTransform, pointerTool);

	scene3DWidget->SetTracker(tracker);

	configTrackerFlag = true;
}

void Scene3D::startTracking()
{
	if(configTrackerFlag)
	{
		scene3DWidget->View->RequestSetTransformAndParent(identityTransform, referenceTool);
		scene3DWidget->View->RequestResetCamera();
		scene3DWidget->View->SetCameraFocalPoint( 0.0, 290.0 ,150.0 );
		scene3DWidget->View->SetCameraPosition( 700.0, -150.0 , 150.0 );
		scene3DWidget->View->SetCameraViewUp( 0.0, 0.0 , 1.0 );
		scene3DWidget->View->SetCameraClippingRange( 10.0 , 2000.0 );
		scene3DWidget->View->SetCameraParallelProjection( false );
		scene3DWidget->View->SetRendererBackgroundColor(185.0/255.0,215.0/255.0,249.0/255.0);

		typedef ::itk::Vector<double, 3>    VectorType;
		typedef ::itk::Versor<double>       VersorType;

		std::vector<double> coords;
		coords.reserve(6);

		while( !scene3DWidget->HasQuitted() )
		{
			coords.clear();
			QTest::qWait(0.001);
			igstk::PulseGenerator::CheckTimeouts();

			TransformType             usTransform;
			TransformType             needleTransform;

			VectorType                usPosition;
			VectorType                needlePosition;

  
			coordSystemAObserverReferenceTool->Clear();
			referenceTool->RequestGetTransformToParent();
			coordSystemAObserverUltrasoundProbe->Clear();
			ultrasoundProbeTool->RequestGetTransformToParent();
			coordSystemAObserverNeedle->Clear();
			needleTool->RequestGetTransformToParent();
			coordSystemAObserverPointer->Clear();
			pointer->RequestGetTransformToParent();

			if (coordSystemAObserverReferenceTool->GotTransform())
			{
				usTransform = coordSystemAObserverUltrasoundProbe->GetTransform();
				usPosition = usTransform.GetTranslation();
				
				coords.push_back(usPosition[0]);
				coords.push_back(usPosition[1]);
				coords.push_back(usPosition[2]);

				needleTransform = coordSystemAObserverNeedle->GetTransform();
				needlePosition = needleTransform.GetTranslation();

				coords.push_back(needlePosition[0]);
				coords.push_back(needlePosition[1]);
				coords.push_back(needlePosition[2]);

				scene3DWidget->setCoords(coords);
			}
		}

		tracker->RequestClose();
		delete scene3DWidget;
	}
	else{
		QErrorMessage errorMessage;
        errorMessage.showMessage(
            "Tracker is not configure, </ br> please configure tracker first");
        errorMessage.exec();
	}
}

void Scene3D::initLogger()
{
	if(configTrackerFlag)
	{
		typedef igstk::Object::LoggerType LoggerType;

		LoggerType::Pointer logger = LoggerType::New();
		itk::StdStreamLogOutput::Pointer fileOutput = itk::StdStreamLogOutput::New();

		std::ofstream ofs( "log.txt" );
		fileOutput->SetStream(ofs);
		logger->AddLogOutput( fileOutput );

		scene3DWidget->qtDisplay->SetLogger( logger );
		tracker->SetLogger(logger);
	}else{
		QErrorMessage errorMessage;
        errorMessage.showMessage(
            "Tracker is not configure, </ br> please configure tracker first");
        errorMessage.exec();
	}
}


void Scene3D::init3DScene()
{
	igstk::RealTimeClock::Initialize();
    scene3DWidget = new Scene3DWidget();

	scene3DWidget->setScene3D(this);

    referenceAxes = igstk::AxesObject::New();
	usProbe = igstk::USProbeObject::New();
    needle = igstk::NeedleObject::New();
	pointer = igstk::PolarisPointerObject::New();

    referenceAxes->SetSize(100,100,100);

	igstk::AxesObjectRepresentation::Pointer referenceAxesRepresentation =
		igstk::AxesObjectRepresentation::New();
	referenceAxesRepresentation->RequestSetAxesObject(referenceAxes);

	igstk::USProbeObjectRepresentation::Pointer usProbeRepresentation = 
		igstk::USProbeObjectRepresentation::New();
	usProbeRepresentation->RequestSetUSProbeObject(usProbe);

	igstk::NeedleObjectRepresentation::Pointer needleRepresentation = 
		igstk::NeedleObjectRepresentation::New();
	needleRepresentation->RequestSetNeedleObject(needle);

	igstk::PolarisPointerObjectRepresentation::Pointer pointerRepresentation = 
		igstk::PolarisPointerObjectRepresentation::New();
	pointerRepresentation->RequestSetPolarisPointerObject(pointer);

	scene3DWidget->View->RequestAddObject(referenceAxesRepresentation);
	scene3DWidget->View->RequestAddObject(usProbeRepresentation);
	scene3DWidget->View->RequestAddObject(needleRepresentation);
	scene3DWidget->View->RequestAddObject(pointerRepresentation);

	scene3DWidget->qtDisplay->RequestEnableInteractions();

	scene3DWidget->Show();

	configTrackerFlag = false;
}

