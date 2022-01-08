add_library(animation 
    src/CAnimationManager.cpp
    src/ScopedAnimationId.cpp)
target_include_directories(animation PRIVATE ${CMAKE_SOURCE_DIR}/animation/include)