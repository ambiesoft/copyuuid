cmake_minimum_required(VERSION 3.10.2)
project(copyuuid)


if(MSVC)
  add_definitions( -DUNICODE -D_UNICODE )
  
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")

endif()

include_directories(
  copyuuid
  ../lsMisc/stlsoft/include
)

add_executable(copyuuid WIN32
  copyuuid/copyuuid.cpp
  ../lsMisc/CommandLineParser.cpp
  ../lsMisc/SetClipboardText.cpp
  ../lsMisc/OpenCommon.cpp
  ../lsMisc/I18N.cpp
  ../lsMisc/UTF16toUTF8.cpp
  ../lsMisc/UrlEncode.cpp
  ../lsMisc/stdwin32/stdwin32.cpp

)

