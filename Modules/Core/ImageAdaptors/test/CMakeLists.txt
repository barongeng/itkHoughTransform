itk_module_test()
set(ITK-ImageAdaptorsTests
itkImageAdaptorTest.cxx
itkImageAdaptorsHeaderTest.cxx
itkRGBToVectorImageAdaptorTest.cxx
)

CreateTestDriver(ITK-ImageAdaptors  "${ITK-ImageAdaptors-Test_LIBRARIES}" "${ITK-ImageAdaptorsTests}")

add_test(NAME itkImageAdaptorsHeaderTest
      COMMAND ITK-ImageAdaptorsTestDriver itkImageAdaptorsHeaderTest)
add_test(NAME itkImageAdaptorTest
      COMMAND ITK-ImageAdaptorsTestDriver itkImageAdaptorTest)
add_test(NAME itkRGBToVectorImageAdaptorTest
      COMMAND ITK-ImageAdaptorsTestDriver itkRGBToVectorImageAdaptorTest)
