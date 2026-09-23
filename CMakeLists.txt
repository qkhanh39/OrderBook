cmake_minimum_required(VERSION 3.20)

project(orderbook
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(orderbook
    app/main.cpp
    src/OrderBook.cpp
    src/Order.cpp
    src/PriceLevel.cpp
)

target_include_directories(orderbook
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)