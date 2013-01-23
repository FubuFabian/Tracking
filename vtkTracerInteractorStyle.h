#ifndef VTKTRACERINTERACTORSTYLE_H
#define VTKTRACERINTERACTORSTYLE_H

#include <vtkImageTracerWidget.h>
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>

class CheckCalibrationErrorWidget;

class vtkTracerInteractorStyle : public vtkInteractorStyleImage
{

public:

    static vtkTracerInteractorStyle* New();
    vtkTypeMacro(vtkTracerInteractorStyle, vtkInteractorStyleImage);
    vtkTracerInteractorStyle();

    void initTracer(vtkSmartPointer<vtkImageActor> imageActor);
	void clearTracer();

	void setCalibrationErrorWidget(CheckCalibrationErrorWidget* calibrationErrorWidget);

private:

    
    void catchEvent(vtkObject* caller, long unsigned int eventId, void* callData);

    vtkSmartPointer<vtkImageTracerWidget> tracer;

	CheckCalibrationErrorWidget* calibrationErrorWidget;

};


#endif // VTKTRACERINTERACTORSTYLE_H
