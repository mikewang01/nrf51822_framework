#include "Q_Shell.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
unsigned char  crc16_on_off = 0;
unsigned short Var2;
unsigned int   Var3;

QSH_VAR_REG(crc16_on_off, "crc16_on_off on:0 off:1", "uint8_t");
QSH_VAR_REG(Var2, "unsigned char  Var2", "uint16_t");
QSH_VAR_REG(Var3, "unsigned char  Var3", "uint32_t");

/*********************************************************************
 * @fn      Add()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
unsigned int Add(unsigned a, unsigned b)
{
    return (a + b);
}

QSH_FUN_REG(Add, "unsigned int Add(unsigned a,unsigned b)");
/*********************************************************************
 * @fn      PutString()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
unsigned int PutString(char *Str)
{
    if(*Str == '\0')
        return 0;

    printf("%s\r\n", Str);
    return 1;
}

QSH_FUN_REG(PutString, "unsigned int PutString(char *Str)");

/*********************************************************************
 * @fn      list_timer()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
extern int _list_timer(void);
long list_timer(void)
{
    return _list_timer();
}
QSH_FUN_REG(list_timer, "long list_timer()");
#if 0
/*********************************************************************
 * @fn      list_timer()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
extern int _list_device(void);
long list_device(void)
{
    return _list_device();
}
QSH_FUN_REG(list_device, "long list_device()");


/*********************************************************************
 * @fn      relay_control()
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
extern int _relay_control(uint8_t relay_no, uint8_t cmd);
int relay_control(uint8_t relay_no, uint8_t cmd)
{

    _relay_control(relay_no, cmd);
    return 0;
}
QSH_FUN_REG(relay_control, "int relay_control(uint8_t relay_no,uint8_t cmd) relayno:1~4;cmd 1:open 0 closed");


/*********************************************************************
 * @fn      _list_mem()
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
extern int _list_mem(void);
int list_mem(void)
{

    _list_mem();
    return 0;
}
QSH_FUN_REG(list_mem, "int list_mem()");


/*********************************************************************
 * @fn      _mem_read(uint32_t adress , uint32_t lenth)
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
extern int _mem_read(uint32_t adress , uint32_t lenth);
int mem_read(uint32_t adress , uint32_t lenth)
{

    _mem_read(adress , lenth);
    return 0;
}
QSH_FUN_REG(mem_read, "int mem_read(uint32_t adress , uint32_t lenth)");
/*********************************************************************
 * @fn      _flash_read()
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
extern int _flash_read(uint32_t adress , uint32_t lenth);
int flash_read(uint32_t adress , uint32_t lenth)
{
    _flash_read(adress , lenth);
    return 0;

}
QSH_FUN_REG(flash_read, "int flash_read(uint32_t adress , uint32_t lenth)");


/*********************************************************************
 * @fn      _flash_read()
 *
 * @brief   This function is implented to read data from ram to flash.
 *
 * @param   void
 *
 * @return  none
 */
extern int _flash_cp(uint32_t ram_adrr , uint32_t flash_adress , uint32_t lenth);
int flash_cp(uint32_t ram_adrr , uint32_t flash_adress , uint32_t lenth)
{
    _flash_cp(ram_adrr ,  flash_adress ,  lenth);
    return 0;

}
QSH_FUN_REG(flash_cp, "int flash_cp(uint32_t ram_adrr , uint32_t flash_adress , uint32_t lenth)");

/*********************************************************************
 * @fn      _flash_read()
 *
 * @brief   This function is implented to read data from ram to flash.
 *
 * @param   void
 *
 * @return  none
 */
unsigned short ymodem_init();
void  q_sh_setmode(uint8_t mode);
int y_modem(uint32_t destination_adress)
{
    q_sh_setmode(ymodem_mode);
    send_message(SHELL_TASK_ID  , SYSTEM_EVENT , IPC_MSG , YMODEM_START_UP , &destination_adress , sizeof(uint32_t));
    ymodem_init();
    return 0;

}
QSH_FUN_REG(y_modem, "int y_modem(uint32_t destination_adress)");
#endif


