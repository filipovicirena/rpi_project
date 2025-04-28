# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/motion_cam_gui_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/motion_cam_gui_autogen.dir/ParseCache.txt"
  "motion_cam_gui_autogen"
  )
endif()
