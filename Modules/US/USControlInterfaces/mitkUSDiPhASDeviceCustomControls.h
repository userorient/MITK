/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef MITKUSDiPhASDeviceCustomControls_H_HEADER_INCLUDED_
#define MITKUSDiPhASDeviceCustomControls_H_HEADER_INCLUDED_

#include "mitkUSAbstractControlInterface.h"
#include "mitkUSImageVideoSource.h"
#include "mitkUSDevice.h"

#include <itkObjectFactory.h>

#include <QLabel>

namespace mitk {
/**
  * \brief Custom controls for mitk::USDiPhASDevice.
  */
class MITKUS_EXPORT USDiPhASDeviceCustomControls : public USAbstractControlInterface
{
public:
  mitkClassMacro(USDiPhASDeviceCustomControls, USAbstractControlInterface);
  mitkNewMacro1Param(Self, itk::SmartPointer<USDevice>);

  /**
    * Activate or deactivate the custom controls. This is just for handling
    * widget visibility in a GUI for example.
    */
  virtual void SetIsActive( bool isActive ) override;

  /**
    * \return if this custom controls are currently activated
    */
  virtual bool GetIsActive( ) override;

  //Transmit
  virtual void SetTransmitPhaseLength(double us);
  virtual void SetExcitationFrequency(double MHz);
  virtual void SetTransmitEvents(int events);
  virtual void SetVoltage(int voltage);
  virtual void SetMode(bool interleaved);

  //Receive
  virtual void SetScanDepth(double mm);
  virtual void SetAveragingCount(int count);
  virtual void SetTGCMin(int min);
  virtual void SetTGCMax(int max);
  virtual void SetDataType(int type); // 0= raw; 1= beamformed;

  //Beamforming
  virtual void SetPitch(double mm);
  virtual void SetReconstructedSamples(int samples);
  virtual void SetReconstructedLines(int lines);
  virtual void SetSpeedOfSound(int mps);

  //Bandpass
  virtual void SetBandpassEnabled(bool bandpass);
  virtual void SetLowCut(double MHz);
  virtual void SetHighCut(double MHz);

protected:
  /**
    * Class needs an mitk::USDevice object for beeing constructed.
    */
  USDiPhASDeviceCustomControls( itk::SmartPointer<USDevice> device );
  virtual ~USDiPhASDeviceCustomControls( );

  bool                          m_IsActive;
  USImageVideoSource::Pointer   m_ImageSource;

  /** virtual handlers implemented in Device Controls
    */
  //Transmit
  virtual void OnSetTransmitPhaseLength(double us);
  virtual void OnSetExcitationFrequency(double MHz);
  virtual void OnSetTransmitEvents(int events);
  virtual void OnSetVoltage(int voltage);
  virtual void OnSetMode(bool interleaved);

  //Receive
  virtual void OnSetScanDepth(double mm);
  virtual void OnSetAveragingCount(int count);
  virtual void OnSetTGCMin(int min);
  virtual void OnSetTGCMax(int max);
  virtual void OnSetDataType(int type); // 0= raw; 1= beamformed;

  //Beamforming
  virtual void OnSetPitch(double mm);
  virtual void OnSetReconstructedSamples(int samples);
  virtual void OnSetReconstructedLines(int lines);
  virtual void OnSetSpeedOfSound(int mps);

  //Bandpass
  virtual void OnSetBandpassEnabled(bool bandpass);
  virtual void OnSetLowCut(double MHz);
  virtual void OnSetHighCut(double MHz);

};
} // namespace mitk

#endif // MITKUSDiPhASDeviceCustomControls_H_HEADER_INCLUDED_