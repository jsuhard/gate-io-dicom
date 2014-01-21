/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2010 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmRAWCodec.h"
#include "gdcmTransferSyntax.h"
#include "gdcmByteSwap.txx"
#include "gdcmDataElement.h"
#include "gdcmSequenceOfFragments.h"
#include "gdcmUnpacker12Bits.h"

#include <sstream>

namespace gdcm
{

class RAWInternals
{
public:
};

RAWCodec::RAWCodec()
{
  Internals = new RAWInternals;
}

RAWCodec::~RAWCodec()
{
  delete Internals;
}

bool RAWCodec::CanCode(TransferSyntax const &ts) const
{
  return ts == TransferSyntax::ImplicitVRLittleEndian
   || ts == TransferSyntax::ExplicitVRLittleEndian
   || ts == TransferSyntax::ExplicitVRBigEndian
   || ts == TransferSyntax::ImplicitVRBigEndianPrivateGE
   || ts == TransferSyntax::DeflatedExplicitVRLittleEndian;
}

bool RAWCodec::CanDecode(TransferSyntax const &ts) const
{
  return ts == TransferSyntax::ImplicitVRLittleEndian
   || ts == TransferSyntax::ExplicitVRLittleEndian
   || ts == TransferSyntax::ExplicitVRBigEndian
   || ts == TransferSyntax::ImplicitVRBigEndianPrivateGE
   || ts == TransferSyntax::DeflatedExplicitVRLittleEndian;
}

bool RAWCodec::Code(DataElement const &in, DataElement &out)
{
  out = in;
  //assert(0);
  return true;
}

bool RAWCodec::Decode(DataElement const &in, DataElement &out)
{
  // First let's see if we can do a fast-path:
  if( !NeedByteSwap &&
    !RequestPaddedCompositePixelCode &&
    PI == PhotometricInterpretation::MONOCHROME2 &&
    !PlanarConfiguration && !RequestPlanarConfiguration &&
    GetPixelFormat().GetBitsAllocated() != 12 &&
    !NeedOverlayCleanup )
    {
    assert( this->GetPixelFormat() != PixelFormat::UINT12 );
    assert( this->GetPixelFormat() != PixelFormat::INT12 );
    out = in;
    return true;
    }
  // else
  const ByteValue *bv = in.GetByteValue();
  assert( bv );
  std::stringstream is;
  is.write(bv->GetPointer(), bv->GetLength());
  std::stringstream os;
  bool r = DecodeByStreams(is, os);
  if(!r) return false;
  assert( r );

  std::string str = os.str();
  //std::string::size_type check = str.size();

  out = in;

  if( this->GetPixelFormat() == PixelFormat::UINT12 ||
    this->GetPixelFormat() == PixelFormat::INT12 )
    {
    size_t len = str.size() * 16 / 12;
    char * copy = new char[len];//why use an array, and not a vector?
    bool b = Unpacker12Bits::Unpack(copy, &str[0], str.size() );
    assert( b );
    (void)b;
    VL::Type lenSize = (VL::Type)len;
    out.SetByteValue( copy, lenSize );
    delete[] copy;

    this->GetPixelFormat().SetBitsAllocated( 16 );
    }
  else
    {
      VL::Type strSize = (VL::Type) str.size();
    out.SetByteValue( &str[0], strSize);
    }

  return r;
}

bool RAWCodec::DecodeByStreams(std::istream &is, std::ostream &os)
{
  bool r = ImageCodec::DecodeByStreams(is, os);
  return r;
}

bool RAWCodec::GetHeaderInfo(std::istream &, TransferSyntax &ts)
{
  ts = gdcm::TransferSyntax::ExplicitVRLittleEndian;
  if( NeedByteSwap )
    {
    ts = gdcm::TransferSyntax::ImplicitVRBigEndianPrivateGE;
    }
  return true;
}

} // end namespace gdcm
