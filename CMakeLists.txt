cmake_minimum_required(VERSION 3.16)
project(OpenWordPad VERSION 1.4.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_definitions(OPENWORDPAD_VERSION="${PROJECT_VERSION}")

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    PrintSupport
    Svg
    Test
)

set(COMMON_UI_SOURCES
    src/core/Units.h
    src/core/miniz.h
    src/core/miniz_common.h
    src/core/miniz_export.h
    src/core/miniz_tdef.h
    src/core/miniz_tdef.c
    src/core/miniz_tinfl.h
    src/core/miniz_tinfl.c
    src/core/miniz_zip.h
    src/core/miniz_zip.c
    src/core/miniz.c
    src/core/RtfEngine.h
    src/core/RtfEngine.cpp
    src/core/DocxEngine.h
    src/core/DocxEngine.cpp
    src/core/DocumentManager.h
    src/core/DocumentManager.cpp
    src/core/TranslationManager.h
    src/core/TranslationManager.cpp
    src/ui/RibbonBar.h
    src/ui/RibbonBar.cpp
    src/ui/RulerWidget.h
    src/ui/RulerWidget.cpp
    src/ui/DocumentEditor.h
    src/ui/DocumentEditor.cpp
    src/ui/ZoomSliderWidget.h
    src/ui/ZoomSliderWidget.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/dialogs/AboutDialog.h
    src/ui/dialogs/AboutDialog.cpp
    src/ui/dialogs/PageSetupDialog.h
    src/ui/dialogs/PageSetupDialog.cpp
    src/ui/dialogs/ParagraphDialog.h
    src/ui/dialogs/ParagraphDialog.cpp
    src/ui/dialogs/DateTimeDialog.h
    src/ui/dialogs/DateTimeDialog.cpp
    src/ui/dialogs/FindReplaceDialog.h
    src/ui/dialogs/FindReplaceDialog.cpp
    src/ui/dialogs/ImageResizeDialog.h
    src/ui/dialogs/ImageResizeDialog.cpp
    src/ui/dialogs/PaintDrawingDialog.h
    src/ui/dialogs/PaintDrawingDialog.cpp
    resources/resources.qrc
)

set(SOURCES
    src/main.cpp
    ${COMMON_UI_SOURCES}
)

if(WIN32)
    list(APPEND SOURCES resources/app_icon.rc)
elseif(APPLE)
    set_source_files_properties(resources/icons/app_icon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    list(APPEND SOURCES resources/icons/app_icon.icns)
endif()

add_executable(openwordpad WIN32 MACOSX_BUNDLE ${SOURCES})

if(APPLE)
    set_target_properties(openwordpad PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "OpenWordPad"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.openwordpad.OpenWordPad"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "app_icon.icns"
    )
endif()

target_link_libraries(openwordpad PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Svg
)

target_include_directories(openwordpad PRIVATE
    src
    src/core
    src/ui
    src/ui/dialogs
)

if(MSVC)
    target_compile_options(openwordpad PRIVATE /W4 /O2)
else()
    target_compile_options(openwordpad PRIVATE -Wall -Wextra -O3)
endif()

add_executable(generate_screenshots
    tools/generate_screenshots.cpp
    ${COMMON_UI_SOURCES}
)
target_link_libraries(generate_screenshots PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Svg
)
target_include_directories(generate_screenshots PRIVATE
    src
    src/core
    src/ui
    src/ui/dialogs
)

add_executable(test_engine
    tests/test_engine.cpp
    src/core/Units.h
    src/core/miniz.h
    src/core/miniz_common.h
    src/core/miniz_export.h
    src/core/miniz_tdef.h
    src/core/miniz_tdef.c
    src/core/miniz_tinfl.h
    src/core/miniz_tinfl.c
    src/core/miniz_zip.h
    src/core/miniz_zip.c
    src/core/miniz.c
    src/core/RtfEngine.h
    src/core/RtfEngine.cpp
    src/core/DocxEngine.h
    src/core/DocxEngine.cpp
    src/core/DocumentManager.h
    src/core/DocumentManager.cpp
)

target_link_libraries(test_engine PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

target_include_directories(test_engine PRIVATE
    src
    src/core
)

enable_testing()
add_test(NAME EngineTests COMMAND test_engine)

add_executable(test_batch_100
    tests/test_batch_100.cpp
    src/core/Units.h
    src/core/miniz.h
    src/core/miniz_common.h
    src/core/miniz_export.h
    src/core/miniz_tdef.h
    src/core/miniz_tdef.c
    src/core/miniz_tinfl.h
    src/core/miniz_tinfl.c
    src/core/miniz_zip.h
    src/core/miniz_zip.c
    src/core/miniz.c
    src/core/RtfEngine.h
    src/core/RtfEngine.cpp
    src/core/DocxEngine.h
    src/core/DocxEngine.cpp
    src/core/DocumentManager.h
    src/core/DocumentManager.cpp
)

target_link_libraries(test_batch_100 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

target_include_directories(test_batch_100 PRIVATE
    src
    src/core
)

add_test(NAME Batch100Tests COMMAND test_batch_100)

add_executable(test_rtf_roundtrip
    tests/test_rtf_roundtrip.cpp
    src/core/Units.h
    src/core/miniz.h
    src/core/miniz_common.h
    src/core/miniz_export.h
    src/core/miniz_tdef.h
    src/core/miniz_tdef.c
    src/core/miniz_tinfl.h
    src/core/miniz_tinfl.c
    src/core/miniz_zip.h
    src/core/miniz_zip.c
    src/core/miniz.c
    src/core/RtfEngine.h
    src/core/RtfEngine.cpp
    src/core/DocxEngine.h
    src/core/DocxEngine.cpp
    src/core/DocumentManager.h
    src/core/DocumentManager.cpp
)

target_link_libraries(test_rtf_roundtrip PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

target_include_directories(test_rtf_roundtrip PRIVATE
    src
    src/core
)

add_test(NAME RtfRoundTripTests COMMAND test_rtf_roundtrip)

add_executable(verify_debugger_workflow
    tests/verify_debugger_workflow.cpp
    src/core/Units.h
    src/core/miniz.h
    src/core/miniz_common.h
    src/core/miniz_export.h
    src/core/miniz_tdef.h
    src/core/miniz_tdef.c
    src/core/miniz_tinfl.h
    src/core/miniz_tinfl.c
    src/core/miniz_zip.h
    src/core/miniz_zip.c
    src/core/miniz.c
    src/core/RtfEngine.h
    src/core/RtfEngine.cpp
    src/core/DocxEngine.h
    src/core/DocxEngine.cpp
    src/core/DocumentManager.h
    src/core/DocumentManager.cpp
)

target_link_libraries(verify_debugger_workflow PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

target_include_directories(verify_debugger_workflow PRIVATE
    src
    src/core
)

add_test(NAME DebuggerWorkflowTests COMMAND verify_debugger_workflow)

add_executable(test_translations
    tests/test_translations.cpp
    src/core/TranslationManager.h
    src/core/TranslationManager.cpp
    resources/resources.qrc
)

target_link_libraries(test_translations PRIVATE
    Qt6::Core
    Qt6::Test
)

target_include_directories(test_translations PRIVATE
    src
    src/core
)

add_test(NAME TranslationTests COMMAND test_translations)

# ==========================================
# CPack Packaging Configuration
# ==========================================
set(CPACK_PACKAGE_NAME "OpenWordPad")
set(CPACK_PACKAGE_VENDOR "Leonardo Corato")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenWordPad - Open Source WordPad recreation in Qt6")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenWordPad")

if(WIN32)
    set(CPACK_GENERATOR "WIX;ZIP")
    set(CPACK_WIX_UPGRADE_GUID "D98A76B2-3C4E-481A-B982-581F78A690B4")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "OpenWordPad")
    install(TARGETS openwordpad RUNTIME DESTINATION .)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;ZIP")
    set(CPACK_DMG_VOLUME_NAME "OpenWordPad")
    set(CPACK_DMG_FORMAT "UDZO")
    install(TARGETS openwordpad BUNDLE DESTINATION .)
endif()

include(CPack)
