add_library(samd21driver STATIC Pin.cpp Serial.cpp delay.cpp)
target_include_directories(samd21driver INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})