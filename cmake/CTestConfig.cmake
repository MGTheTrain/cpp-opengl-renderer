include(CTest)
enable_testing()

set(CTEST_PROJECT_NAME "${PROJECT_NAME}")
set(CTEST_NIGHTLY_START_TIME "00:00:00 UTC")
set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "code.bbraun.io/")
set(CTEST_DROP_LOCATION "Mgtt/cpp-opengl-renderer")
set(CTEST_DROP_SITE_CDASH TRUE)
set(CTEST_DROP_SITE_CDASH_BI TRUE)