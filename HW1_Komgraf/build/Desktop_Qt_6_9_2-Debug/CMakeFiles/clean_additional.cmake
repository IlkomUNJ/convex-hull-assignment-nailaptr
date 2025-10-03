# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/HW1_Komgraf_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/HW1_Komgraf_autogen.dir/ParseCache.txt"
  "HW1_Komgraf_autogen"
  )
endif()
