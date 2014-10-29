/**
 * \class GateDICOMCT
 * \details Represents a 3D Dose distribution.
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

#ifndef __GATEDICOMRTDOSE_HH__
#define __GATEDICOMRTDOSE_HH__

#include "GateConfiguration.h"

#ifdef GATE_USE_DICOM

#include "GateVDICOM.hh"

class GateDICOMRTDose : public GateVDICOM {
    
public:
    GateDICOMRTDose();
    ~GateDICOMRTDose();
    void setPixels(const std::vector<float> & pixels);
    void getPixels(      std::vector<float> & output);
    void setIntercept(double) { intercept = 0.0; }
    double getIntercept() { return 0.0; }
    bool Read(const std::string fileName);
    bool Write(const std::string fileName);
    
private:    
    static const std::string SOPClassUID;           // 0008,0016
    static const std::string modality;              // 0008,0060
};

#endif /* GATE_USE_DICOM */

#endif /* __GATEDICOMRTDOSE_HH__ */


