itk_module_test()
set(ITK-ImageComposeTests
itkCompose2DCovariantVectorImageFilterTest.cxx
itkImageComposeHeaderTest.cxx
itkCompose3DVectorImageFilterTest.cxx
itkCompose3DCovariantVectorImageFilterTest.cxx
itkCompose2DVectorImageFilterTest.cxx
itkComposeRGBImageFilterTest.cxx
itkImageToVectorImageFilterTest.cxx
)

CreateTestDriver(ITK-ImageCompose  "${ITK-ImageCompose-Test_LIBRARIES}" "${ITK-ImageComposeTests}")

add_test(NAME itkImageComposeHeaderTest
      COMMAND ITK-ImageComposeTestDriver itkImageComposeHeaderTest)
add_test(NAME itkCompose2DCovariantVectorImageFilterTest
      COMMAND ITK-ImageComposeTestDriver itkCompose2DCovariantVectorImageFilterTest)
add_test(NAME itkCompose3DVectorImageFilterTest
      COMMAND ITK-ImageComposeTestDriver itkCompose3DVectorImageFilterTest)
add_test(NAME itkCompose3DCovariantVectorImageFilterTest
      COMMAND ITK-ImageComposeTestDriver itkCompose3DCovariantVectorImageFilterTest)
add_test(NAME itkCompose2DVectorImageFilterTest
      COMMAND ITK-ImageComposeTestDriver itkCompose2DVectorImageFilterTest)
add_test(NAME itkComposeRGBImageFilterTest
      COMMAND ITK-ImageComposeTestDriver itkComposeRGBImageFilterTest)
add_test(NAME itkImageToVectorImageFilterTest
      COMMAND ITK-ImageComposeTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ImageToVectorImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/ImageToVectorImageFilterTest.png
    itkImageToVectorImageFilterTest ${ITK_DATA_ROOT}/Input/Spots.png ${ITK_DATA_ROOT}/Input/SpotsInverted.png ${ITK_DATA_ROOT}/Input/SpotsInverted.png ${ITK_TEST_OUTPUT_DIR}/ImageToVectorImageFilterTest.png)
