cmake_minimum_required(VERSION  2.8)
project(test_game_engine CXX)

set(GoEngineSrc "../../Implementation/")
add_library(
    GoEngine
    ${GoEngineSrc}/definitions.h
    ${GoEngineSrc}/GoEngine.cpp
    ${GoEngineSrc}/GoEngine.h
    ${GoEngineSrc}/Point.cpp
    ${GoEngineSrc}/Point.h
    ${GoEngineSrc}/State.cpp
    ${GoEngineSrc}/State.h
    ${GoEngineSrc}/Action.h
    ${GoEngineSrc}/Action.cpp
)
    
set(TestHelpersSrc "./")
enable_testing()

add_definitions(-DBOOST_TEST_DYN_LINK) 
set (BOOST_ROOT $ENV{boost_test})
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS} ${GoEngineSrc} ${TestHelpersSrc})

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} ${Boost_LIBRARIES} GoEngine)
        
        add_test(${testName} scoring_test)
endforeach(testSrc)

