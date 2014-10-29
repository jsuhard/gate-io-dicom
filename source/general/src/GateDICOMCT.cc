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

#include "GateConfiguration.h"
#ifdef GATE_USE_DICOM

#include "GateDICOMCT.hh"
#include "GateMessageManager.hh"

#include "gdcmImageWriter.h"
#include "gdcmFilename.h"
#include "gdcmDataSet.h"
#include "gdcmAttribute.h"
#include "gdcmIPPSorter.h"
#include "gdcmReader.h"

GateDICOMCT::GateDICOMCT() : GateDICOMCTSlice()
{
}

GateDICOMCT::~GateDICOMCT() {
}

bool GateDICOMCT::Read(const std::string fileName)
{
    GateDICOMCTSlice::Read(fileName);

    gdcm::Directory::FilenamesType files;

    std::set<gdcm::Tag> tags;
    tags.insert(gdcm::Tag(0x0008, 0x0016));
    tags.insert(gdcm::Tag(0x0020, 0x000e));

    std::string path = gdcm::Filename(fileName.c_str()).GetPath();
    if(path == "")  path = ".";
    GateMessage("Image",2,"DICOM Path: " << path << " SeriesInstanceUID: <" << getSeriesInstanceUID().c_str() << ">" << G4endl);

    {
        gdcm::Directory directory;
        directory.Load(path);
        const gdcm::Directory::FilenamesType filesInDirectory = directory.GetFilenames();
        gdcm::Directory::FilenamesType::const_iterator it;
        for (it = filesInDirectory.begin(); it < filesInDirectory.end(); it++)
        {
            gdcm::Reader reader;
            reader.SetFileName((*it).c_str());
            reader.ReadSelectedTags(tags);
            const gdcm::DataSet &ds = reader.GetFile().GetDataSet();

            gdcm::Attribute<0x0008,0x0016> attributeSOPClassUID;
            attributeSOPClassUID.SetFromDataSet( ds );
            if(SOPClassUID != attributeSOPClassUID.GetValue().c_str()) {
                continue;
            }

            gdcm::Attribute<0x0020,0x000e> attributeSeriesInstanceUID;
            attributeSeriesInstanceUID.SetFromDataSet( ds );
            if(getSeriesInstanceUID() != attributeSeriesInstanceUID.GetValue()) {
                continue;
            }

            files.push_back(*it);
        }
    }

    if(files.size() > 1) {
        gdcm::IPPSorter ippsorter;
        ippsorter.SetComputeZSpacing( true );
        if(ippsorter.Sort(files)) {
            files = ippsorter.GetFilenames();
            setZSpacing(ippsorter.GetZSpacing());

            GateDICOMCTSlice slice;
            slice.Read(files.front());
            GateDICOMCTSlice sliceLast;
            sliceLast.Read(files.back());

            if(getZSpacing() == 0.0) {
                setZSpacing(fabs(slice.getZOrigin() - sliceLast.getZOrigin()) / (files.size()-1));
            }

        }else {
            GateWarning("Could not sort CT slice with Image Position Patient" << G4endl << "First: " << files.front() << G4endl << "Last: " << files.back());
        }
    }else if(files.empty()) {
        GateError("Can not find slices in " << path << " directory for SeriesInstanceUID:" << getSeriesInstanceUID());
        return false;
    }

    {        
        GateDICOMCTSlice slice;
        std::string file = files.front();
        GateMessage("Image",3,"Fetch Z origin from first DICOM CT Slice " << file << G4endl);
        slice.Read(file);
        setZOrigin(slice.getZOrigin());
        if(getZSpacing() == 0.0) {  setZSpacing(slice.getZSpacing()); }
    }

    unsigned int offset = getPixelsCount(); // x * y * 1
    setZSize(files.size());

    pixels.resize(offset*getZSize()); // x * y * z

    std::vector<int16_t> buffer(offset);

    for (size_t i = 0; i < getZSize(); i++) {

        GateDICOMCTSlice * slice = new GateDICOMCTSlice;
        std::string file = files.at(i);
        GateMessage("Image",4,"Processing DICOM CT Slice " << file << G4endl);
        if( !slice->Read(file) ) {
            delete slice;
            continue;
        }
        slice->getPixels(buffer);
        delete slice;

        std::copy(buffer.begin(), buffer.begin() + offset, pixels.begin() + i * offset);
        
    }

    buffer.clear();

    return true;
}

bool GateDICOMCT::Write(const std::string fileName)
{
    GateError("GateDICOMCT::Write is not (yet) implemented for " << fileName);
    return false;
}

#endif /* GATE_USE_DICOM */

