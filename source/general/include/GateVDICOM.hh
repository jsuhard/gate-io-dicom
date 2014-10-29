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

#ifndef __GATEVDICOM_HH__
#define __GATEVDICOM_HH__

#include "GateConfiguration.h"

#ifdef GATE_USE_DICOM

#include <string>
#include <vector>

#include "GateDICOMTags.hh"

class GateVDICOM
{
public:

    GateVDICOM();
    virtual ~GateVDICOM();

    void Print();

    void setXSize(unsigned int pixels) { size[0] = pixels; }
    void setYSize(unsigned int pixels) { size[1] = pixels; }
    void setZSize(unsigned int pixels) { size[2] = pixels; }
    unsigned int getXSize() { return size[0]; }
    unsigned int getYSize() { return size[1]; }
    unsigned int getZSize() { return size[2]; }

    unsigned int getPixelsCount() const { return size[0] * size[1] * size[2]; }

    void setXSpacing(double mm) { spacing[0] = mm; }
    void setYSpacing(double mm) { spacing[1] = mm; }
    void setZSpacing(double mm) { spacing[2] = mm; }
    double getXSpacing() { return spacing[0]; }
    double getYSpacing() { return spacing[1]; }
    double getZSpacing() { return spacing[2]; }

    void setXOrigin(double mm) { origin[0] = mm; }
    void setYOrigin(double mm) { origin[1] = mm; }
    void setZOrigin(double mm) { origin[2] = mm; }
    double getXOrigin() { return origin[0]; }
    double getYOrigin() { return origin[1]; }
    double getZOrigin() { return origin[2]; }

    void setIntercept(double intercept) { this->intercept = intercept; }
    double getIntercept() { return intercept; }
    void setSlope(double slope) { this->slope = slope; }
    double getSlope() { return slope; }

    void setPixels(const std::vector<float> & pixels);
    void getPixels(      std::vector<float> & output);
    void getPixels(    std::vector<int16_t> & output);

    virtual bool Read(const std::string fileName) = 0;
    virtual bool Write(const std::string fileName) = 0;

    const std::string getSOPInstanceUID() { return SOPInstanceUID; }

    void setTags(const GateDICOMTags & tags);
    const GateDICOMTags & getTags() const;

    void setSeriesInstanceUID(const std::string seriesInstanceUID) { this->seriesInstanceUID = seriesInstanceUID; }
    const std::string getSeriesInstanceUID() { return seriesInstanceUID; }

protected:
    GateDICOMTags tags;
    std::string SOPInstanceUID;                     // 0008,0018
    std::string seriesInstanceUID;                  // 0020,000e

    static const std::string SOPClassUID;           // 0008,0016
    static const std::string modality;              // 0008,0060
    static const std::string manufacturer;          // 0008,0070
    static const std::string manufacturerModelName; // 0008,1090
    static const std::string softwareVersion;       // 0018,1020

    std::vector<unsigned int> size;
    std::vector<double> spacing;
    std::vector<double> origin;
    double intercept;
    double slope;
    std::vector<int16_t> pixels;
};

#endif /* GATE_USE_DICOM */

#endif /* __GATEVDICOM_HH__ */


