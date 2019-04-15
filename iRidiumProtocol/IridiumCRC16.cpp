/*******************************************************************************
 * Copyright (c) 2013-2019 iRidi Ltd. www.iridi.com
 *
 * Все права зарегистрированы. Эта программа и сопровождающие материалы доступны
 * на условиях Eclipse Public License v2.0 и Eclipse Distribution License v1.0,
 * которая сопровождает это распространение. 
 *
 * Текст Eclipse Public License доступен по ссылке
 *    http://www.eclipse.org/legal/epl-v20.html
 * Текст Eclipse Distribution License доступн по ссылке
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Участники:
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
//////////////////////////////////////////////////////////////////////////
// Вычисление CRC16
// Тест на AVR вычисление CRC16 10000 раз для 256 байт
// Медленная реализация 14 секунд
// Быстрая реализация 5 секунды
//////////////////////////////////////////////////////////////////////////
#include "IridiumCRC16.h"

// Проверка на использование быстрого алгоритма вычисления CRC16
#if defined(IRIDIUM_ENABLE_FAST_CRC16)

#include "IridiumPlatform.h"

// Таблица CRC16 modbus
#if defined(IRIDIUM_AVR_PLATFORM)
// Таблица располагается в программной памяти
#include <avr/pgmspace.h>
const static u16 g_aModbusCRCTable[1024] PROGMEM =
#else    // defined(IRIDIUM_AVR_PLATFORM)
const static u16 g_aModbusCRCTable[1024] =
#endif   // defined(IRIDIUM_AVR_PLATFORM)
{
   0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
   0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
   0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
   0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
   0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
   0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
   0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
   0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
   0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
   0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
   0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
   0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
   0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
   0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
   0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
   0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
   0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
   0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
   0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
   0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
   0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
   0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
   0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
   0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
   0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
   0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
   0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
   0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
   0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
   0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
   0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
   0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
   0x0000, 0x9001, 0x6001, 0xf000, 0xc002, 0x5003, 0xa003, 0x3002,
   0xc007, 0x5006, 0xa006, 0x3007, 0x0005, 0x9004, 0x6004, 0xf005,
   0xc00d, 0x500c, 0xa00c, 0x300d, 0x000f, 0x900e, 0x600e, 0xf00f,
   0x000a, 0x900b, 0x600b, 0xf00a, 0xc008, 0x5009, 0xa009, 0x3008,
   0xc019, 0x5018, 0xa018, 0x3019, 0x001b, 0x901a, 0x601a, 0xf01b,
   0x001e, 0x901f, 0x601f, 0xf01e, 0xc01c, 0x501d, 0xa01d, 0x301c,
   0x0014, 0x9015, 0x6015, 0xf014, 0xc016, 0x5017, 0xa017, 0x3016,
   0xc013, 0x5012, 0xa012, 0x3013, 0x0011, 0x9010, 0x6010, 0xf011,
   0xc031, 0x5030, 0xa030, 0x3031, 0x0033, 0x9032, 0x6032, 0xf033,
   0x0036, 0x9037, 0x6037, 0xf036, 0xc034, 0x5035, 0xa035, 0x3034,
   0x003c, 0x903d, 0x603d, 0xf03c, 0xc03e, 0x503f, 0xa03f, 0x303e,
   0xc03b, 0x503a, 0xa03a, 0x303b, 0x0039, 0x9038, 0x6038, 0xf039,
   0x0028, 0x9029, 0x6029, 0xf028, 0xc02a, 0x502b, 0xa02b, 0x302a,
   0xc02f, 0x502e, 0xa02e, 0x302f, 0x002d, 0x902c, 0x602c, 0xf02d,
   0xc025, 0x5024, 0xa024, 0x3025, 0x0027, 0x9026, 0x6026, 0xf027,
   0x0022, 0x9023, 0x6023, 0xf022, 0xc020, 0x5021, 0xa021, 0x3020,
   0xc061, 0x5060, 0xa060, 0x3061, 0x0063, 0x9062, 0x6062, 0xf063,
   0x0066, 0x9067, 0x6067, 0xf066, 0xc064, 0x5065, 0xa065, 0x3064,
   0x006c, 0x906d, 0x606d, 0xf06c, 0xc06e, 0x506f, 0xa06f, 0x306e,
   0xc06b, 0x506a, 0xa06a, 0x306b, 0x0069, 0x9068, 0x6068, 0xf069,
   0x0078, 0x9079, 0x6079, 0xf078, 0xc07a, 0x507b, 0xa07b, 0x307a,
   0xc07f, 0x507e, 0xa07e, 0x307f, 0x007d, 0x907c, 0x607c, 0xf07d,
   0xc075, 0x5074, 0xa074, 0x3075, 0x0077, 0x9076, 0x6076, 0xf077,
   0x0072, 0x9073, 0x6073, 0xf072, 0xc070, 0x5071, 0xa071, 0x3070,
   0x0050, 0x9051, 0x6051, 0xf050, 0xc052, 0x5053, 0xa053, 0x3052,
   0xc057, 0x5056, 0xa056, 0x3057, 0x0055, 0x9054, 0x6054, 0xf055,
   0xc05d, 0x505c, 0xa05c, 0x305d, 0x005f, 0x905e, 0x605e, 0xf05f,
   0x005a, 0x905b, 0x605b, 0xf05a, 0xc058, 0x5059, 0xa059, 0x3058,
   0xc049, 0x5048, 0xa048, 0x3049, 0x004b, 0x904a, 0x604a, 0xf04b,
   0x004e, 0x904f, 0x604f, 0xf04e, 0xc04c, 0x504d, 0xa04d, 0x304c,
   0x0044, 0x9045, 0x6045, 0xf044, 0xc046, 0x5047, 0xa047, 0x3046,
   0xc043, 0x5042, 0xa042, 0x3043, 0x0041, 0x9040, 0x6040, 0xf041,
   0x0000, 0xc051, 0xc0a1, 0x00f0, 0xc141, 0x0110, 0x01e0, 0xc1b1,
   0xc281, 0x02d0, 0x0220, 0xc271, 0x03c0, 0xc391, 0xc361, 0x0330,
   0xc501, 0x0550, 0x05a0, 0xc5f1, 0x0440, 0xc411, 0xc4e1, 0x04b0,
   0x0780, 0xc7d1, 0xc721, 0x0770, 0xc6c1, 0x0690, 0x0660, 0xc631,
   0xca01, 0x0a50, 0x0aa0, 0xcaf1, 0x0b40, 0xcb11, 0xcbe1, 0x0bb0,
   0x0880, 0xc8d1, 0xc821, 0x0870, 0xc9c1, 0x0990, 0x0960, 0xc931,
   0x0f00, 0xcf51, 0xcfa1, 0x0ff0, 0xce41, 0x0e10, 0x0ee0, 0xceb1,
   0xcd81, 0x0dd0, 0x0d20, 0xcd71, 0x0cc0, 0xcc91, 0xcc61, 0x0c30,
   0xd401, 0x1450, 0x14a0, 0xd4f1, 0x1540, 0xd511, 0xd5e1, 0x15b0,
   0x1680, 0xd6d1, 0xd621, 0x1670, 0xd7c1, 0x1790, 0x1760, 0xd731,
   0x1100, 0xd151, 0xd1a1, 0x11f0, 0xd041, 0x1010, 0x10e0, 0xd0b1,
   0xd381, 0x13d0, 0x1320, 0xd371, 0x12c0, 0xd291, 0xd261, 0x1230,
   0x1e00, 0xde51, 0xdea1, 0x1ef0, 0xdf41, 0x1f10, 0x1fe0, 0xdfb1,
   0xdc81, 0x1cd0, 0x1c20, 0xdc71, 0x1dc0, 0xdd91, 0xdd61, 0x1d30,
   0xdb01, 0x1b50, 0x1ba0, 0xdbf1, 0x1a40, 0xda11, 0xdae1, 0x1ab0,
   0x1980, 0xd9d1, 0xd921, 0x1970, 0xd8c1, 0x1890, 0x1860, 0xd831,
   0xe801, 0x2850, 0x28a0, 0xe8f1, 0x2940, 0xe911, 0xe9e1, 0x29b0,
   0x2a80, 0xead1, 0xea21, 0x2a70, 0xebc1, 0x2b90, 0x2b60, 0xeb31,
   0x2d00, 0xed51, 0xeda1, 0x2df0, 0xec41, 0x2c10, 0x2ce0, 0xecb1,
   0xef81, 0x2fd0, 0x2f20, 0xef71, 0x2ec0, 0xee91, 0xee61, 0x2e30,
   0x2200, 0xe251, 0xe2a1, 0x22f0, 0xe341, 0x2310, 0x23e0, 0xe3b1,
   0xe081, 0x20d0, 0x2020, 0xe071, 0x21c0, 0xe191, 0xe161, 0x2130,
   0xe701, 0x2750, 0x27a0, 0xe7f1, 0x2640, 0xe611, 0xe6e1, 0x26b0,
   0x2580, 0xe5d1, 0xe521, 0x2570, 0xe4c1, 0x2490, 0x2460, 0xe431,
   0x3c00, 0xfc51, 0xfca1, 0x3cf0, 0xfd41, 0x3d10, 0x3de0, 0xfdb1,
   0xfe81, 0x3ed0, 0x3e20, 0xfe71, 0x3fc0, 0xff91, 0xff61, 0x3f30,
   0xf901, 0x3950, 0x39a0, 0xf9f1, 0x3840, 0xf811, 0xf8e1, 0x38b0,
   0x3b80, 0xfbd1, 0xfb21, 0x3b70, 0xfac1, 0x3a90, 0x3a60, 0xfa31,
   0xf601, 0x3650, 0x36a0, 0xf6f1, 0x3740, 0xf711, 0xf7e1, 0x37b0,
   0x3480, 0xf4d1, 0xf421, 0x3470, 0xf5c1, 0x3590, 0x3560, 0xf531,
   0x3300, 0xf351, 0xf3a1, 0x33f0, 0xf241, 0x3210, 0x32e0, 0xf2b1,
   0xf181, 0x31d0, 0x3120, 0xf171, 0x30c0, 0xf091, 0xf061, 0x3030,
   0x0000, 0xfc01, 0xb801, 0x4400, 0x3001, 0xcc00, 0x8800, 0x7401,
   0x6002, 0x9c03, 0xd803, 0x2402, 0x5003, 0xac02, 0xe802, 0x1403,
   0xc004, 0x3c05, 0x7805, 0x8404, 0xf005, 0x0c04, 0x4804, 0xb405,
   0xa006, 0x5c07, 0x1807, 0xe406, 0x9007, 0x6c06, 0x2806, 0xd407,
   0xc00b, 0x3c0a, 0x780a, 0x840b, 0xf00a, 0x0c0b, 0x480b, 0xb40a,
   0xa009, 0x5c08, 0x1808, 0xe409, 0x9008, 0x6c09, 0x2809, 0xd408,
   0x000f, 0xfc0e, 0xb80e, 0x440f, 0x300e, 0xcc0f, 0x880f, 0x740e,
   0x600d, 0x9c0c, 0xd80c, 0x240d, 0x500c, 0xac0d, 0xe80d, 0x140c,
   0xc015, 0x3c14, 0x7814, 0x8415, 0xf014, 0x0c15, 0x4815, 0xb414,
   0xa017, 0x5c16, 0x1816, 0xe417, 0x9016, 0x6c17, 0x2817, 0xd416,
   0x0011, 0xfc10, 0xb810, 0x4411, 0x3010, 0xcc11, 0x8811, 0x7410,
   0x6013, 0x9c12, 0xd812, 0x2413, 0x5012, 0xac13, 0xe813, 0x1412,
   0x001e, 0xfc1f, 0xb81f, 0x441e, 0x301f, 0xcc1e, 0x881e, 0x741f,
   0x601c, 0x9c1d, 0xd81d, 0x241c, 0x501d, 0xac1c, 0xe81c, 0x141d,
   0xc01a, 0x3c1b, 0x781b, 0x841a, 0xf01b, 0x0c1a, 0x481a, 0xb41b,
   0xa018, 0x5c19, 0x1819, 0xe418, 0x9019, 0x6c18, 0x2818, 0xd419,
   0xc029, 0x3c28, 0x7828, 0x8429, 0xf028, 0x0c29, 0x4829, 0xb428,
   0xa02b, 0x5c2a, 0x182a, 0xe42b, 0x902a, 0x6c2b, 0x282b, 0xd42a,
   0x002d, 0xfc2c, 0xb82c, 0x442d, 0x302c, 0xcc2d, 0x882d, 0x742c,
   0x602f, 0x9c2e, 0xd82e, 0x242f, 0x502e, 0xac2f, 0xe82f, 0x142e,
   0x0022, 0xfc23, 0xb823, 0x4422, 0x3023, 0xcc22, 0x8822, 0x7423,
   0x6020, 0x9c21, 0xd821, 0x2420, 0x5021, 0xac20, 0xe820, 0x1421,
   0xc026, 0x3c27, 0x7827, 0x8426, 0xf027, 0x0c26, 0x4826, 0xb427,
   0xa024, 0x5c25, 0x1825, 0xe424, 0x9025, 0x6c24, 0x2824, 0xd425,
   0x003c, 0xfc3d, 0xb83d, 0x443c, 0x303d, 0xcc3c, 0x883c, 0x743d,
   0x603e, 0x9c3f, 0xd83f, 0x243e, 0x503f, 0xac3e, 0xe83e, 0x143f,
   0xc038, 0x3c39, 0x7839, 0x8438, 0xf039, 0x0c38, 0x4838, 0xb439,
   0xa03a, 0x5c3b, 0x183b, 0xe43a, 0x903b, 0x6c3a, 0x283a, 0xd43b,
   0xc037, 0x3c36, 0x7836, 0x8437, 0xf036, 0x0c37, 0x4837, 0xb436,
   0xa035, 0x5c34, 0x1834, 0xe435, 0x9034, 0x6c35, 0x2835, 0xd434,
   0x0033, 0xfc32, 0xb832, 0x4433, 0x3032, 0xcc33, 0x8833, 0x7432,
   0x6031, 0x9c30, 0xd830, 0x2431, 0x5030, 0xac31, 0xe831, 0x1430
};

#if defined(IRIDIUM_AVR_PLATFORM)
// Версия для AVR
#define crc_n4(crc, data, table) crc ^= data; \
   crc = pgm_read_word(&table[(crc & 0xff) + 0x300]) ^ \
   pgm_read_word(&table[((crc >> 8) & 0xff) + 0x200]) ^ \
   pgm_read_word(&table[((data >> 16) & 0xff) + 0x100]) ^ \
   pgm_read_word(&table[data >> 24]);

#define  crc_n1(crc, data, table) \
   crc = (crc >> 8) ^ pgm_read_word(&table[(crc & 0xff) ^ data]);

#else
// Версия для других платформ
#define crc_n4(crc, data, table) crc ^= data; \
   crc = table[(crc & 0xff) + 0x300] ^ \
   table[((crc >> 8) & 0xff) + 0x200] ^ \
   table[((data >> 16) & 0xff) + 0x100] ^ \
   table[data >> 24];

#define  crc_n1(crc, data, table) \
   crc = (crc >> 8) ^ table[(crc & 0xff) ^ data];

#endif   // defined(IRIDIUM_AVR_PLATFORM)

/**
   Медленное вычисление CRC-16 (Modbus)
   на входе    :  in_u16Init  - первичное значение
                  in_pBuffer  - указатель на буфер с данными
                  in_stSize   - размер данных в буфере
   на выходе   :  вычисленное значение CRC
   примечание  :  name:    "CRC-16/MODBUS"
                  init:    0xFFFF
                  poly:    0x8005 (0xA001)
                  xor:     0x0000
                  rev:     true
*/
u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize)
{
   u16 l_u16CRC = in_u16Init;

   // Расчет до выравнивания
   while(((uintptr_t)in_pBuffer & 3) && in_stSize)
   {
      crc_n1(l_u16CRC, *in_pBuffer++, g_aModbusCRCTable);
      in_stSize--;
   }

   // Расчет блоками по 16 байт
   while(in_stSize >= 16)
   {
      in_stSize -= 16;
      crc_n4(l_u16CRC, ((u32*)in_pBuffer)[0], g_aModbusCRCTable);
      crc_n4(l_u16CRC, ((u32*)in_pBuffer)[1], g_aModbusCRCTable);
      crc_n4(l_u16CRC, ((u32*)in_pBuffer)[2], g_aModbusCRCTable);
      crc_n4(l_u16CRC, ((u32*)in_pBuffer)[3], g_aModbusCRCTable);
      in_pBuffer += 16;
   }

   // Расчет остатка
   while(in_stSize--)
      crc_n1(l_u16CRC, *in_pBuffer++, g_aModbusCRCTable);

   return l_u16CRC;
}

#else    // defined(IRIDIUM_ENABLE_FAST_CRC16)

/**
   Медленное вычисление CRC-16 (Modbus)
   на входе    :  in_u16Init  - первичное значение
                  in_pBuffer  - указатель на буфер с данными
                  in_stSize   - размер данных в буфере
   на выходе   :  вычисленное значение CRC
   примечание  :  name:    "CRC-16/MODBUS"
                  init:    0xFFFF
                  poly:    0x8005 (0xA001)
                  xor:     0x0000
                  rev:     true
*/
u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize)
{
   // Обработка данных из буфера
   while(in_stSize--)
   {
      in_u16Init ^= *in_pBuffer++;

      for(u8 i = 0; i < 8; i++)
         in_u16Init = in_u16Init & 0x1 ? (in_u16Init >> 1) ^ 0xA001 : in_u16Init >> 1;
   }
   return in_u16Init;
}

#endif   // defined(IRIDIUM_ENABLE_FAST_CRC16)
