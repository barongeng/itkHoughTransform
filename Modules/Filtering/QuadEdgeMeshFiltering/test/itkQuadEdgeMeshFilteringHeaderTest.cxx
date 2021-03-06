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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkBorderQuadEdgeMeshFilter.txx"
#include "itkDiscreteCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscretePrincipalCurvaturesQuadEdgeMeshFilter.h"
#include "itkSmoothingQuadEdgeMeshFilter.txx"
#include "itkDiscreteCurvatureTensorQuadEdgeMeshFilter.h"
#include "itkDelaunayConformingQuadEdgeMeshFilter.txx"
#include "itkQuadEdgeMeshDecimationQuadricElementHelper.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkNormalQuadEdgeMeshFilter.txx"
#include "itkParameterizationQuadEdgeMeshFilter.txx"
#include "itkDiscreteMeanCurvatureQuadEdgeMeshFilter.h"
#include "itkDecimationQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter.txx"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.txx"
#include "itkCleanQuadEdgeMeshFilter.h"
#include "itkEdgeDecimationQuadEdgeMeshFilter.txx"
#include "itkDiscreteMinimumCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscreteGaussianCurvatureQuadEdgeMeshFilter.h"
#include "itkParameterizationQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkQuadricDecimationQuadEdgeMeshFilter.h"
#include "itkDiscreteMaximumCurvatureQuadEdgeMeshFilter.h"



int itkQuadEdgeMeshFilteringHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
