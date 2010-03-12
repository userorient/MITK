
#ifndef QMITKTRANSFERFUNCTIONWIDGET_H
#define QMITKTRANSFERFUNCTIONWIDGET_H

#include "ui_QmitkTransferFunctionWidget.h"
#include "QmitkExtExports.h"

#include <mitkCommon.h>

#include <QWidget>

#include <mitkDataNode.h>
#include <mitkTransferFunctionProperty.h>

#include <QSlider>
#include <QPushButton>

#include <QmitkTransferFunctionWidget.h>


class QmitkExt_EXPORT QmitkTransferFunctionWidget : public QWidget, public Ui::QmitkTransferFunctionWidget
{

  Q_OBJECT

  public:

    QmitkTransferFunctionWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~QmitkTransferFunctionWidget () ;

	  void SetDataNode(mitk::DataNode* node);

  public slots:

	  void SetXValueScalar();
	  void SetYValueScalar();
	  void SetXValueGradient();
	  void SetYValueGradient();
	  void SetXValueColor();
	 
	  void OnUpdateCanvas();
	  void UpdateRanges();
	  void OnResetSlider();
	  
	  void OnSpanChanged (int lower, int upper);
	  

  protected:
  
    mitk::TransferFunctionProperty::Pointer tfpToChange;
    
    int m_RangeSliderMin;
    int m_RangeSliderMax;

    mitk::SimpleHistogramCache histogramCache;
    
};
#endif
