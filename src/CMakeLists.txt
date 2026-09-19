find_package(Boost CONFIG REQUIRED COMPONENTS
    headers
    asio
    beast
    date_time
)

find_package(spdlog CONFIG REQUIRED)
find_package(MbedTLS CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(BURIED_SOURCES
    buried.cc
    buried_core.cc
    common/common_service.cc
    context/context.cc
    crypt/crypt.cc
    database/database.cc
    report/buried_report.cc
    report/http_report.cc
)

set(BURIED_PUBLIC_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/include
)

set(BURIED_PRIVATE_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/buried_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/buried_config.h
)

set(BURIED_LINK_LIBRARIES
    Boost::headers
    Boost::asio
    Boost::beast
    Boost::date_time
    spdlog::spdlog
    MbedTLS::mbedtls
    MbedTLS::mbedcrypto
    sqlite_orm::sqlite_orm
    nlohmann_json::nlohmann_json
    ws2_32
    iphlpapi
    winmm
)

if(BUILD_BURIED_STATIC_LIBS)
    add_library(Buried_static STATIC ${BURIED_SOURCES})

    target_include_directories(Buried_static
        PUBLIC
            ${BURIED_PUBLIC_INCLUDE_DIR}
        PRIVATE
            ${BURIED_PRIVATE_INCLUDE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(Buried_static
        PRIVATE
            ${BURIED_LINK_LIBRARIES}
    )
endif()

if(BUILD_BURIED_SHARED_LIBS)
    add_library(Buried_shared SHARED ${BURIED_SOURCES})

    target_include_directories(Buried_shared
        PUBLIC
            ${BURIED_PUBLIC_INCLUDE_DIR}
        PRIVATE
            ${BURIED_PRIVATE_INCLUDE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(Buried_shared
        PRIVATE
            ${BURIED_LINK_LIBRARIES}
    )
endif()