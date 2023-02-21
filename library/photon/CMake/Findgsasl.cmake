find_path(GSASL_INCLUDE_DIRS gsasl.h)

find_library(GSASL_LIBRARIES gsasl)

find_package_handle_standard_args(gsasl DEFAULT_MSG GSASL_LIBRARIES GSASL_INCLUDE_DIRS)

mark_as_advanced(GSASL_INCLUDE_DIRS GSASL_LIBRARIES)