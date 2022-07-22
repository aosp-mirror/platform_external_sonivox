cmake_minimum_required(VERSION 3.1)

project( sonivox 
    LANGUAGES C
    VERSION 3.6.10.14
)

option(BUILD_SONIVOX_STATIC "Build the static library" TRUE)
option(BUILD_SONIVOX_SHARED "Build the shared library" TRUE)

include(GNUInstallDirs)

list(APPEND SOURCES
  arm-wt-22k/host_src/eas_config.c 
  arm-wt-22k/host_src/eas_hostmm.c 
#arm-wt-22k/host_src/eas_main.c 
  arm-wt-22k/host_src/eas_report.c
#arm-wt-22k/host_src/eas_wave.c
  arm-wt-22k/lib_src/eas_chorus.c
  arm-wt-22k/lib_src/eas_chorusdata.c
  arm-wt-22k/lib_src/eas_data.c 
  arm-wt-22k/lib_src/eas_dlssynth.c 
  arm-wt-22k/lib_src/eas_flog.c 
#arm-wt-22k/lib_src/eas_ima_tables.c 
#arm-wt-22k/lib_src/eas_imaadpcm.c 
#arm-wt-22k/lib_src/eas_imelody.c 
#arm-wt-22k/lib_src/eas_imelodydata.c 
  arm-wt-22k/lib_src/eas_math.c 
  arm-wt-22k/lib_src/eas_mdls.c 
  arm-wt-22k/lib_src/eas_midi.c 
  arm-wt-22k/lib_src/eas_mididata.c 
  arm-wt-22k/lib_src/eas_mixbuf.c
  arm-wt-22k/lib_src/eas_mixer.c
#arm-wt-22k/lib_src/eas_ota.c 
#arm-wt-22k/lib_src/eas_otadata.c 
  arm-wt-22k/lib_src/eas_pan.c 
  arm-wt-22k/lib_src/eas_pcm.c 
  arm-wt-22k/lib_src/eas_pcmdata.c 
  arm-wt-22k/lib_src/eas_public.c 
  arm-wt-22k/lib_src/eas_reverb.c 
  arm-wt-22k/lib_src/eas_reverbdata.c 
#arm-wt-22k/lib_src/eas_rtttl.c 
#arm-wt-22k/lib_src/eas_rtttldata.c 
  arm-wt-22k/lib_src/eas_smf.c 
  arm-wt-22k/lib_src/eas_smfdata.c 
  arm-wt-22k/lib_src/eas_tcdata.c 
  arm-wt-22k/lib_src/eas_tonecontrol.c 
  arm-wt-22k/lib_src/eas_voicemgt.c 
#arm-wt-22k/lib_src/eas_wavefile.c 
#arm-wt-22k/lib_src/eas_wavefiledata.c 
  arm-wt-22k/lib_src/eas_wtengine.c 
  arm-wt-22k/lib_src/eas_wtsynth.c 
#arm-wt-22k/lib_src/eas_xmf.c 
#arm-wt-22k/lib_src/eas_xmfdata.c 
#arm-wt-22k/lib_src/jet.c 
  arm-wt-22k/lib_src/wt_22khz.c 
)

list(APPEND HEADERS 
    arm-wt-22k/host_src/eas.h
    arm-wt-22k/host_src/eas_chorus.h
    arm-wt-22k/host_src/eas_reverb.h
    arm-wt-22k/host_src/eas_types.h
#arm-wt-22k/host_src/jet.h
)

add_library( sonivox-objects OBJECT ${SOURCES} )

target_compile_definitions( sonivox-objects PRIVATE
  UNIFIED_DEBUG_MESSAGES
  EAS_WT_SYNTH
  NUM_OUTPUT_CHANNELS=2
  _SAMPLE_RATE_22050
  MAX_SYNTH_VOICES=64
  _16_BIT_SAMPLES
  _FILTER_ENABLED
  DLS_SYNTHESIZER
  _REVERB_ENABLED
  _CHORUS_ENABLED
  #_IMELODY_PARSER
  #_RTTTL_PARSER
  #_OTA_PARSER
  #_XMF_PARSER
  #JET_INTERFACE
)

target_include_directories( sonivox-objects PRIVATE
    arm-wt-22k/host_src
    arm-wt-22k/lib_src
    fakes
)

set_target_properties( sonivox-objects PROPERTIES POSITION_INDEPENDENT_CODE TRUE )

target_compile_options( sonivox-objects PRIVATE
    -Wno-unused-parameter
    -Wno-unused-value
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-misleading-indentation
    -Wno-attributes
)

if (BUILD_SONIVOX_STATIC)
    add_library( sonivox-static STATIC $<TARGET_OBJECTS:sonivox-objects> )
    target_include_directories( sonivox-static PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/arm-wt-22k/include/libsonivox>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sonivox>" )
    set_target_properties(sonivox-static PROPERTIES PUBLIC_HEADER "${HEADERS}")
    add_library( sonivox::sonivox-static ALIAS sonivox-static)
    list(APPEND SONIVOX_TARGETS sonivox-static)
endif()

if (BUILD_SONIVOX_SHARED)
    add_library( sonivox SHARED $<TARGET_OBJECTS:sonivox-objects> )
    target_include_directories( sonivox PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/arm-wt-22k/include/libsonivox>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sonivox>" )
    set_target_properties(sonivox PROPERTIES VERSION ${PROJECT_VERSION})
    set_target_properties(sonivox PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
    set_target_properties(sonivox PROPERTIES PUBLIC_HEADER "${HEADERS}")
    add_library( sonivox::sonivox ALIAS sonivox)
    list(APPEND SONIVOX_TARGETS sonivox)
endif()

if(IS_ABSOLUTE ${CMAKE_INSTALL_LIBDIR})
    set(sonivox_libdir "${CMAKE_INSTALL_LIBDIR}")
else()
    set(sonivox_libdir "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()
if(IS_ABSOLUTE ${CMAKE_INSTALL_INCLUDEDIR})
    set(sonivox_includedir "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    set(sonivox_includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

if (BUILD_SONIVOX_STATIC)
    configure_file(sonivox-static.pc.in ${PROJECT_BINARY_DIR}/sonivox-static.pc @ONLY)
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/sonivox-static.pc 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
endif()
if (BUILD_SONIVOX_SHARED)
    configure_file(sonivox.pc.in ${PROJECT_BINARY_DIR}/sonivox.pc @ONLY)
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/sonivox.pc 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
endif()

install( TARGETS ${SONIVOX_TARGETS}
    EXPORT sonivox-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sonivox
)

install( EXPORT sonivox-targets
	FILE ${PROJECT_NAME}-targets.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

export( EXPORT sonivox-targets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
	NAMESPACE ${PROJECT_NAME}::
)

include( CMakePackageConfigHelpers )

write_basic_package_version_file(
	${PROJECT_NAME}-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
	${PROJECT_NAME}-config.cmake.in
	${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

install( FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
