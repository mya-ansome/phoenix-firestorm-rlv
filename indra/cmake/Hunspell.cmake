# -*- cmake -*-
include(Prebuilt)

set(HUNSPELL_FIND_QUIETLY ON)
set(HUNSPELL_FIND_REQUIRED ON)

if (USESYSTEMLIBS)
  include(FindHUNSPELL)
else (USESYSTEMLIBS)
  use_prebuilt_binary(libhunspell)
  if (WINDOWS)
    set(HUNSPELL_LIBRARY libhunspell)
  elseif(DARWIN)
    set(HUNSPELL_LIBRARY hunspell-1.3)
  elseif(LINUX)
    set(HUNSPELL_LIBRARY libhunspell-1.3.a)
  else()
    message(FATAL_ERROR "Invalid platform")
  endif()
  set(HUNSPELL_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/hunspell)
  use_prebuilt_binary(dictionaries)
endif (USESYSTEMLIBS)
