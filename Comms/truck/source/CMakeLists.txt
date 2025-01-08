idf_build_get_property(target IDF_TARGET)

# if(${target} STREQUAL "linux")
#     set(requires esp_event esp_stubs protocol_examples_common nvs_flash)
# endif()

# set(tcp_client_ip tcp_client_v4.cpp)

# if("${CONFIG_EXAMPLE_IPV4}" STREQUAL y)
#     set(tcp_client_ip tcp_client_v4.c)
# else()
#     set(tcp_client_ip tcp_client_v6.c)
# endif()

# idf_component_register(SRCS "Tcp_Trial.cpp" "PathDataHandler.cpp" "TcpClient.cpp" "tcp_client_main.cpp" "${tcp_client_ip}"
#                                 INCLUDE_DIRS "."
#                                 REQUIRES ${requires})

# idf_component_register(SRCS "TcpClient.cpp" "tcp_client_main.cpp" 
#                                 INCLUDE_DIRS "."
#                                 REQUIRES ${requires})


idf_component_register(SRCS "data_transfer.c" "tcp_client_v4.c" "tcp_client_main.c" "${tcp_client_ip}"
                                INCLUDE_DIRS "."
                                REQUIRES ${requires})
