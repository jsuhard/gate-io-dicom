/**
 * \class GateVDICOM
 * \details Virtual class for all DICOM images.
 * \author Jérôme Suhard, jerome@suhard.fr
 * \date October 2014
 * \copyright GNU Lesser General Public Licence (LGPL)
 */

/*
 * Copyright (C): OpenGATE Collaboration
 *
 * This software is distributed under the terms
 * of the GNU Lesser General  Public Licence (LGPL)
 * See GATE/LICENSE.txt for further details
 */

#include "GateConfiguration.h"
#ifdef GATE_USE_DICOM

#include <iostream>
#include <algorithm>
#include <cmath>

#include "GateVDICOM.hh"
#include "GateDICOMTags.hh"

const std::string GateVDICOM::manufacturer = "opengatecollaboration.org";
const std::string GateVDICOM::manufacturerModelName = "GATE";
const std::string GateVDICOM::softwareVersion = GATE_VERSION;

#include "gdcmUIDGenerator.h"

//-----------------------------------------------------------------------------
GateVDICOM::GateVDICOM() {
    size.resize(3);
    spacing.resize(3);
    spacing[0] = 1;
    spacing[1] = 1;
    spacing[2] = 1;
    origin.resize(3);
    SOPInstanceUID = gdcm::UIDGenerator().Generate();
    seriesInstanceUID = gdcm::UIDGenerator().Generate();
    pixels.resize(0);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GateVDICOM::~GateVDICOM() {
    pixels.clear();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GateVDICOM::Print()
{
    std::cout << "size = " << size[0] << " " << size[1] << " " << size[2] << std::endl
    << "spacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << std::endl
    << "origin = " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;
}
//-----------------------------------------------------------------------------

void GateVDICOM::setTags(const GateDICOMTags & tags)
{
    this->tags = tags;
}

const GateDICOMTags & GateVDICOM::getTags() const
{
    return tags;
}

void GateVDICOM::setPixels(const std::vector<float> & pixels)
{
    const unsigned int pixelsCount = getPixelsCount();
    
    double slope = *std::max_element(pixels.begin(),pixels.end()) / (double) (INT16_MAX);
    setSlope(slope);
    
    this->pixels.resize(pixelsCount);
    
    for(unsigned int i = 0; i < pixelsCount; i++) {
        this->pixels[i] = pixels[i] / slope;
    }
    
}

void GateVDICOM::getPixels(std::vector<float> & output)
{
    const unsigned int pixelsCount = getPixelsCount();
    output.resize(pixels.size());
    
    const double intercept = getIntercept();
    const double slope = getSlope();
    
    for(unsigned int i = 0; i < pixelsCount; i++) {
        output[i] = pixels[i] * slope + intercept;
    }
}

void GateVDICOM::getPixels(std::vector<int16_t> & output)
{
    output.resize(pixels.size());
    
    output.assign(pixels.begin(), pixels.end());
}

#endif /* GATE_USE_DICOM */

