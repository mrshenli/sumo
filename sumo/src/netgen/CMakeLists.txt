set(netgenerate_SRCS
   netgen_main.cpp
   NGEdge.cpp
   NGEdge.h
   NGFrame.cpp
   NGFrame.h
   NGNet.h
   NGNet.cpp
   NGNode.cpp
   NGNode.h
   NGRandomNetBuilder.h
   NGRandomNetBuilder.cpp
)

add_executable(netgenerate ${netgenerate_SRCS})

target_link_libraries(netgenerate netbuild netwrite ${GDAL_LIBRARY} ${commonlibs})

install(TARGETS netgenerate DESTINATION bin)
