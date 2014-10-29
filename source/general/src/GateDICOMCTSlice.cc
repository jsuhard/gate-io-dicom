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

#include "GateConfiguration.h"
#ifdef GATE_USE_DICOM

#include "GateDICOMCTSlice.hh"
#include "GateMessageManager.hh"

const std::string GateDICOMCTSlice::SOPClassUID = "1.2.840.10008.5.1.4.1.1.2";
const std::string GateDICOMCTSlice::modality = "CT";

#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "gdcmAttribute.h"

GateDICOMCTSlice::GateDICOMCTSlice() : GateVDICOM()
{
}

GateDICOMCTSlice::~GateDICOMCTSlice() {
}

bool GateDICOMCTSlice::ReadDemographics(const std::string fileName)
{
    gdcm::Reader ir;
    ir.SetFileName( fileName.c_str() );
    if(!ir.Read())
    {
        GateError("Can not read " << fileName << " as a DICOM image.");
        return false;
    }

    setDemographicsFromDataSet(ir.GetFile().GetDataSet());

    return true;
}

bool GateDICOMCTSlice::Read(const std::string fileName)
{
    gdcm::ImageReader ir;
    ir.SetFileName( fileName.c_str() );
    if(!ir.Read())
    {
        GateError("Can not read " << fileName << " as a DICOM image.");
        return false;
    }
    gdcm::Image &image = ir.GetImage();
    setXSize(image.GetDimension(0));
    setYSize(image.GetDimension(1));
    try {
        setZSize(image.GetDimension(2));
    } catch (std::exception) {
        setZSize(1);
    }
    setXOrigin(image.GetOrigin(0));
    setYOrigin(image.GetOrigin(1));
    setZOrigin(image.GetOrigin(2));
    setXSpacing(image.GetSpacing(0));
    setYSpacing(image.GetSpacing(1));
    setIntercept(image.GetIntercept());
    setSlope(image.GetSlope());
    std::vector<float> imageOrientationPatient(6);
    for(size_t i = 0; i < 6; i++) {
        imageOrientationPatient[i] = image.GetDirectionCosines(i);
    }
    tags.setImageOrientationPatient(imageOrientationPatient);

    gdcm::DataSet& ds = ir.GetFile().GetDataSet();

    {
        gdcm::Attribute<0x18,0x50> at;
        at.SetFromDataSet( ds );
        setZSpacing(at.GetValue());
    }

    setDemographicsFromDataSet(ds);

    unsigned long bufferLength = image.GetBufferLength();

    const gdcm::ByteValue *bv = ds.GetDataElement( gdcm::Tag(0x7fe0,0x0010) ).GetByteValue();

    pixels.resize(bufferLength);
    bv->GetBuffer(reinterpret_cast<char*>(&pixels.front()), bufferLength);

    return true;
}

void GateDICOMCTSlice::setDemographicsFromDataSet(const gdcm::DataSet& ds)
{
    {
        gdcm::Attribute<0x10,0x10> at;
        at.SetFromDataSet( ds );
        tags.setPatientName(at.GetValue());
    }
    {
        gdcm::Attribute<0x10,0x20> at;
        at.SetFromDataSet( ds );
        tags.setPatientID(at.GetValue());
    }
    {
        gdcm::Attribute<0x10,0x30> at;
        at.SetFromDataSet( ds );
        tags.setPatientDateOfBirth(at.GetValue());
    }
    {
        gdcm::Attribute<0x10,0x40> at;
        at.SetFromDataSet( ds );
        tags.setPatientSex(at.GetValue());
    }
    {
        gdcm::Attribute<0x18,0x5100> at;
        at.SetFromDataSet( ds );
        tags.setPatientPosition(at.GetValue());
    }
    {
        gdcm::Attribute<0x20,0xd> at;
        at.SetFromDataSet( ds );
        tags.setStudyInstanceUID(at.GetValue());
    }
    {
        gdcm::Attribute<0x20,0xe> at;
        at.SetFromDataSet( ds );
        setSeriesInstanceUID(at.GetValue());
    }
    {
        gdcm::Attribute<0x20,0x13> at;
        at.SetFromDataSet( ds );
        setInstanceNumber(at.GetValue());
    }
    {
        gdcm::Attribute<0x20,0x52> at;
        at.SetFromDataSet( ds );
        tags.setFrameOfReferenceUID(at.GetValue());
    }
}
                       
bool GateDICOMCTSlice::Write(const std::string fileName)
{
    GateError("GateDICOMCTSlice::Write is not defined. Do nothing with " << fileName);
    return false;
}

#endif /* GATE_USE_DICOM */

