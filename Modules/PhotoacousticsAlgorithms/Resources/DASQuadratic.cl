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

__kernel void ckDASQuad(
  __read_only image3d_t dSource, // input image
  __global float* dDest, // output buffer
  __global float* apodArray,
  unsigned short apodArraySize,
  unsigned short outputS,
  unsigned short outputL,
  float SpeedOfSound,
  float RecordTime,
  float Pitch,
  float Angle,
  unsigned short PAImage  // parameters
)
{
  // get thread identifier
  unsigned int globalPosX = get_global_id(0);
  unsigned int globalPosY = get_global_id(1);
  unsigned int globalPosZ = get_global_id(2);
  
  // get image width and weight
  const unsigned int inputL = get_image_width( dSource );
  const unsigned int inputS = get_image_height( dSource ) / (PAImage + 1);
  const unsigned int Slices = get_image_depth( dSource );

  // create an image sampler
  const sampler_t defaultSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST ;

  // terminate non-valid threads
  if ( globalPosX < outputL && globalPosY < outputS && globalPosZ < Slices )
  {	
    float l_i = (float)globalPosX / outputL * inputL;
    float s_i = (float)globalPosY / outputS * inputS;

    float tan_phi = tan(Angle / 360 * 2 * M_PI);
    float part_multiplicator = tan_phi * RecordTime / inputS * SpeedOfSound / Pitch;

    float part = part_multiplicator * s_i;
    if (part < 1)
      part = 1;

    short maxLine = min((l_i + part) + 1, (float)inputL);
    short minLine = max((l_i - part), 0.0f);
    short usedLines = (maxLine - minLine);
	float apod_mult = apodArraySize / (maxLine - minLine);
	
	short AddSample = 0;
	float output = 0;
	float delayMultiplicator = pow(inputS / (RecordTime*SpeedOfSound) * Pitch, 2) / s_i / 2;

    for (short l_s = minLine; l_s < maxLine; ++l_s)
    {
      AddSample = delayMultiplicator * pow((l_s - l_i), 2) + s_i;
      if (AddSample < inputS && AddSample >= 0) 
        output += apodArray[(short)((l_s - minLine)*apod_mult)] * read_imagef( dSource, defaultSampler, (int4)(l_s, AddSample, globalPosZ, 0 )).x;
      else
        --usedLines;
    }
	
    dDest[ globalPosZ * outputL * outputS + globalPosY * outputL + globalPosX ] = output / usedLines;
  }
}