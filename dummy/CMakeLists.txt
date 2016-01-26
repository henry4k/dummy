include(FindPkgConfig)

file(GLOB_RECURSE Sources *.c *.cpp)
file(GLOB_RECURSE Headers *.h)

add_library(dummy ${LibraryType} ${Sources} ${Headers})
set_target_properties(dummy PROPERTIES PUBLIC_HEADERS ${Headers})
set_target_properties(dummy PROPERTIES MACOSX_RPATH ON)

install(TARGETS dummy
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/dummy)
