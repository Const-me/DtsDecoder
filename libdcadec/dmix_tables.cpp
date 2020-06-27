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

#include <stdint.h>
#include "dmix_tables.h"

const uint16_t dmix_table[ 241 ] = {
	   33,    35,    37,    39,    41,    44,    46,    49,
	   52,    55,    58,    62,    65,    69,    73,    78,
	   82,    87,    92,    98,   104,   110,   116,   123,
	  130,   138,   146,   155,   164,   174,   184,   195,
	  207,   219,   232,   246,   260,   276,   292,   309,
	  328,   347,   368,   389,   413,   437,   463,   490,
	  519,   550,   583,   617,   654,   693,   734,   777,
	  823,   872,   924,   978,  1036,  1066,  1098,  1130,
	 1163,  1197,  1232,  1268,  1305,  1343,  1382,  1422,
	 1464,  1506,  1550,  1596,  1642,  1690,  1740,  1790,
	 1843,  1896,  1952,  2009,  2068,  2128,  2190,  2254,
	 2320,  2388,  2457,  2529,  2603,  2679,  2757,  2838,
	 2920,  3006,  3093,  3184,  3277,  3372,  3471,  3572,
	 3677,  3784,  3894,  4008,  4125,  4246,  4370,  4497,
	 4629,  4764,  4903,  5046,  5193,  5345,  5501,  5662,
	 5827,  5912,  5997,  6084,  6172,  6262,  6353,  6445,
	 6538,  6633,  6729,  6827,  6925,  7026,  7128,  7231,
	 7336,  7442,  7550,  7659,  7771,  7883,  7997,  8113,
	 8231,  8350,  8471,  8594,  8719,  8845,  8973,  9103,
	 9235,  9369,  9505,  9643,  9783,  9924, 10068, 10214,
	10362, 10512, 10665, 10819, 10976, 11135, 11297, 11460,
	11627, 11795, 11966, 12139, 12315, 12494, 12675, 12859,
	13045, 13234, 13426, 13621, 13818, 14018, 14222, 14428,
	14637, 14849, 15064, 15283, 15504, 15729, 15957, 16188,
	16423, 16661, 16902, 17147, 17396, 17648, 17904, 18164,
	18427, 18694, 18965, 19240, 19519, 19802, 20089, 20380,
	20675, 20975, 21279, 21587, 21900, 22218, 22540, 22867,
	23170, 23534, 23875, 24221, 24573, 24929, 25290, 25657,
	26029, 26406, 26789, 27177, 27571, 27970, 28376, 28787,
	29205, 29628, 30057, 30493, 30935, 31383, 31838, 32300,
	32768
};

const uint32_t dmix_table_inv[ 201 ] = {
	6553600, 6186997, 5840902, 5514167, 5205710, 4914507, 4639593, 4380059,
	4135042, 3903731, 3685360, 3479204, 3284581, 3100844, 2927386, 2763630,
	2609035, 2463088, 2325305, 2195230, 2072430, 2013631, 1956500, 1900990,
	1847055, 1794651, 1743733, 1694260, 1646190, 1599484, 1554103, 1510010,
	1467168, 1425542, 1385096, 1345798, 1307615, 1270515, 1234468, 1199444,
	1165413, 1132348, 1100221, 1069005, 1038676, 1009206,  980573,  952752,
	 925721,  899456,  873937,  849141,  825049,  801641,  778897,  756798,
	 735326,  714463,  694193,  674497,  655360,  636766,  618700,  601146,
	 584090,  567518,  551417,  535772,  520571,  505801,  491451,  477507,
	 463959,  450796,  438006,  425579,  413504,  401772,  390373,  379297,
	 368536,  363270,  358080,  352964,  347920,  342949,  338049,  333219,
	 328458,  323765,  319139,  314579,  310084,  305654,  301287,  296982,
	 292739,  288556,  284433,  280369,  276363,  272414,  268522,  264685,
	 260904,  257176,  253501,  249879,  246309,  242790,  239321,  235901,
	 232531,  229208,  225933,  222705,  219523,  216386,  213295,  210247,
	 207243,  204282,  201363,  198486,  195650,  192855,  190099,  187383,
	 184706,  182066,  179465,  176901,  174373,  171882,  169426,  167005,
	 164619,  162267,  159948,  157663,  155410,  153190,  151001,  148844,
	 146717,  144621,  142554,  140517,  138510,  136531,  134580,  132657,
	 130762,  128893,  127052,  125236,  123447,  121683,  119944,  118231,
	 116541,  114876,  113235,  111617,  110022,  108450,  106901,  105373,
	 103868,  102383,  100921,   99479,   98057,   96656,   95275,   93914,
	  92682,   91249,   89946,   88660,   87394,   86145,   84914,   83701,
	  82505,   81326,   80164,   79019,   77890,   76777,   75680,   74598,
	  73533,   72482,   71446,   70425,   69419,   68427,   67450,   66486,
	  65536
};

const uint8_t dmix_primary_nch[ 7 ] = {
	1, 2, 2, 3, 3, 4, 4
};
