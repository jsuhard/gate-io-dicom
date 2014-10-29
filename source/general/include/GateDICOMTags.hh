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

#ifndef __GATEDICOMTAGS_HH__
#define __GATEDICOMTAGS_HH__

#include "GateConfiguration.h"

#ifdef GATE_USE_DICOM

#include <iostream>
#include <vector>

class GateDICOMTags
{
public:

    GateDICOMTags();
    ~GateDICOMTags();

    void setPatientName(const std::string patientName) { this->patientName = patientName; }
    const std::string getPatientName() const{ return patientName; }

    void setPatientID(const std::string patientID) { this->patientID = patientID; }
    const std::string getPatientID() const{ return patientID; }

    void setPatientDateOfBirth(const std::string patientDateOfBirth) { this->patientDateOfBirth = patientDateOfBirth; }
    const std::string getPatientDateOfBirth() const{ return patientDateOfBirth; }

    void setPatientSex(const std::string patientSex);
    const std::string getPatientSex() const{ return patientSex; }

    void setPatientPosition(const std::string patientPosition) { this->patientPosition = patientPosition; }
    const std::string getPatientPosition() { return patientPosition; }

    void setStudyInstanceUID(const std::string studyInstanceUID) { this->studyInstanceUID = studyInstanceUID; }
    const std::string getStudyInstanceUID() const{ return studyInstanceUID; }

    void setImageOrientationPatient(const std::vector<float> & imageOrientationPatient);
    const std::vector<float> & getImageOrientationPatient() const{ return imageOrientationPatient; }

    /*
     * All images in a Series that share the same Frame of Reference UID shall be spatially related to each other.
     */
    void setFrameOfReferenceUID(const std::string frameOfReferenceUID) { this->frameOfReferenceUID = frameOfReferenceUID; }
    const std::string getFrameOfReferenceUID() const{ return frameOfReferenceUID; }

    friend std::ostream& operator<<(std::ostream& os, const GateDICOMTags& tags);

private:
    std::string patientName;                        // 0010,0010
    std::string patientID;                          // 0010,0020
    std::string patientDateOfBirth;                 // 0010,0030
    std::string patientSex;                         // 0010,0040
    std::string patientPosition;                    // 0018,5100
    std::string studyInstanceUID;                   // 0020,000d
    std::vector<float> imageOrientationPatient;     // 0020,0037
    std::string frameOfReferenceUID;                // 0020,0052
};

#endif /* GATE_USE_DICOM */

#endif /* __GATEDICOMTAGS_HH__ */


