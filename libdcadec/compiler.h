// I changed the files on June 26 2020 and June 27 2020
/*
 * This file is part of libdcadec.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once
#define restrict    __restrict

#ifndef __GNUC__
#define __attribute__(x)
#endif

#ifndef HAVE_BIGENDIAN
# if (defined __GNUC__)
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define HAVE_BIGENDIAN   0
#  elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define HAVE_BIGENDIAN   1
#  else
#   error Unsupported byte order
#  endif
# elif (defined _MSC_VER)
#  define HAVE_BIGENDIAN    0
# else
#  error Unsupported compiler. Define HAVE_BIGENDIAN macro to specify endianness.
# endif
#endif