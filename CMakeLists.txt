cmake_minimum_required (VERSION 3.9.6)
project( DtsDecoder CXX )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

set( MAIN
	decoderLibrary.h
	DtsDecoder.cpp
	DtsDecoder.h
	stdafx.h
)

set( COMLIGHT
	ComLight/comLightClient.h
	ComLight/comLightCommon.h
	ComLight/comLightServer.h
	ComLight/Exception.hpp
	ComLight/hresult.h
	ComLight/unknwn.h
)

set( DECODER
	libdcadec/bitstream.cpp
	libdcadec/bitstream.h
	libdcadec/common.h
	libdcadec/compiler.h
	libdcadec/core_decoder.cpp
	libdcadec/core_decoder.h
	libdcadec/core_huffman.h
	libdcadec/core_tables.h
	libdcadec/core_vectors.h
	libdcadec/dca_context.cpp
	libdcadec/dca_context.h
	libdcadec/dca_frame.cpp
	libdcadec/dca_frame.h
	libdcadec/dmix_tables.cpp
	libdcadec/dmix_tables.h
	libdcadec/errorCodes.h
	libdcadec/exss_parser.cpp
	libdcadec/exss_parser.h
	libdcadec/fir_fixed.h
	libdcadec/fir_float.h
	libdcadec/fixed_math.h
	libdcadec/huffman.h
	libdcadec/idct.h
	libdcadec/idct_fixed.cpp
	libdcadec/idct_float.cpp
	libdcadec/interpolator.cpp
	libdcadec/interpolator.h
	libdcadec/interpolator_fixed.cpp
	libdcadec/interpolator_float.cpp
	libdcadec/lbr_bitstream.h
	libdcadec/lbr_decoder.cpp
	libdcadec/lbr_decoder.h
	libdcadec/lbr_huffman.h
	libdcadec/lbr_tables.h
	libdcadec/memory.hpp
	libdcadec/xll_decoder.cpp
	libdcadec/xll_decoder.h
	libdcadec/xll_tables.h
)

set( UTILS
	Utils/ApplyVolume.hpp
	Utils/ChannelsData.hpp
	Utils/interleaveChannels.hpp
	Utils/loadSamples.hpp
	Utils/simdTypes-sse.hpp
	Utils/store3-sse.hpp
)

add_library( DtsDecoder SHARED ${DECODER} ${UTILS} ${COMLIGHT} ${MAIN} )

set_target_properties( DtsDecoder PROPERTIES
	CXX_VISIBILITY_PRESET hidden # -fvisibility=hidden
	C_VISIBILITY_PRESET hidden # -fvisibility=hidden
	VISIBILITY_INLINES_HIDDEN TRUE
	POSITION_INDEPENDENT_CODE ON
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
# Would love to enable INTERPROCEDURAL_OPTIMIZATION, unfortunately GCC bug blocks that:
# https://gcc.gnu.org/legacy-ml/gcc-patches/2016-01/msg00248.html

target_compile_definitions( DtsDecoder PUBLIC _M_ARM )
target_compile_options( DtsDecoder PUBLIC -Wall -Wno-psabi -Wno-comment -march=native -mfpu=neon -O3 -fPIC -Wl,--no-undefined )
target_link_libraries( DtsDecoder -Wl,--no-undefined )