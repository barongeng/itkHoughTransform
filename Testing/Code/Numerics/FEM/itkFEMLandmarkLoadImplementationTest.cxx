/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFEMLandmarkLoadImplementationTest.cxx
  Language:  C++
  Date: $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include <iostream>
#include "itkFEMLoadImplementationGenericLandmarkLoad.h"
#include "itkFEMElement2DC0LinearQuadrilateralMembrane.h"
#include "itkFEM.h"


//
int itkFEMLandmarkLoadImplementationTest(int, char*[])
{
    Solver S;
    S.InitializeLinearSystemWrapper();


    Node::Pointer n1 = Node::New();
    Element::VectorType pt(2);

    pt[0]=0.;
    pt[1]=0.;
    n1->SetCoordinates(pt);
    S.node.push_back(FEMP<Node>(&*n1));

    n1=Node::New();
    pt[0]=1.;
    pt[1]=1.;
    n1->SetCoordinates(pt);
    S.node.push_back(FEMP<Node>(&*n1));

    n1=Node::New();
    pt[0]=3.;
    pt[1]=2.;
    n1->SetCoordinates(pt);
    S.node.push_back(FEMP<Node>(&*n1));

    n1=Node::New();
    pt[0]=0.;
    pt[1]=3.;
    n1->SetCoordinates(pt);
    S.node.push_back(FEMP<Node>(&*n1));

    S.node.Renumber();

    std::cout << "Nodes\n";

    MaterialLinearElasticity::Pointer m;
    m=MaterialLinearElasticity::New();
    m->GN=0;
    m->E=30000.0;
    m->A=0.02;
    m->I=0.004;
    S.mat.push_back( FEMP<Material>(&*m) );

    std::cout << "Material\n";

    Element2DC0LinearQuadrilateralMembrane::Pointer e0 = Element2DC0LinearQuadrilateralMembrane::New();

    e0->GN=0;
    e0->SetNode(0, &*S.node.Find(0));
    e0->SetNode(1, &*S.node.Find(1));
    e0->SetNode(2, &*S.node.Find(2));
    e0->SetNode(3, &*S.node.Find(3));
    e0->m_mat=dynamic_cast<MaterialLinearElasticity*>(&*S.mat.Find(0));

    std::cout << "Element\n";

    LoadBC::Pointer l1 = LoadBC::New();
    l1->m_element = &*e0;
    l1->m_dof = 0;
    l1->m_value = vnl_vector<double>(1,0.0);
    S.load.push_back(FEMP<Load>(*&l1));

    std::cout << "BC\n";

    LoadLandmark::Pointer lm0 = LoadLandmark::New();
    lm0->eta = 0.01;
    lm0->m_pt = vnl_vector<double>(0.,0.);
    lm0->m_target = vnl_vector<double>(0.,1.);
    lm0->F = lm0->m_target / (lm0->eta*lm0->eta);
    lm0->el.push_back(&*e0);
    S.load.push_back(FEMP<Load>(&*lm0));

    std::cout << "Landmark\n";

    S.GenerateGFN();
    S.AssembleK();
    S.DecomposeK();
    S.AssembleF();

    std::cout << "Assembled\n";

    try 
      { 
      S.Solve();
      }
    catch (ExceptionObject &e) 
      {
      std::cerr << "Exception caught: " << e << "\n";
      return EXIT_FAILURE;
      }

    std::cout << "Test PASSED!\n";
    return EXIT_SUCCESS;
}


