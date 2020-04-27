#----------------------------------------------------------------------------

set(MODULES base geant fibers_stack tools)

foreach(MOD ${MODULES})
    add_subdirectory(${MOD})
endforeach()
