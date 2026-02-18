#include "i3GfxType.h"
#include "i3GfxCaps.h"

#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/compare/generic_icompare.h"

#include "i3Base/string/ext/atof.h"

#if defined( I3G_DX)
#include "i3GfxGlobalVariableDX.h"

struct I3_GPUTABLE_INFO
{
	char	m_szDesc[128];
	INT32	m_TexFillRate;
};

static I3_GPUTABLE_INFO		s_GPUTable[] =
{
	{	"GeForce 7200 GS2",				900	},
	{	"GeForce 7300 SE2",				900	},
	{	"GeForce FX 5600 XT",			940	},
	{	"GeForce FX 5200",				1000 },
	{	"GeForce PCX 5300",				1000 },
	{	"GeForce FX 5700 LE",			1000 },
	{	"Quadro 600",					1000 },
	{	"GeForce FX 5500",				1080 },
	{	"Quadro FX 500",				1080 },
	{	"Quadro FX 350",				1100 },
	{	"GeForce 6200",					1200 },
	{	"GeForce FX 5700 VE",			1200 },
	{	"GeForce 6600 LE",				1200 },
	{	"GeForce FX 5600",				1300 },
	{	"GeForce FX 5200 Ultra",		1300 },
	{	"GeForce 6200 TC2",				1400 },
	{	"GeForce 6500",					1400 },
	{	"GeForce 7100 GS2",				1400 },
	{	"GeForce Go 73002",				1400 },
	{	"GeForce FX 5600 Ultra",		1400 },
	{	"GeForce FX 5600 Ultra Rev.2",	1600 },
	{	"GeForce PCX 5750",				1700 },
	{	"GeForce FX 5700",				1700 },
	{	"GeForce 7300 LE2",				1800 },
	{	"GeForce Go 74002",				1800 },
	{	"GeForce FX 5700 Ultra",		1900 },
	{	"GeForce FX 5700 Ultra DDR3",	1900 },
	{	"GeForce 7300 GS2",				2200 },
	{	"GeForce 205",					2356 },
	{	"GeForce 6600",					2400 },
	{	"Quadro FX 1000",				2400 },
	{	"GeForce 6800 LE",				2600 },
	{	"GeForce 6800 XT",				2600 },
	{	"GeForce 7300 GT",				2800 },
	{	"GeForce 6600 DDR2",			2800 },
	{	"Quadro FX 550",				2880 },
	{	"GeForce 8400M G",				3200 },
	{	"GeForce 8400M GS",				3200 },
	{	"GeForce FX 5800",				3200 },
	{	"Quadro FX 2000",				3200 },
	{	"GeForce FX 5900 XT",			3200 },
	{	"GeForce FX 5900",				3200 },
	{	"Quadro FX 3000",				3200 },
	{	"Quadro FX 3000 G",				3200 },
	{	"GeForce 9300M G",				3200 },
	{	"Quadro FX 3500",				3290 },
	{	"GeForce PCX 5900",				3400 },
	{	"GeForce 8400 GS",				3600 },
	{	"GeForce 8500 GT",				3600 },
	{	"GeForce Go 7600",				3600 },
	{	"GeForce Go 7600 GS",			3600 },
	{	"GeForce 8400M GT",				3600 },
	{	"GeForce FX 5900 Ultra",			3600 },
	{	"GeForce FX 5950 Ultra",			3800 },
	{	"GeForce PCX 5950",				3800 },
	{	"GeForce 6800",					3900 },
	{	"GeForce 6600 GT",				4000 },
	{	"GeForce FX 5800 Ultra",			4000 },
	{	"GeForce 6600 XL",				4200 },
	{	"Quadro FX 560",					4200 },
	{	"Quadro FX 1400",				4200 },
	{	"GeForce 6800 GTO",				4200 },
	{	"GeForce 6800 GS",				4200 },
	{	"GeForce G 100",				4300 },
	{	"Quadro FX 4000",				4500 },
	{	"Quadro FX 4000 SDI",			4500 },
	{	"GeForce 210",					4712 },
	{	"GeForce 310",					4712 },
	{	"GeForce 7600 GS",				4800 },
	{	"GeForce 8600M GS",				4800 },
	{	"Quadro FX 3450",				5100 },
	{	"GeForce Go 7700",				5400 },
	{	"GeForce 6800 GT",				5600 },
	{	"GeForce GT 430",				5600 },
	{	"GeForce Go 7600 GT",			6000 },
	{	"GeForce 7800 GS",				6000 },
	{	"GeForce 6800 Ultra",			6400 },
	{	"GeForce Go 7800",				6400 },
	{	"GeForce 7600 GT",				6720 },
	{	"GeForce Go 7900 GS",			7500 },
	{	"Quadro FX 1500",				7500 },
	{	"GeForce 8600M GT",				7600 },
	{	"GeForce 9500M GS",				7600 },
	{	"GeForce 9600M GS",				7600 },
	{	"GeForce 9600M GT",				7600 },
	{	"GeForce 9650M GS",				7600 },
	{	"GeForce 315",					7600 },
	{	"GeForce 7600 GT 80nm",			7800 },
	{	"GeForce 7800 GT",				8000 },
	{	"GeForce 8600 GT",				8640 },
	{	"GeForce GT 120",				8800 },
	{	"GeForce 7900 GS",				9000 },
	{	"GeForce Go 7800 GTX",			9600 },
	{	"GeForce 8700M GT",				10000 },
	{	"GeForce GT 220",				10000 },
	{	"GeForce 7800 GTX",				10320 },
	{	"GeForce 9500 GT",				10400 },
	{	"GeForce 8600 GTS",				10800 },
	{	"GeForce 7900 GT",				10800 },
	{	"Quadro FX 4500",				11280 },
	{	"Quadro FX 4500X2",				11280 },
	{	"Quadro FX 4500 SDI",			11280 },
	{	"GeForce Go 7900 GTX",			12000 },
	{	"GeForce GT 130",				12000 },
	{	"GeForce GT 320",				12960 },
	{	"GeForce GT 330",				12960 },
	{	"GeForce GT 440",				12960 },
	{	"GeForce 7950 GT",				13200 },
	{	"GeForce 7800 GTX 512",			13200 },
	{	"GeForce Go 7950 GTX",			13800 },
	{	"GeForce 7900 GTO",				15600 },
	{	"GeForce 7900 GTX",				15600 },
	{	"GeForce GT 230 v.1",			15600 },
	{	"Quadro FX 5500",				16800 },
	{	"Quadro FX 5500 SDI",			16800 },
	{	"GeForce GT 240",				17600 },
	{	"GeForce GT 340",				17600 },
	{	"GeForce 9600 GT",				20800 },
	{	"GeForce GT 140",				20800 },
	{	"GeForce 8800 GTS",				24000 },
	{	"GeForce 7900 GX2",				24000 },
	{	"GeForce 7950 GX2",				24000 },
	{	"GeForce 8800M GTX",			24000 },
	{	"GeForce GT 230 v.2",			24000 },
	{	"GeForce GTS 450",				25060 },
	{	"GeForce 9600 GSO",				26400 },
	{	"GeForce GTX 465",				26710 },
	{	"GeForce GTX 460 SE",			31200 },
	{	"Quadro FX 4600",				32000 },
	{	"GeForce 8800 GT",				33600 },
	{	"GeForce 9800 GT",				33600 },
	{	"GeForce GTX 470",				34000 },
	{	"GeForce 8800 GTX",				36800 },
	{	"GeForce GTX 260",				36900 },
	{	"GeForce GTS 240",				37800 },
	{	"GeForce GTX 460",				37800 },
	{	"Quadro FX 5600",				38400 },
	{	"GeForce 9800 GX2",				38400 },
	{	"GeForce 8800 Ultra",			39168 },
	{	"GeForce GTX 260 Core 216",		41472 },
	{	"GeForce GTX 480",				42000 },
	{	"GeForce 9800 GTX",				43200 },
	{	"GeForce GTX 460 v2",			43570 },
	{	"GeForce 9800 GTX+",			47232 },
	{	"GeForce GTS 150",				47232 },
	{	"GeForce GTS 250",				47232 },
	{	"GeForce GTX 230",				48200 },
	{	"GeForce GTX 280",				48200 },
	{	"GeForce GTX 275",				50600 },
	{	"GeForce GTX 285",				51840 },
	{	"GeForce GTX 295",				51840 },
	{ "GeForce GTX 295", 		 51840 },
	{ "GeForce GT 445M", 		 51840 },
	{ "Quadro 2000M", 		 51840 },
	{ "Quadro K2000M", 		 51840 },
	{ "Quadro FX 5800", 		 51840 },
	{ "Quadro K1100M", 		 51840 },
	{ "GeForce GTX 260", 		 51840 },
	{ "Quadro 2000 D", 		 51840 },
	{ "GeForce GT 745A", 		 51840 },
	{ "GeForce GT 745M", 		 51840 },
	{ "GeForce 840M", 		 51840 },
	{ "GeForce GTX 280", 		 51840 },
	{ "GeForce GTX 275", 		 51840 },
	{ "GeForce GTX 285", 		 51840 },
	{ "GeForce GT 545", 		 51840 },
	{ "Quadro 2000D", 		 51840 },
	{ "GeForce GT 640", 		 51840 },
	{ "GeForce GT 650M", 		 51840 },
	{ "Quadro 3000M", 		 51840 },
	{ "Quadro 2000", 		 51840 },
	{ "Quadro 5010M", 		 51840 },
	{ "GeForce GTX 460M", 		 51840 },
	{ "Quadro K2100M", 		 51840 },
	{ "GeForce GT 750M", 		 51840 },
	{ "GeForce GTX 660M", 		 51840 },
	{ "GeForce GTS 450", 		 51840 },
	{ "GeForce GTX 560M", 		 51840 },
	{ "GeForce GT 740", 		 51840 },
	{ "Quadro K3000M", 		 51840 },
	{ "Quadro K2000D", 		 51840 },
	{ "Quadro K2000", 		 51840 },
	{ "GeForce GTX 760M", 		 51840 },
	{ "GeForce GT 755M", 		 51840 },
	{ "Quadro 4000M", 		 51840 },
	{ "GeForce GTX 650", 		 51840 },
	{ "GeForce GTX 645", 		 51840 },
	{ "GeForce GTX 550 Ti", 		 51840 },
	{ "GeForce GTX 670M", 		 51840 },
	{ "Quadro 4000", 		 51840 },
	{ "GeForce GTX 570M", 		 51840 },
	{ "GeForce GTX 470M", 		 51840 },
	{ "GeForce GTX 480M", 		 51840 },
	{ "GeForce GTX 765M", 		 51840 },
	{ "GeForce GTX 670MX", 		 51840 },
	{ "Quadro K4000M", 		 51840 },
	{ "GeForce GTX 745", 		 51840 },
	{ "GeForce GTX 555", 		 51840 },
	{ "GeForce GTX 460 SE", 		 51840 },
	{ "GeForce GTX 560 SE", 		 51840 },
	{ "GeForce GTX 675M", 		 51840 },
	{ "GeForce GTX 485M", 		 51840 },
	{ "Quadro K3100M", 		 51840 },
	{ "GeForce GTX 850M", 		 51840 },
	{ "GeForce GTX 580M", 		 51840 },
	{ "GeForce GT 645", 		 51840 },
	{ "GeForce GTX 675MX", 		 51840 },
	{ "GeForce GTX 460 v2", 		 51840 },
	{ "Quadro 5000", 		 51840 },
	{ "GeForce GTX 460", 		 51840 },
	{ "GeForce GTX 650 Ti", 		 51840 },
	{ "GeForce GTX 860M", 		 51840 },
	{ "Quadro K4000", 		 51840 },
	{ "Quadro K5000M", 		 51840 },
	{ "Quadro K4100M", 		 51840 },
	{ "GeForce GTX 770M", 		 51840 },
	{ "GeForce GTX 465", 		 51840 },
	{ "Quadro K5100M", 		 51840 },
	{ "GeForce GTX 560", 		 51840 },
	{ "GeForce GTX 750", 		 51840 },
	{ "Quadro 6000", 		 51840 },
	{ "GeForce GTX 870M", 		 51840 },
	{ "GeForce GTX 680M", 		 51840 },
	{ "GeForce GTX 560 Ti", 		 51840 },
	{ "GeForce GTX 650 Ti BOOST", 		 51840 },
	{ "GeForce GTX 470", 		 51840 },
	{ "GeForce GTX 750 Ti", 		 51840 },
	{ "Quadro K5000", 		 51840 },
	{ "GeForce GTX 590", 		 51840 },
	{ "GeForce GTX 660", 		 51840 },
	{ "GeForce GTX 680MX", 		 51840 },
	{ "GeForce GTX 480", 		 51840 },
	{ "GeForce GTX 780M", 		 51840 },
	{ "GeForce GTX 570", 		 51840 },
	{ "GeForce GTX 775M", 		 51840 },
	{ "GeForce GTX 760 Ti", 		 51840 },
	{ "GeForce GTX 660 Ti", 		 51840 },
	{ "GeForce GTX 880M", 		 51840 },
	{ "GeForce GTX 580", 		 51840 },
	{ "GeForce GTX 760", 		 51840 },
	{ "GeForce GTX 690", 		 51840 },
	{ "GeForce GTX 670", 		 51840 },
	{ "GeForce GTX 680", 		 51840 },
	{ "GeForce GTX 770", 		 51840 },
	{ "GeForce GTX TITAN Z", 		 51840 },
	{ "Quadro K6000", 		 51840 },
	{ "GeForce GTX Titan", 		 51840 },
	{ "GeForce GTX 780", 		 51840 },
	{ "GeForce GTX TITAN Black", 		 51840 },
	{ "GeForce GTX 780 Ti", 		 51840 },
	{ "GeForce GTX 960",			 72128 },		//Core Clock(1127) * TMU(64)
	{	"Radeon Xpress 200",				600 },
	{	"Radeon Xpress 1100",				600 },
	{	"Radeon Xpress 1150",				600 },
	{	"Radeon 9550 SE",					1000 },
	{	"Radeon 9550",						1000 },
	{	"Radeon 9500",						1100 },
	{	"Radeon 9600 SE",					1300 },
	{	"Radeon X300 SE",					1300 },
	{	"Radeon 9600",						1300 },
	{	"Radeon X300",						1300 },
	{	"Radeon 9800 SE 128Bit",			1300 },
	{	"Mobility Radeon 9600 Pro Turbo",	1332 },
	{	"Mobility Radeon X300",				1400 },
	{	"Radeon X1200 IGP",					1400 },
	{	"Radeon 9800 SE 256Bit",			1520 },
	{	"Radeon X550 SE",					1600 },
	{	"Radeon X1050",						1600 },
	{	"Radeon 9600 Pro EZ",				1600 },
	{	"Radeon X550",						1600 },
	{	"Mobility Radeon X600",				1600 },
	{	"Radeon 9600 Pro",					1600 },
	{	"Radeon X600 Pro",					1600 },
	{	"Mobility Radeon X1300",			1600 },
	{	"Radeon X1270 IGP",					1600 },
	{	"Radeon X1250 IGP",					1600 },
	{	"Mobility Radeon X2500",			1700 },
	{	"Mobility Radeon X1350",			1728 },
	{	"Mobility Radeon X1400",			1728 },
	{	"Mobility Radeon X1450",			1728 },
	{	"Mobility Radeon X1600",			1728 },
	{	"Radeon X1300",						1800 },
	{	"Mobility Radeon 9700",				1800 },
	{	"Mobility Radeon X1700",			1880 },
	{	"Radeon 9600 XT",					2000 },
	{	"Radeon X600 XT",					2000 },
	{	"Radeon X1300 XT",					2000 },
	{	"Radeon X1600 Pro",					2000 },
	{	"Radeon X1650",						2000 },
	{	"Radeon Xpress 1250",				2000 },
	{	"Radeon HD 2350",					2100 },
	{	"Radeon HD 2400 Pro",				2100 },
	{	"Radeon 9500 Pro",					2200 },
	{	"Radeon X1550",						2200 },
	{	"Radeon 9700",						2200 },
	{	"Radeon X1600 XT",					2360 },
	{	"Radeon X1300 Pro",					2400 },
	{	"Radeon X1650 Pro",					2400 },
	{	"Radeon 9800 128Bit",				2600 },
	{	"Radeon 9800",						2600 },
	{	"Radeon 9700 Pro",					2600 },
	{	"Mobility Radeon X700",				2800 },
	{	"Radeon HD 2400 XT",				2800 },
	{	"Mobility Radeon 9800",				2800 },
	{	"Radeon 9800 Pro 128Bit",			3040 },
	{	"Radeon 9800 Pro",					3040 },
	{	"Radeon 9800 Pro 256MB",			3040 },
	{	"Radeon X700",						3200 },
	{	"Radeon X1650 GT",					3200 },
	{	"Radeon 9800 XT",					3296 },
	{	"Radeon X700 Pro",					3400 },
	{	"Radeon X800 SE",					3400 },
	{	"Radeon X800 SE",					3400 },
	{	"Radeon X800 GTO 128",				4800 },
	{	"Mobility Radeon X700",			4800 },
	{	"Radeon X700 XT",					3800 },
	{	"Radeon X800 GT 128",				3800 },
	{	"Radeon X800 GT",					3800 },
	{	"Radeon X800 GT 256",				3800 },
	{	"Radeon X1650 XT",					4600 },
	{	"Radeon X800",						4700 },
	{	"Radeon HD 2600 PRO",				4800 },
	{	"Radeon X800 GTO 256",				4800 },
	{	"Mobility Radeon X1800",			5400 },
	{	"Radeon X800 Pro",					5700 },
	{	"Radeon X1800 GTO",					6000 },
	{	"Radeon X1950 GT",					6000 },
	{	"Radeon X850 Pro",					6240 },
	{	"Radeon X800 GTO²",				6400 },
	{	"Radeon X800 GTO-16",				6400 },
	{	"Radeon X800 XL",					6400 },
	{	"Radeon X800 XL",					6400 },
	{	"Radeon HD 2600 XT",				6800 },
	{	"Radeon HD 2600 X2",				6800 },
	{	"Radeon X1900 GT",					6900 },
	{	"Radeon X1950 Pro",					6900 },
	{	"Mobility Radeon X800 XT",			7680 },
	{	"Radeon X1900 All-In-Wonder",		8000 },
	{	"Radeon X800 XT",					8000 },
	{	"Radeon X1800 XL",					8000 },
	{	"Radeon X850 XT",					8320 },
	{	"Radeon X850 XT",					8320 },
	{	"Radeon X800 XT PE",				8320 },
	{	"Radeon X850 XT PE",				8640 },
	{	"Mobility Radeon X1800 XT",			8800 },
	{	"Radeon HD 2900 GT",				9600 },
	{	"Radeon HD 2900 Pro",				9600 },
	{	"Radeon X1900 XT",					10000 },
	{	"Radeon X1800 XT",					10000 },
	{	"Radeon X1950 XT 512",				10000 },
	{	"Radeon X1950 XT 256",				10000 },
	{	"Radeon X1900 XTX",					10400 },
	{	"Radeon X1950 XTX",					10400 },
	{	"Radeon HD 3850",				 	10700 },
	{	"Radeon HD 2900 XT",				11888 },
	{	"Radeon HD 3870",					12400 },
	{	"Radeon HD 4450",					5600 },
	{	"Radeon HD 4470",					7200 },
	{	"Radeon HD 4650",					19200 },
	{	"Radeon HD 4670",					20800 },
	{	"Radeon HD 4850",					24000 },
	{	"Radeon HD 4870",					27200 },
	{ "FirePro M40003", 		 27200 },
	{ "Radeon HD 5750", 		 27200 },
	{ "Radeon R7 250", 		 27200 },
	{ "FirePro M4000", 		 27200 },
	{ "Radeon HD 7560D + 7670 Dual", 		 27200 },
	{ "FirePro V5900", 		 27200 },
	{ "Radeon HD 4890", 		 27200 },
	{ "Radeon HD 7870M", 		 27200 },
	{ "Radeon R9 255", 		 27200 },
	{ "FirePro V7800", 		 27200 },
	{ "Radeon HD 7750", 		 27200 },
	{ "FirePro M6000 Mobility Pro", 		 27200 },
	{ "FirePro W600", 		 27200 },
	{ "Radeon HD 6770", 		 27200 },
	{ "Radeon HD 5770", 		 27200 },
	{ "Radeon HD 8670D + 7700 Dual", 		 27200 },
	{ "Radeon HD 8870M", 		 27200 },
	{ "Radeon HD 6900M", 		 27200 },
	{ "Radeon HD 6790", 		 27200 },
	{ "Radeon HD 5830", 		 27200 },
	{ "Radeon HD 7770", 		 27200 },
	{ "Radeon HD 6850", 		 27200 },
	{ "Radeon HD 5850", 		 27200 },
	{ "FirePro V8800", 		 27200 },
	{ "Radeon HD 6870", 		 27200 },
	{ "FirePro V9800", 		 27200 },
	{ "Radeon HD 5870", 		 27200 },
	{ "Radeon HD 5970", 		 27200 },
	{ "FirePro V7900", 		 27200 },
	{ "FirePro M6100 FireGL V", 		 27200 },
	{ "Radeon R7 260", 		 27200 },
	{ "FirePro W5000", 		 27200 },
	{ "Radeon HD 7790", 		 27200 },
	{ "Radeon R7 260X", 		 27200 },
	{ "Radeon HD 6950", 		 27200 },
	{ "Radeon HD 6990", 		 27200 },
	{ "Radeon HD 6970", 		 27200 },
	{ "Radeon HD 8970M", 		 27200 },
	{ "Radeon HD 7850", 		 27200 },
	{ "Radeon HD 7970M", 		 27200 },
	{ "FirePro W7000", 		 27200 },
	{ "FirePro W7000 Adapter", 		 27200 },
	{ "Radeon R9 270", 		 27200 },
	{ "FirePro S7000", 		 27200 },
	{ "Radeon HD 7870", 		 27200 },
	{ "FirePro W8000", 		 27200 },
	{ "Radeon R9 270X", 		 27200 },
	{ "Radeon HD 7950", 		 27200 },
	{ "Radeon HD 8990", 		 27200 },
	{ "Radeon HD 7970 / R9 280X", 		 27200 },
	{ "FirePro W9000", 		 27200 },
	{ "Radeon HD 7990", 		 27200 },
	{ "Radeon R9 290", 		 27200 },
	{ "Radeon R9 290X", 		 27200 },
	{ "Radeon R9 295X2", 		 27200 },
	{	"",									0	}
};

#endif

static void		_removeStrPattern( char * pszDest, const char * pszPattern)
{
	char * pszStart;

	pszStart = strstr( pszDest, pszPattern);
	if( pszStart == nullptr)
		return;

	INT32 i, cnt = strlen( pszPattern);

	for( i = 0; i < cnt; i++)
	{
		*pszStart = ' ';
		pszStart++;
	}
};

static bool		_extractSeriesName( char * pszName, const char * pszPattern, char * pszSeries)
{
	char * pszStart = strstr( pszName, pszPattern);
	char * pszEnd, * pszTemp;

	if( pszStart == nullptr)
		return false;

	pszEnd = pszStart + strlen( pszPattern);

	// 문자열의 시작부터 pszEnd까지 복사
	for( pszTemp = pszName; pszTemp <= pszEnd; pszTemp++, pszSeries++)
	{
		*pszSeries = *pszTemp;
	}
	*pszSeries = 0;

	// Series Name을 제거한다.
	while( *pszTemp )
	{
		*pszName = *pszTemp;

		pszName++;
		pszTemp++;
	}

	*pszName = 0;

	return true;
};

static void _extractModelName( char * pszName, char * pszModel, char * pszEdition)
{
	bool bModel = true;
	bool bNumber = false;
	char lastChar = 0;

	// 지금부터는 모두 Model Name으로 간주한다.
	// 그러나, 하나의 Token이 숫자로 끝나면 Model Name의 끝이라고 판단하고
	// 그 뒤는 모두 Edition이라고 판단한다.

	while( *pszName)
	{
		switch( *pszName)
		{
			case ' ' :
			case '\n' :
			case '\t' :
			case '\r' :
				// Token의 종료
				{
					bool bAdd = true;

					if( bModel)
					{
						if( isdigit( lastChar))
						{
							bModel = false;				// 숫자로 Token이 끝났기 때문에 Edition의 시작으로 전환
							bAdd = false;
						}
					}
					
					if( bAdd)
					{
						if( bModel)
						{
							*pszModel = ' ';
							pszModel++;
						}
						else
						{
							*pszEdition = ' ';
							pszEdition++;
						}
					}
				}
				break;

			default :
				if( bModel)
				{
					if( isdigit( *pszName))
					{
						bNumber = true;
						*pszModel = *pszName;
						pszModel ++;
					}
					else
					{
						if( bNumber == false)
						{
							// 아직 숫자가 나오지 않았다.
							*pszModel = *pszName;
							pszModel ++;
						}
						else
						{
							// Edition의 시작으로 간주.
							bModel = false;

							*pszEdition = *pszName;
							pszEdition ++;
						}
					}
				}
				else
				{
					*pszEdition = *pszName;
					pszEdition ++;
				}
				break;
		}

		lastChar = *pszName;
		pszName++;
	}

	*pszModel = 0;
	*pszEdition = 0;
}

I3G_GPU_CLASS i3GfxCaps::DetermineGPUClass(void)
{
	#if defined( I3G_DX)
		INT32 i;
		INT32	FillRate = 2500;		// I3G_GPU_CLASS_E : Default
		char szName[ 512];
		char szSeries[ 64], szModel[64], szEdition[64];

		memset(&szName,0,sizeof(szName));
		memset(&szSeries,0,sizeof(szSeries));
		memset(&szModel,0,sizeof(szModel));
		memset(&szEdition,0,sizeof(szEdition));

		I3ASSERT( g_pD3D != nullptr);

		D3DADAPTER_IDENTIFIER9 m_Info;
		
		g_pD3D->GetAdapterIdentifier(g_iCurrentAdapterIndex, 0, &m_Info);

		strncpy( szName, m_Info.Description, sizeof(szName)-1);
		i3::to_upper( szName);					// 확인을 쉽도록 대문자로...

		// 회사명을 제거한다.
		{
			switch( m_Info.VendorId)
			{
				case _NVIDIA_APAPTER :
					{
						static char szNVSeries[][32] = 
						{
							"GEFORCE",
							"QUADRO",
							"GOFORCE",
							""
						};

						_removeStrPattern( szName, "NVIDIA");
						i3::remove_blank_from_head( szName);

						// Series 명을 추출한다.
						for( i = 0; szNVSeries[i][0] != 0; i++)
						{
							if( _extractSeriesName( szName, szNVSeries[i], szSeries))
								break;
						}
					}
					break;

				case _ATI_APAPTER :
					{
						static char szATISeries[][32] = 
						{
							"RADEON",
							""
						};

						_removeStrPattern( szName, "ATI");
						i3::remove_blank_from_head( szName);

						// Series 명을 추출한다.
						for( i = 0; szATISeries[i][0] != 0; i++)
						{
							if( _extractSeriesName( szName, szATISeries[i], szSeries))
								break;
						}
					}
					break;
			}
		}

		// 숫자로 시작한 Token 중에서 숫자가 계속 이어지는 끝 지점을 찾는다. (Model Number)
		_extractModelName( szName, szModel, szEdition);

		i3::remove_blank_from_head( szSeries);
		i3::remove_blank_from_tail( szSeries);
		i3::remove_blank_from_head( szModel);
		i3::remove_blank_from_tail( szModel);
		i3::remove_blank_from_head( szEdition);
		i3::remove_blank_from_tail( szEdition);
		
		// 모든 이름을 합하여 이름을 생성한다.
		sprintf( szName, "%s %s %s", szSeries, szModel, szEdition);

		{
			bool bFind = false;

			for( i = 0; s_GPUTable[i].m_szDesc[0] != 0; i++)
			{
				if(i3::generic_is_iequal( s_GPUTable[i].m_szDesc, szName) )
				{
					FillRate = s_GPUTable[i].m_TexFillRate;
					bFind = true;
				}
			}

			if( bFind == false)
			{
				// 만약 못찾았다면 Edition을 제외한 이름으로 검색한다.
				sprintf( szName, "%s %s", szSeries, szModel);

				for( i = 0; s_GPUTable[i].m_szDesc[0] != 0; i++)
				{
					if( i3::generic_is_iequal( s_GPUTable[i].m_szDesc, szName) )
					{
						FillRate = s_GPUTable[i].m_TexFillRate;
						bFind = true;
					}
				}
			}
		}

		if( FillRate < 1700)
			return I3G_GPU_CLASS_F;
		else if( FillRate < 2400)
			return I3G_GPU_CLASS_E;
		else if( FillRate < 4000)
			return I3G_GPU_CLASS_D;
		else if( FillRate < 6000)
			return I3G_GPU_CLASS_C;
		else if( FillRate < 10000)
			return I3G_GPU_CLASS_B;
	#endif

	return I3G_GPU_CLASS_A;
}

//그래픽카드를 일일이 등록할수 없다!!
//픽셀쉐이더가 3.0이상이고 desktopPC를 사용하는 유저는
//I3G_GPU_CLASS type을 최상으로 올린다
I3G_GPU_CLASS i3GfxCaps::CheckGPUClass(const I3G_GPU_CLASS gpuClass)
{
	//gpuClass type이 D로 나왔다는것은 하드코딩된 그래픽카드를 찾지 못하였다는 것을 의미한다
	if (gpuClass == I3G_GPU_CLASS_D && m_strVertexShaderProfile.size() > 0)
	{
		i3::rc_string psTemp;

		//(ps_)을 삭제
		i3::generic_string_copy(psTemp, m_strVertexShaderProfile.begin() + 3, m_strVertexShaderProfile.end());

		i3::rc_string psVersion;
		psVersion.clear();

		for (i3::rc_string::iterator it = psTemp.begin(); it != psTemp.end(); ++it)
		{
			if (*it == '_')
			{
				psVersion += '.';
			}
			else
			{
				psVersion += *it;
			}
		}

		REAL32 fPSVer = i3::atof(psVersion);

		if (i3::generic_icompare(m_Wmi.GetMachineName(), "enum_Desktop") == 0
			&& fPSVer >= 3.0f)
		{
			return I3G_GPU_CLASS_A;
		}
	}

	return gpuClass;
}