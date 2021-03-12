#----------------------------------------------------------------------------

set(MODULES base geant fibers tools)

foreach(MOD ${MODULES})
    add_subdirectory(${MOD})
endforeach()
