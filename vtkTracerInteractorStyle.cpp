#include "vtkTracerInteractorStyle.h"
#include "CheckCalibrationErrorWidget.h"

#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>


vtkStandardNewMacro(vtkTracerInteractorStyle);

vtkTracerInteractorStyle::vtkTracerInteractorStyle()
{
    tracer = vtkSmartPointer<vtkImageTracerWidget>::New();
    tracer->GetLineProperty()->SetLineWidth(1);
    tracer->AddObserver(vtkCommand::EndInteractionEvent, this, &vtkTracerInteractorStyle::catchEvent);
}

void vtkTracerInteractorStyle::initTracer(vtkSmartPointer<vtkImageActor> imageActor)
{
    tracer->SetInteractor(this->Interactor);
    tracer->SetViewProp(imageActor);
    tracer->ProjectToPlaneOn();
    tracer->SnapToImageOn();
    tracer->On();
}

void vtkTracerInteractorStyle::catchEvent(vtkObject* caller, long unsigned int eventId, void* callData)
{

  vtkImageTracerWidget* tracerEvent =
    static_cast<vtkImageTracerWidget*>(caller);

  vtkSmartPointer<vtkPolyData> path = vtkSmartPointer<vtkPolyData>::New();
  tracerEvent->GetPath(path);

  vtkSmartPointer<vtkPoints> points = path->GetPoints();

  calibrationErrorWidget->setTracedPoints(points);

}

void vtkTracerInteractorStyle::clearTracer()
{
  vtkSmartPointer<vtkPoints> emptyPoints = vtkSmartPointer<vtkPoints>::New();
  emptyPoints->InsertNextPoint(0, 0, 0);
  emptyPoints->InsertNextPoint(0, 0, 0);

  tracer->InitializeHandles(emptyPoints);
  tracer->Modified();
  tracer->Off();
}

void vtkTracerInteractorStyle::setCalibrationErrorWidget(CheckCalibrationErrorWidget* calibrationErrorWidget)
{
  this->calibrationErrorWidget = calibrationErrorWidget;
}