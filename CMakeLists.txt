cmake_minimum_required(VERSION 3.9)

project(cpu_scale)

set(SRC ryzen_cpu_tuner.cc)

add_executable(ryzen_scaler ${SRC})

