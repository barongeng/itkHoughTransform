/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "itkFEMElement2DC0LinearLineStress.h"

namespace itk {
namespace fem {

Element2DC0LinearLineStress
::Element2DC0LinearLineStress() : Superclass()
{
}

Element2DC0LinearLineStress
::Element2DC0LinearLineStress(
      NodeIDType n1_,
      NodeIDType n2_,
      Material::ConstPointer m_) : Superclass()
{
  // Set the geometrical points
  this->SetNode( 0, n1_ );
  this->SetNode( 1, n2_ );

  /*
   * Initialize the pointer to material object and check that
   * we were given the pointer to the right class.
   * If the material class was incorrect an exception is thrown.
   */
  if( (m_mat=dynamic_cast<const MaterialLinearElasticity*>(&*m_)) == 0 )
  {
    throw FEMExceptionWrongClass(__FILE__,__LINE__,"Element2DC0LinearLineStress::Element2DC0LinearLineStress()");
  }
}

void
Element2DC0LinearLineStress
::GetMassMatrix( MatrixType& Me ) const
{
  Me.set_size(4,4);
  Me.fill(0.0);

  Float l=(m_node[1]->GetCoordinates()-m_node[0]->GetCoordinates()).magnitude();

  Me[0][0]=2.0;
  Me[1][1]=2.0;
  Me[2][2]=2.0;
  Me[3][3]=2.0;
  Me[0][2]=1.0;
  Me[1][3]=1.0;
  Me[2][0]=1.0;
  Me[3][1]=1.0;

  Me=Me * (l*m_mat->RhoC*m_mat->A/6.0);

}

FEM_CLASS_REGISTER(Element2DC0LinearLineStress)
}} // end namespace itk::fem
