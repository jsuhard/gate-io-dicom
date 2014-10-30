
file(GLOB_RECURSE dicoms data/images/*.dcm.md5)

foreach(filename ${dicoms})
  string(LENGTH ${filename} filename_length)
  math(EXPR filename_length "${filename_length} - 4")
  string(SUBSTRING ${filename} 0 ${filename_length} filename)
  GateAddExampleData("DATA{${filename}}")
endforeach()
