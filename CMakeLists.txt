cmake_minimum_required(VERSION 3.16)

project(Ascent VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#! [felgo-project]
find_package(Felgo REQUIRED)

set(PRODUCT_IDENTIFIER "com.merdak.ascent")
set(PRODUCT_VERSION_NAME "1.0.0")
set(PRODUCT_VERSION_CODE 1)
set(PRODUCT_STAGE "test")
set(PRODUCT_LICENSE_KEY "")

set(FELGO_PLUGINS
)
#! [felgo-project]

# QML/JS files are handed to the QML compiler, everything else ships as a resource.
file(GLOB_RECURSE QmlFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/*.qml qml/*.js)
file(GLOB_RECURSE AssetsFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} assets/* qml/*)
if(AssetsFiles AND QmlFiles)
  list(REMOVE_ITEM AssetsFiles ${QmlFiles})
endif()

add_subdirectory(src)

qt_add_executable(appAscent
    main.cpp
    ${QmlFiles}
    ${AssetsFiles}
)

felgo_configure_executable(appAscent)

# Development: copy QML next to the binary so edits do not need a rebuild.
# For publishing, comment this out and uncomment QML_FILES/RESOURCES below instead,
# which compiles the QML into the binary.
deploy_resources("${QmlFiles};${AssetsFiles}")

qt_add_qml_module(appAscent
    URI Ascent
    VERSION 1.0
# uncomment for publishing:
#    QML_FILES ${QmlFiles}
#    RESOURCES ${AssetsFiles}
    NO_RESOURCE_TARGET_PATH
)

target_compile_definitions(appAscent
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(appAscent PRIVATE AscentCore Felgo)

# The tutorial is generated on demand: cmake --build . --target docs
# It is kept out of the default build so a documentation warning cannot break a
# code build. To have it generated along with the project, tick "docs" next to
# "all" under Build Settings > Build Steps in the Qt Creator.
get_target_property(QtQmakeExecutable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QtBinDir "${QtQmakeExecutable}" DIRECTORY)
find_program(QDOC_EXECUTABLE NAMES qdoc HINTS "${QtBinDir}")

if(QDOC_EXECUTABLE)
  add_custom_target(docs
    COMMAND ${QDOC_EXECUTABLE} ascent.qdocconf
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
    COMMENT "Generating the tutorial with qdoc"
    VERBATIM)
endif()

# The core logic is plain QtCore, so its tests run on the desktop host only.
if(NOT IOS AND NOT ANDROID)
  enable_testing()
  add_subdirectory(tests)
endif()
