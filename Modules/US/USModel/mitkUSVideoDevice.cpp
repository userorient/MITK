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

#include "mitkUSVideoDevice.h"

const std::string mitk::USVideoDevice::DeviceClassIdentifier = "org.mitk.modules.us.USVideoDevice";

mitk::USVideoDevice::USVideoDevice(int videoDeviceNumber, std::string manufacturer, std::string model) : mitk::USDevice(manufacturer, model)
{
  Init();
  m_SourceIsFile = false;
  m_DeviceID = videoDeviceNumber;
  m_FilePath = "";
}

mitk::USVideoDevice::USVideoDevice(std::string videoFilePath, std::string manufacturer, std::string model) : mitk::USDevice(manufacturer, model)
{
  Init();
  m_SourceIsFile = true;
  m_FilePath = videoFilePath;
}

mitk::USVideoDevice::USVideoDevice(int videoDeviceNumber, mitk::USImageMetadata::Pointer metadata) : mitk::USDevice(metadata)
{
  Init();
  m_SourceIsFile = false;
  m_DeviceID = videoDeviceNumber;
  m_FilePath = "";
}

mitk::USVideoDevice::USVideoDevice(std::string videoFilePath, mitk::USImageMetadata::Pointer metadata) : mitk::USDevice(metadata)
{
  Init();
  m_SourceIsFile = true;
  m_FilePath = videoFilePath;
}

mitk::USVideoDevice::~USVideoDevice()
{

}


void mitk::USVideoDevice::Init()
{
  m_Source = mitk::USImageVideoSource::New();
  m_ControlInterfaceCustom = mitk::USVideoDeviceCustomControls::New(m_Source);
  //this->SetNumberOfInputs(1);
  this->SetNumberOfOutputs(1);

  // mitk::USImage::Pointer output = mitk::USImage::New();
  // output->Initialize();
  this->SetNthOutput(0, this->MakeOutput(0));
}

std::string mitk::USVideoDevice::GetDeviceClass(){
  return "org.mitk.modules.us.USVideoDevice";
}

mitk::USAbstractControlInterface::Pointer mitk::USVideoDevice::GetControlInterfaceCustom()
{
  MITK_INFO << "Custom control interface does not exist for this object.";
  return 0;
}

bool mitk::USVideoDevice::OnInitialization()
{
  // nothing to do at initialization of video device
  return true;
}

bool mitk::USVideoDevice::OnConnection()
{
  if (m_SourceIsFile){
    m_Source->SetVideoFileInput(m_FilePath);
  } else {
     m_Source->SetCameraInput(m_DeviceID);
  }
  SetSourceCropArea();
  return true;
}

bool mitk::USVideoDevice::OnDisconnection()
{
  if (m_DeviceState == State_Activated) this->Deactivate();
  return true;
}


bool mitk::USVideoDevice::OnActivation()
{
  // make sure that video device is ready before aquiring images
  if ( ! m_Source->GetIsReady() )
  {
    MITK_WARN("mitkUSDevice")("mitkUSVideoDevice") << "Could not activate us video device. Check if video grabber is configured correctly.";
    return false;
  }

  MITK_INFO << "Activated UsVideoDevice!";
  return true;
}


bool mitk::USVideoDevice::OnDeactivation()
{
  // happens automatically when m_Active is set to false
  return true;
}

void mitk::USVideoDevice::GenerateData()
{
  mitk::USImage::Pointer result;
  result = m_Image;

  // Set Metadata
  result->SetMetadata(this->m_Metadata);
  // Apply Transformation
  this->ApplyCalibration(result);
  // Set Output
  this->SetNthOutput(0, result);
}

void mitk::USVideoDevice::UnregisterOnService()
{
  if (m_DeviceState == State_Activated) { this->Deactivate(); }
  if (m_DeviceState == State_Connected) { this->Disconnect(); }

  mitk::USDevice::UnregisterOnService();
}

mitk::USImageSource::Pointer mitk::USVideoDevice::GetUSImageSource()
{
  return m_Source.GetPointer();
}

void mitk::USVideoDevice::SetSourceCropArea()
{
  if (this->m_Source.IsNotNull())
  {
    if((m_CropArea.cropBottom==0)&&
      (m_CropArea.cropTop==0)&&
      (m_CropArea.cropLeft==0)&&
      (m_CropArea.cropRight==0))
    {
      this->m_Source->RemoveRegionOfInterest();
    }
    else
    {
      int right = m_Source->GetImageWidth() - m_CropArea.cropRight;
      int bottom = m_Source->GetImageHeight() - m_CropArea.cropBottom;
      this->m_Source->SetRegionOfInterest(m_CropArea.cropLeft,
        m_CropArea.cropTop,
        right,
        bottom);
    }
  }
  else
  {
    MITK_WARN << "Cannot set crop are, source is not initialized!";
  }
}

void mitk::USVideoDevice::SetCropArea(mitk::USDevice::USImageCropArea newArea)
{
  m_CropArea = newArea;
  MITK_INFO << "Set Crop Area L:" << m_CropArea.cropLeft << " R:" << m_CropArea.cropRight << " T:" << m_CropArea.cropTop << " B:" << m_CropArea.cropBottom;
  if (this->GetIsConnected() || this->GetIsActive()) { SetSourceCropArea(); }
}

