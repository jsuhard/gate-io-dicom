/**
 * \class GateDICOMCTSlice
 * \details Represents a single DICOM 2D image.
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

#ifndef __GATEDICOMCTSLICE_HH__
#define __GATEDICOMCTSLICE_HH__

#include "GateConfiguration.h"

#ifdef GATE_USE_DICOM

#include "GateVDICOM.hh"
#include "gdcmDataSet.h"

class GateDICOMCTSlice : public GateVDICOM {

public:
    GateDICOMCTSlice();
    ~GateDICOMCTSlice();
    bool Read(const std::string fileName);
    bool ReadDemographics(const std::string fileName);
    bool Write(const std::string fileName);
    void setInstanceNumber(int instanceNumber) { this->instanceNumber = instanceNumber; }
    int getInstanceNumber() { return instanceNumber; }

protected:
    void setDemographicsFromDataSet(const gdcm::DataSet& ds);

    int instanceNumber;
    std::string patientPosition;
    static const std::string SOPClassUID;           // 0008,0016
    static const std::string modality;              // 0008,0060
};

#endif /* GATE_USE_DICOM */

#endif /* __GATEDICOMCTSLICE_HH__ */


