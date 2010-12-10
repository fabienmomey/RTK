#ifndef __itkElektaSynergyRawToAttenuationImageFilter_txx
#define __itkElektaSynergyRawToAttenuationImageFilter_txx

#include <itkImageFileWriter.h>

namespace itk
{

template <class TInputImage, class TOutputImage>
ElektaSynergyRawToAttenuationImageFilter<TInputImage, TOutputImage>
::ElektaSynergyRawToAttenuationImageFilter()
{
  m_LutFilter = LutFilterType::New();
  m_CropFilter = CropFilterType::New();

  //Permanent internal connections
  m_CropFilter->SetInput(m_LutFilter->GetOutput());

  //Default filter parameters
  typename CropFilterType::SizeType border = m_CropFilter->GetLowerBoundaryCropSize();
  border[0] = 4;
  m_CropFilter->SetBoundaryCropSize(border);
}

template<class TInputImage, class TOutputImage>
void
ElektaSynergyRawToAttenuationImageFilter<TInputImage, TOutputImage>
::GenerateOutputInformation()
{
  m_LutFilter->SetInput(this->GetInput());
  m_CropFilter->UpdateOutputInformation();
  this->GetOutput()->SetLargestPossibleRegion( m_CropFilter->GetOutput()->GetLargestPossibleRegion() );
}
  
template<class TInputImage, class TOutputImage>
void
ElektaSynergyRawToAttenuationImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputs();
  m_LutFilter->SetInput(this->GetInput());
  m_CropFilter->Update();
  this->GraftOutput( m_CropFilter->GetOutput() );
}

} // end namespace itk
#endif
