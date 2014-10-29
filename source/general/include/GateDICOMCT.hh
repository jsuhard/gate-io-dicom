/**
 * \class GateDICOMCT
 * \details Represents a 3D serie of multiple 2D DICOM images.
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

#ifndef __GATEDICOMCT_HH__
#define __GATEDICOMCT_HH__

#include "GateConfiguration.h"

#ifdef GATE_USE_DICOM

#include "GateDICOMCTSlice.hh"

class GateDICOMCT : public GateDICOMCTSlice {

public:
    GateDICOMCT();
    ~GateDICOMCT();
    bool Read(const std::string fileName);
    bool Write(const std::string fileName);
};

#endif /* GATE_USE_DICOM */

#endif /* __GATEDICOMCT_HH__ */


