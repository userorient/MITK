
project(org_mitk_gui_qt_preprocessing_resampling)

mitk_create_plugin(
  EXPORT_DIRECTIVE PREPROCESSING_RESAMPLING_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkMapperExt MitkImageDenoising
  PACKAGE_DEPENDS ITK|ITKMathematicalMorphology
)
