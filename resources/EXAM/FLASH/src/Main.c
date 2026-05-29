/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : FALSH读写例程
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

uint8_t TestBuf[1024];

/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   调试初始化
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main()
{
    uint16_t i;
    uint8_t  s;

    SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* 配置串口调试 */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

#if 1 // 读写Data-Flash

    PRINT("EEPROM_READ...\n");
    EEPROM_READ(0, TestBuf, 500);
    for(i = 0; i < 500; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    s = EEPROM_ERASE(0, EEPROM_BLOCK_SIZE);
    PRINT("EEPROM_ERASE=%02x\n", s);
    PRINT("EEPROM_READ...\n");
    EEPROM_READ(0, TestBuf, 500);
    for(i = 0; i < 500; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    for(i = 0; i < 500; i++)
        TestBuf[i] = 0x0 + i;
    s = EEPROM_WRITE(0, TestBuf, 500);
    PRINT("EEPROM_WRITE=%02x\n", s);
    PRINT("EEPROM_READ...\n");
    EEPROM_READ(0, TestBuf, 500);
    for(i = 0; i < 500; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

#endif

#if 1 // 获取唯一ID，MAC地址，读写Flash-ROM

    PRINT("GET_UNIQUE_ID...\n");
    GET_UNIQUE_ID(TestBuf);
    for(i = 0; i < 8; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    PRINT("FlashMACADDRESS...\n");
    GetMACAddress(TestBuf);
    for(i = 0; i < 6; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    PRINT("FLASH_READ 20k...\n");
    FLASH_ROM_READ(20 * 1024, TestBuf, 128);
    for(i = 0; i < 128; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    s = FLASH_ROM_ERASE(20 * 1024, 4096);
    PRINT("FLASH_ROM_ERASE=%02x\n", s);
    PRINT("FLASH_READ 20k...\n");
    FLASH_ROM_READ(20 * 1024, TestBuf, 128);
    for(i = 0; i < 128; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

    for(i = 0; i < 128; i++)
    {
        TestBuf[i] = 0x70 + i;
    }
    s = FLASH_ROM_WRITE(20 * 1024, TestBuf, 128); //Codeflash写入必须是4字节的整数倍
    PRINT("FlashWrite=%02x\n", s);
    s = FLASH_ROM_VERIFY(20 * 1024, TestBuf, 128);
    PRINT("FlashVerify=%02x\n", s);

    PRINT("FLASH_READ...\n");
    FLASH_ROM_READ(20 * 1024, TestBuf, 128);
    for(i = 0; i < 128; i++)
    {
        PRINT("%02x ", TestBuf[i]);
    }
    PRINT("\n");

#endif

    while(1);
}
