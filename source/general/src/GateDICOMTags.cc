/**
 * \class GateDICOMTags
 * \details Contains DICOM tags for easy sharing.
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

#include "GateDICOMTags.hh"
#include "GateMessageManager.hh"

GateDICOMTags::GateDICOMTags()
{
    patientName = "TEST^GATE";
    patientID = "testgate";
    patientDateOfBirth = "20130301";
    patientSex = "O";
    this->imageOrientationPatient.reserve(6);
    this->imageOrientationPatient.push_back(1);
    this->imageOrientationPatient.push_back(0);
    this->imageOrientationPatient.push_back(0);
    this->imageOrientationPatient.push_back(0);
    this->imageOrientationPatient.push_back(1);
    this->imageOrientationPatient.push_back(0);
}

GateDICOMTags::~GateDICOMTags()
{
    
}

void GateDICOMTags::setPatientSex(const std::string patientSex)
{
    if(patientSex.compare("M") || patientSex.compare("m")) {
        this->patientSex = "M";
    }else if(patientSex.compare("F") || patientSex.compare("f")) {
        this->patientSex = "F";
    }else {
        this->patientSex = "O";
    }
}

void GateDICOMTags::setImageOrientationPatient(const std::vector<float> & imageOrientationPatient)
{
    if(imageOrientationPatient.size() != 6) {
        GateError("imageOrientationPatient must contain only 6 elements");
    }else {
        this->imageOrientationPatient = imageOrientationPatient;
    }
}

std::ostream& operator<<(std::ostream& os, const GateDICOMTags& tags)
{
    os
    << "\tPatient Name      = " << tags.patientName << std::endl
    << "\tPatient ID        = " << tags.patientID << std::endl
    << "\tPatient Date Birth= " << tags.patientDateOfBirth << std::endl
    << "\tPatient Sex       = " << tags.patientSex << std::endl
    << "\tStudy Instance ID = " << tags.studyInstanceUID << std::endl
    << "\tImage Orientation = [" << tags.imageOrientationPatient[0] << "," << tags.imageOrientationPatient[1] << "," << tags.imageOrientationPatient[2] << "," << tags.imageOrientationPatient[3] << "," << tags.imageOrientationPatient[4] << "," << tags.imageOrientationPatient[5] << "]" << std::endl
    << "\tFrame Ref. ID     = " << tags.frameOfReferenceUID << std::endl
    ;
    return os;
}

#endif /* GATE_USE_DICOM */


