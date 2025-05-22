cmake_minimum_required(VERSION 3.16)  
project(boost_http_server VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_DIR "D:/local/boost-1.88.0/stage/lib/cmake/Boost-1.88.0") #thay bằng vị trí folder chứa file cmake BoostConfig


find_package(Boost 1.88.0 CONFIG REQUIRED COMPONENTS filesystem system) 
include_directories(${CMAKE_SOURCE_DIR}/Router)
include_directories(${CMAKE_SOURCE_DIR}/app)
include_directories(${CMAKE_SOURCE_DIR}/http)

add_executable(boost_http_server 
main.cpp
Router/router.cpp
app/app.cpp
)


target_link_libraries(boost_http_server PRIVATE Boost::filesystem Boost::system)


target_include_directories(boost_http_server PRIVATE ${Boost_INCLUDE_DIRS})

