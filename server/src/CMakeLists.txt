add_executable(TankServer server/Server.cpp) 
set_target_properties(TankServer PROPERTIES
    CXX_STANDARD 17
    DEBUG_POSTFIX "-debug"
    COMPILE_OPTIONS ${TANK_SERVER_COMPILE_FLAGS}
) 
target_include_directories(TankServer PRIVATE include protos)
target_link_libraries(TankServer PRIVATE Dependencies Protos)
install(TARGETS TankServer)

add_subdirectory(protos)

