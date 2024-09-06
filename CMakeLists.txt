cmake_minimum_required (VERSION 3.8)

project ("SimpleTgLogger")

add_library(SimpleTgLogger "./sources/simple_tg_logger.cpp")
target_link_libraries(SimpleTgLogger PUBLIC TgBot PUBLIC bsl)
target_include_directories(SimpleTgLogger PUBLIC "./include")