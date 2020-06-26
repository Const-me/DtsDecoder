#pragma once

constexpr int DCADEC_EINVAL = 1;   /**< Invalid argument */
constexpr int DCADEC_EBADDATA = 2;   /**< Invalid bitstream format */
constexpr int DCADEC_EBADCRC = 3;   /**< CRC check failed */
constexpr int DCADEC_EBADREAD = 4;   /**< Bitstream navigation error */
constexpr int DCADEC_ENOSYNC = 5;   /**< Synchronization error */
constexpr int DCADEC_ENOSUP = 6;   /**< Unsupported feature */
constexpr int DCADEC_ENOMEM = 7;   /**< Memory allocation error */
constexpr int DCADEC_EOVERFLOW = 8;   /**< PCM output overflow */
constexpr int DCADEC_EIO = 9;   /**< I/O error */
constexpr int DCADEC_EOUTCHG = 10; /**< PCM output parameters changed */
constexpr int DCADEC_EFAIL = 32;   /**< Unspecified error */

constexpr int DCADEC_WCOREAUXFAILED = 1;   /**< Failed to parse core auxiliary data */
constexpr int DCADEC_WCOREEXTFAILED = 2;   /**< Failed to parse core extension */
constexpr int DCADEC_WEXSSFAILED = 3;   /**< Failed to parse EXSS */
constexpr int DCADEC_WXLLFAILED = 4;   /**< Failed to parse XLL */
constexpr int DCADEC_WXLLSYNCERR = 5;   /**< XLL synchronization error */
constexpr int DCADEC_WXLLBANDERR = 6;   /**< XLL frequency band error */
constexpr int DCADEC_WXLLCONFERR = 7;   /**< XLL configuration error */
constexpr int DCADEC_WXLLCLIPPED = 8;   /**< Clipping detected in XLL output */
constexpr int DCADEC_WXLLLOSSY = 9;   /**< XLL output not lossless */