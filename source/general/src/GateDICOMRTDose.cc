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

#include "GateConfiguration.h"
#ifdef GATE_USE_DICOM

#include "GateDICOMRTDose.hh"
#include "GateMessageManager.hh"

const std::string GateDICOMRTDose::SOPClassUID = "1.2.840.10008.5.1.4.1.1.481.2";
const std::string GateDICOMRTDose::modality = "RTDOSE";

#include "gdcmVersion.h"
#include "gdcmUIDGenerator.h"
#include "gdcmImageWriter.h"
#include "gdcmDataSet.h"
#include "gdcmAttribute.h"

GateDICOMRTDose::GateDICOMRTDose() : GateVDICOM()
{
}

GateDICOMRTDose::~GateDICOMRTDose() {
}

void GateDICOMRTDose::setPixels(const std::vector<float> & pixels)
{
    const unsigned int pixelsCount = getPixelsCount();

    double slope = *std::max_element(pixels.begin(),pixels.end()) / (double) (UINT16_MAX);
    setSlope(slope);

    this->pixels.resize(pixelsCount);

    for(unsigned int i = 0; i < pixelsCount; i++) {
        const uint16_t upixel = pixels[i] / slope;
        this->pixels[i] = upixel;
    }

}

void GateDICOMRTDose::getPixels(std::vector<float> & output)
{
    const unsigned int pixelsCount = getPixelsCount();
    output.resize(pixels.size());

    const double slope = getSlope();

    for(unsigned int i = 0; i < pixelsCount; i++) {
        const uint16_t upixel = pixels[i];
        output[i] = upixel * slope;
    }
}

bool GateDICOMRTDose::Read(const std::string fileName)
{
    GateError("GateDICOMRTDose::Read is not (yet) implemented for " << fileName);
    return false;
}

bool GateDICOMRTDose::Write(const std::string fileName)
{

    gdcm::ImageWriter w;

    gdcm::SmartPointer<gdcm::Image> im = new gdcm::Image;
    im->SetNumberOfDimensions( 3 );
    im->SetDimension(0, getXSize() );
    im->SetDimension(1, getYSize() );
    im->SetDimension(2, getZSize() );
    im->SetOrigin(0, getXOrigin() );
    im->SetOrigin(1, getYOrigin() );
    im->SetOrigin(2, getZOrigin() );
    im->SetSpacing(0, getXSpacing() );
    im->SetSpacing(1, getYSpacing() );
    im->SetSpacing(2, getZSpacing() );
    im->SetDirectionCosines(&tags.getImageOrientationPatient()[0]);

    /**
     * As GDCM < 2.2 has hard-coded values of which modalities are images and which ones, in our case RTDose, are not,
     * we put the slope value to 1 and give our right slope in <0x3004,0x000e> attribute.
     */
    if(gdcm::Version::GetMajorVersion() <= 2 && gdcm::Version::GetMinorVersion() <= 2) {
        im->SetSlope(1.0);
    }else {
        im->SetSlope(getSlope());
    }

    im->SetPixelFormat(gdcm::PixelFormat::UINT16);
    im->SetPhotometricInterpretation( gdcm::PhotometricInterpretation::MONOCHROME2 );
    im->SetTransferSyntax( gdcm::TransferSyntax::ExplicitVRLittleEndian );

    unsigned long bufferLength = im->GetBufferLength();
    gdcm::DataElement pixeldata( gdcm::Tag(0x7fe0,0x0010) );
    pixeldata.SetByteValue( reinterpret_cast<char*>(&pixels.front()), bufferLength);
    pixeldata.SetVR(gdcm::VR::OW);
    im->SetDataElement( pixeldata );

    w.SetImage( *im );

    gdcm::DataSet& ds = w.GetFile().GetDataSet();

    {
        gdcm::Attribute<0x8,0x16> at;
        at.SetValue( SOPClassUID );
        ds.Replace(at.GetAsDataElement());
    }
    {
        /*
         * SOP Instance UID : unique value in the *world*
         */
        gdcm::Attribute<0x8,0x18> at;
        at.SetValue( SOPInstanceUID );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x8,0x60> at;
        at.SetValue( modality );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x8,0x70> at;
        at.SetValue( manufacturer );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x8,0x1090> at;
        at.SetValue( manufacturerModelName );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x10,0x10> at;
        at.SetValue( tags.getPatientName() );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x10,0x20> at;
        at.SetValue( tags.getPatientID() );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x10,0x30> at;
        at.SetValue( tags.getPatientDateOfBirth() );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x10,0x40> at;
        at.SetValue( tags.getPatientSex() );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::DataElement data( gdcm::Tag(0x18,0x1020));
        data.SetByteValue(softwareVersion.c_str(), (uint32_t)softwareVersion.length());
        data.SetVR(gdcm::VR::LO);
        ds.Replace(data);
    }
    {
        /*
         * Study Instance UID
         */
        gdcm::Attribute<0x20,0xd> at;
        at.SetValue( tags.getStudyInstanceUID() );
        ds.Replace(at.GetAsDataElement());
    }
    {
        /*
         * Series Instance UID : unique value in the *world*
         */
        gdcm::Attribute<0x20,0xe> at;
        at.SetValue( seriesInstanceUID );
        ds.Replace(at.GetAsDataElement());
    }
    {
        /*
         * Instance Number
         * Must be keeped
         */
        gdcm::Attribute<0x20,0x13> at;
        at.SetValue( 1 );
        ds.Replace(at.GetAsDataElement());
    }
    {
        /*
         * Frame of Reference UID
         * All images in a Series that share the same Frame of Reference UID shall be spatially related to each other.
         */
        gdcm::Attribute<0x20,0x52> at;
        at.SetValue( tags.getFrameOfReferenceUID() );
        ds.Replace( at.GetAsDataElement() );
    }
    {
        gdcm::Attribute<0x3004,0x2> at;
        at.SetValue( "GY" );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x3004,0x4> at;
        at.SetValue( "PHYSICAL" );
        ds.Replace(at.GetAsDataElement());
    }
    {
        gdcm::Attribute<0x3004,0xa> at;
        at.SetValue( "PLAN" );
        ds.Replace( at.GetAsDataElement() );
    }
    {
        gdcm::Attribute<0x3004,0xe> at;
        at.SetValue( getSlope() );
        ds.Replace( at.GetAsDataElement() );
    }

    w.SetCheckFileMetaInformation( true);
    w.SetFileName( fileName.c_str() );

    if( !w.Write() )
    {
        GateError(" can not write " << fileName );
        return false;
    }

    return true;
}

#endif /* GATE_USE_DICOM */


