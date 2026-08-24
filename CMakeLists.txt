cmake_minimum_required(VERSION 3.16)
project(OpenWordPad VERSION 1.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    PrintSupport
    Svg
)

set(SOURCES
    src/main.cpp
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

add_executable(openwordpad WIN32 ${SOURCES})

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
