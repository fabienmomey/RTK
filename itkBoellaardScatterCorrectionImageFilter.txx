#ifndef __itkBoellaardScatterCorrectionImageFilter_txx
#define __itkBoellaardScatterCorrectionImageFilter_txx

#include <itkImageRegionIteratorWithIndex.h>

namespace itk
{

template <class TInputImage, class TOutputImage>
BoellaardScatterCorrectionImageFilter<TInputImage, TOutputImage>
::BoellaardScatterCorrectionImageFilter():
m_AirThreshold(32000),
m_ScatterToPrimaryRatio(0.),
m_NonNegativityConstraintThreshold(20)
{
}
  
  /**
 * Requires full projection images to estimate scatter.
 */
template <class TInputImage, class TOutputImage>
void
BoellaardScatterCorrectionImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  typename Superclass::OutputImagePointer outputPtr = this->GetOutput();
  if ( !outputPtr )
    return;

  const unsigned int Dimension = TInputImage::ImageDimension;
  typename TOutputImage::RegionType orr = outputPtr->GetRequestedRegion();
  typename TOutputImage::RegionType lpr = outputPtr->GetLargestPossibleRegion();

  for(unsigned int i=0; i<Dimension-1; i++)
    {
    orr.SetIndex( i, lpr.GetIndex(i) );
    orr.SetSize( i, lpr.GetSize(i) );
    }

  outputPtr->SetRequestedRegion( orr );
}

template <class TInputImage, class TOutputImage>
void
BoellaardScatterCorrectionImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId )
{
  // Input / ouput iterators
  itk::ImageRegionConstIterator<InputImageType> itIn(this->GetInput(), outputRegionForThread);
  itk::ImageRegionIterator<OutputImageType>     itOut(this->GetOutput(), outputRegionForThread);

  const unsigned int Dimension = TInputImage::ImageDimension;
  unsigned int npixelPerSlice = 1;
  for(unsigned int i=0; i<Dimension-1; i++)
    npixelPerSlice *= outputRegionForThread.GetSize(i);

  for(unsigned int slice=0; slice<outputRegionForThread.GetSize(Dimension-1); slice++)
    {
    itk::ImageRegionConstIterator<InputImageType> itInSlice = itIn;

    // Retrieve useful characteristics of current slice
    double averageBehindPatient = 0.;
    unsigned int npixelBehindPatient = 0;
    double smallestValue = itk::NumericTraits<double>::max();
    for(unsigned int i=0; i<npixelPerSlice; i++)
      {
      smallestValue = std::min(smallestValue, (double)itInSlice.Get() );
      if(itInSlice.Get()>=m_AirThreshold)
        {
        averageBehindPatient += itInSlice.Get();
        npixelBehindPatient++;
        }
      ++itInSlice;
      }
    if(npixelBehindPatient)
      averageBehindPatient /= npixelBehindPatient;

    // Compute constant correction
    double correction = averageBehindPatient * m_ScatterToPrimaryRatio;

    // Apply non-negativity constraint
    if(smallestValue-correction<m_NonNegativityConstraintThreshold)
      correction = smallestValue - m_NonNegativityConstraintThreshold;

    // Remove constant factor
    for(unsigned int i=0; i<npixelPerSlice; i++)
      {
      itOut.Set( itIn.Get() - correction );
      ++itIn;
      ++itOut;
      }
    }
}

template <class TInputImage, class TOutputImage>
int
BoellaardScatterCorrectionImageFilter<TInputImage, TOutputImage>
::SplitRequestedRegion(int i, int num, OutputImageRegionType& splitRegion)
{
  // Get the output pointer
  OutputImageType * outputPtr = this->GetOutput();
  const typename TOutputImage::SizeType& requestedRegionSize
    = outputPtr->GetRequestedRegion().GetSize();

  int splitAxis;
  typename TOutputImage::IndexType splitIndex;
  typename TOutputImage::SizeType splitSize;

  // Initialize the splitRegion to the output requested region
  splitRegion = outputPtr->GetRequestedRegion();
  splitIndex = splitRegion.GetIndex();
  splitSize = splitRegion.GetSize();

  // split on the outermost dimension available
  splitAxis = outputPtr->GetImageDimension() - 1;
  if (requestedRegionSize[splitAxis] == 1)
    { // cannot split
    itkDebugMacro("  Cannot Split");
    return 1;
    }

  // determine the actual number of pieces that will be generated
  typename TOutputImage::SizeType::SizeValueType range = requestedRegionSize[splitAxis];
  int valuesPerThread = Math::Ceil<int>(range/(double)num);
  int maxThreadIdUsed = Math::Ceil<int>(range/(double)valuesPerThread) - 1;

  // Split the region
  if (i < maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
    }
  if (i == maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    // last thread needs to process the "rest" dimension being split
    splitSize[splitAxis] = splitSize[splitAxis] - i*valuesPerThread;
    }

  // set the split region ivars
  splitRegion.SetIndex( splitIndex );
  splitRegion.SetSize( splitSize );

  itkDebugMacro("  Split Piece: " << splitRegion );

  return maxThreadIdUsed + 1;
}

} // end namespace itk
#endif