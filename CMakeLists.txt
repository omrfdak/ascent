cmake_minimum_required(VERSION 3.16)

# Qt 6.8's own minimum. Set here rather than left blank because it also becomes
# LSMinimumSystemVersion, and an empty value there is a bundle that tells macOS
# nothing about what it can run on.
if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
endif()

project(Ascent VERSION 1.0.0 LANGUAGES CXX)

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

qt_add_executable(Ascent
    main.cpp
    ${QmlFiles}
    ${AssetsFiles}
)

# Picks up macx/app_icon.icns and ios/Assets.xcassets on its own, so the icons
# are wired up by being where Felgo looks for them.
felgo_configure_executable(Ascent)

if(APPLE AND NOT IOS)
  # Felgo copies the icon into the bundle but names it in a variable of its own
  # scope, which leaves CFBundleIconFile empty: the file is there and the Dock
  # never looks at it. The property is what ends up in Info.plist.
  #
  # The rest of these are the same story from the other end. Left alone, the
  # bundle describes itself as version 0.1 under an identifier taken from the
  # login name, and Launch Services is entitled to distrust a bundle that
  # contradicts what the project says about itself.
  set_target_properties(Ascent PROPERTIES
    MACOSX_BUNDLE_ICON_FILE app_icon.icns
    MACOSX_BUNDLE_BUNDLE_NAME "Ascent"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${PRODUCT_IDENTIFIER}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PRODUCT_VERSION_NAME}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PRODUCT_VERSION_CODE}")
endif()

if(IOS)
  # The screen iOS shows before the first frame. Qt writes the plist key for it,
  # which is why the storyboard is named here instead of just being copied in.
  set_target_properties(Ascent PROPERTIES
    QT_IOS_LAUNCH_SCREEN "${CMAKE_CURRENT_SOURCE_DIR}/ios/LaunchScreen.storyboard")
endif()

# Development: copy QML next to the binary so edits do not need a rebuild.
# For publishing, comment this out and uncomment QML_FILES/RESOURCES below instead,
# which compiles the QML into the binary.
deploy_resources("${QmlFiles};${AssetsFiles}")

qt_add_qml_module(Ascent
    URI Ascent
    VERSION 1.0
# uncomment for publishing:
#    QML_FILES ${QmlFiles}
#    RESOURCES ${AssetsFiles}
    NO_RESOURCE_TARGET_PATH
)

target_compile_definitions(Ascent
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(Ascent PRIVATE AscentCore Felgo)

if(APPLE AND NOT IOS)
  # The linker signs the executable ad hoc, and everything copied into the bundle
  # afterwards - the icon, the QML, the assets - lands outside that signature.
  # macOS then reads the bundle as one whose resources have gone missing and
  # declines to trust it, which it shows by refusing to use its icon.
  #
  # Registered after the copy steps so it is the last thing that runs. Editing
  # the deployed QML by hand invalidates it again, which costs nothing but the
  # icon until the next build.
  add_custom_command(TARGET Ascent POST_BUILD
    COMMAND codesign --force --sign - "$<TARGET_BUNDLE_DIR:Ascent>"
    COMMENT "Signing the bundle so macOS accepts its resources"
    VERBATIM)
endif()

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
