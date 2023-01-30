cmake_minimum_required(VERSION 3.24)
project(PMCSN C)

set(CMAKE_C_STANDARD 11)

include_directories(Code)
include_directories(Code/Logic)
include_directories(Code/Logic/FiniteHorizon)
include_directories(Code/Logic/InfiniteHorizon)
include_directories(Code/Utils)
include_directories(Code/Utils/Standard)

add_executable(PMCSN
        Code/Logic/FiniteHorizon/finite_horizon.c
        Code/Logic/FiniteHorizon/finite_horizon.h
        Code/Logic/InfiniteHorizon/infinite_horizon.c
        Code/Logic/InfiniteHorizon/infinite_horizon.h
        Code/Logic/logic.c
        Code/Logic/logic.h
        Code/Logic/structure.h
        Code/Logic/var.h
        Code/Utils/Standard/rngs.c
        Code/Utils/Standard/rngs.h
        Code/Utils/Standard/rvgs.c
        Code/Utils/Standard/rvgs.h
        Code/Utils/utils.c
        Code/Utils/utils.h
        Code/config.c
        Code/config.h
        Code/main.c)
