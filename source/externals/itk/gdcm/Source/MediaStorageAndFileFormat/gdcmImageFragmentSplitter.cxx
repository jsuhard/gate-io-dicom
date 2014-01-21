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
#include "gdcmImageFragmentSplitter.h"
#include "gdcmSequenceOfFragments.h"

namespace gdcm
{

bool ImageFragmentSplitter::Split()
{
  Output = Input;
  const Pixmap &image = *Input;

  const unsigned int *dims = image.GetDimensions();
  if( dims[2] != 1 )
    {
    gdcmDebugMacro( "Cannot split a 3D image" );
    return false;
    }
  const DataElement& pixeldata = image.GetDataElement();

  const SequenceOfFragments *sqf = pixeldata.GetSequenceOfFragments();
  if( !sqf )
    {
    gdcmDebugMacro( "Cannot split a non-encapsulated syntax" );
    return false;
    }

  if ( sqf->GetNumberOfFragments() != 1 )
    {
    gdcmDebugMacro( "Case not handled (for now)" );
    return false;
    }

  //assert( sqf->GetNumberOfFragments() == 1 );

  // WARNING do not keep the same Basic Offset Table...
  const Fragment& frag = sqf->GetFragment(0);
  const ByteValue *bv = frag.GetByteValue();
  const char *p = bv->GetPointer();
  size_t len = bv->GetLength();
  if( FragmentSizeMax > len && !Force )
    {
    // I think it is ok
    return true;
    }
  // prevent zero division
  if( FragmentSizeMax == 0 )
    {
    gdcmDebugMacro( "Need to set a real value for fragment size" );
    return false; // seriously...
    }
  size_t nfrags = len / FragmentSizeMax;
  size_t lastfrag = len % FragmentSizeMax;

  SmartPointer<SequenceOfFragments> sq = new SequenceOfFragments;
  // Let's do all complete frag:
  for(size_t i = 0; i < nfrags; ++i)
    {
    Fragment splitfrag;
    splitfrag.SetByteValue( p + i * FragmentSizeMax, FragmentSizeMax);
    sq->AddFragment( splitfrag );
    }
  // Last (incomplete one):
  if( lastfrag )
    {
    Fragment splitfrag;
    splitfrag.SetByteValue( p + nfrags * FragmentSizeMax, (uint32_t)lastfrag );
    assert( nfrags * FragmentSizeMax + lastfrag == len );
    sq->AddFragment( splitfrag );
    }
  Output->GetDataElement().SetValue( *sq );

  bool success = true;
  return success;
}

void ImageFragmentSplitter::SetFragmentSizeMax(unsigned int fragsize)
{
/*
 * A.4 TRANSFER SYNTAXES FOR ENCAPSULATION OF ENCODED PIXEL DATA
 *
 * All items containing an encoded fragment shall be made of an even number of bytes
 * greater or equal to two. The last fragment of a frame may be padded, if necessary,
 * to meet the sequence item format requirements of the DICOM Standard.
 */
  FragmentSizeMax = fragsize;
  if( fragsize % 2 )
    {
    // what if FragmentSizeMax == 0 ...
    FragmentSizeMax--;
    }
  // How do I handle this one...
  if( fragsize < 2 )
    {
    FragmentSizeMax = 2;
    }
  // \postcondition:
  assert( FragmentSizeMax >= 2 && (FragmentSizeMax % 2) == 0 );
}

} // end namespace gdcm
