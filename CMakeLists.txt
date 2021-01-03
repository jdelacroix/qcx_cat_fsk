cmake_minimum_required(VERSION 3.0)

project(qcx_cat_fsk VERSION 0.1.0)

find_package(PulseAudio REQUIRED)
find_package(Boost COMPONENTS log log_setup program_options REQUIRED)

add_definitions(-DBOOST_LOG_DYN_LINK)

configure_file(include/qcx_cat_fsk/version.h.in include/qcx_cat_fsk/version.h)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    ${PULSEAUDIO_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

add_executable(qcx_cat_ft8
    src/qcx_cat_fsk/audio_decoder.cc
    src/qcx_cat_fsk/cat_interface.cc
    src/main.cc)

set_property(TARGET qcx_cat_ft8 PROPERTY CXX_STANDARD 17)


target_link_libraries(qcx_cat_ft8
    ${PULSEAUDIO_LIBRARY} pulse-simple ${Boost_LIBRARIES} pthread)
