#include "../lx_nrf51Kit.h"
#include "Q_Shell.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#if USE_Q_SHELL_FUNCTION

#define shell_send_message(x,y ,z) send_message((x),SYSTEM_EVENT , DEV_MSG , SHELL_COMMAND ,(void*)(y), z) //x taskid to recieve ,y data of message ,z  lenth of data
//环境变量不占用内存
extern int qShellFunTab$$Base;	//函数记录段段首
extern int qShellFunTab$$Limit;	//函数记录段段尾
extern int qShellVarTab$$Base;	//变量记录段段首
extern int qShellVarTab$$Limit;	//变量记录段段尾

#define QSH_FUN_PARA_MAX_NUM 4//函数最多支持四个参数
#define SHELL_CMD_SIZE 64

typedef unsigned int (*QSH_FUNC_TYPE)();//统一的shell函数类型支持不同个数的参数


struct finsh_shell {
    uint8_t esc_flag;
    uint8_t escbuf[4];
    char line[SHELL_CMD_SIZE];
    uint8_t line_position;
    uint8_t line_curpos;
    uint8_t status;
    uint8_t ifpass;

} finsh_shell_t;

enum input_stat {
    WAIT_NORMAL,
    WAIT_SPEC_KEY,
    WAIT_FUNC_KEY,
};

typedef enum {
    QSCT_ERROR,        //无效调用
    QSCT_LIST_FUN,     //列出所有可被shell访问的函数
    QSCT_HELP_FUN,     //列出所有可被shell访问的函数
    QSCT_LIST_VAR,     //列出所有可被shell访问的变量
    QSCT_CALL_FUN,     //调用函数
    QSCT_READ_VAR,     //查看变量的值
    QSCT_WRITE_VAR,    //给变量赋值
    QSCT_READ_REG,     //读寄存器
    QSCT_WRITE_REG     //写寄存器
} Q_SH_CALL_TYPE;      //shell调用类型

typedef enum {
    QSFPT_NOSTRING,   //非字符串型参数
    QSFPT_STRING      //字符串型参数
} Q_SH_FUN_PARA_TYPE; //函数参数类型

typedef struct {
    char               *CmdStr;    //命令字符串首地址
    Q_SH_CALL_TYPE     CallType;   //shell调用类型
    unsigned char      CallStart;  //shell调用名在命令字符串中的起始位置
    unsigned char      CallEnd;    //shell调用名在命令字符串中的结束位置
    unsigned char      ParaNum;    //shell调用的参数个数
    unsigned char      ParaStart[QSH_FUN_PARA_MAX_NUM];//每个参数在命令字符串中的起始位置
    unsigned char      ParaEnd[QSH_FUN_PARA_MAX_NUM];  //每个参数在命令字符串中的结束位置
    Q_SH_FUN_PARA_TYPE ParaTypes[QSH_FUN_PARA_MAX_NUM];//每个参数的类型
} Q_SH_LEX_RESU_DEF;



static struct finsh_shell *shell = NULL;
/*
功能：在函数记录段寻找指定的函数
入参：函数名
返回：对应的函数记录
 */
static QSH_RECORD* Q_Sh_SearchFun(char* Name)
{
    QSH_RECORD* Index;

    for(Index = (QSH_RECORD*) &qShellFunTab$$Base; Index < (QSH_RECORD*) &qShellFunTab$$Limit; Index ++) {
        if(strcmp((char*)Index->name, Name) == 0)
            return Index;
    }

    return (void *)0;
}
/*
功能：在函数记录段寻找指定的变量
入参：变量名
返回：对应的变量记录
 */
static QSH_RECORD* Q_Sh_SearchVar(char* Name)
{
    QSH_RECORD* Index;

    for(Index = (QSH_RECORD*) &qShellVarTab$$Base; Index < (QSH_RECORD*) &qShellVarTab$$Limit; Index ++) {
        if(strcmp((char*)Index->name, Name) == 0)
            return Index;
    }

    return (void *)0;
}
/*
功能：列出函数段中的所有函数记录的描述字符串
入参：无
返回：无
 */
static void Q_Sh_ListFuns(void)
{

    QSH_RECORD* Index;

    for(Index = (QSH_RECORD*) &qShellFunTab$$Base; Index < (QSH_RECORD*) &qShellFunTab$$Limit; Index++) {
        printf("%s\r\n", Index->desc);
    }
}



/*
功能：list help imformation
入参：无
返回：无
 */
static void Q_Sh_HelpFuns(void)
{
    QSH_RECORD* Index;

    for(Index = (QSH_RECORD*) &qShellFunTab$$Base; Index < (QSH_RECORD*) &qShellFunTab$$Limit; Index++) {
        printf("%s\r\n", Index->desc);
    }
}
/*
功能：列出变量段中的所有变量记录的描述字符串
入参：无
返回：无
 */
static void Q_Sh_ListVars(void)
{
    QSH_RECORD* Index;

    for(Index = (QSH_RECORD*) &qShellVarTab$$Base; Index < (QSH_RECORD*) &qShellVarTab$$Limit; Index++) {
        printf("%s\r\n", Index->desc);
    }
}
/*
功能：计算m的n次方
入参：m 底数 n 幂指数
返回：计算结果
 */
static unsigned int Q_Sh_Pow(unsigned int m, unsigned int n)
{
    unsigned int Result = 1;

    while(n--)
        Result *= m;

    return Result;
}
/*
功能：将字符转换成数字，支持16进制（大小写字母均可），不支持负数
入参：Str 数字字符串 Res 结果存放的地址
返回：0 成功 ！0 失败
 */
static unsigned int Q_Sh_Str2Num(char*Str, unsigned int *Res)
{
    unsigned int Temp;
    unsigned int Num = 0;
    unsigned int HexDec = 10;
    char *p;
    p = Str;
    *Res = 0;

    while(1) {
        if((*p >= 'A' && *p <= 'F') || (*p == 'X'))
            *p = *p + 0x20;
        else if(*p == '\0')break;

        p++;
    }

    p = Str;

    while(1) {
        if((*p <= '9' && *p >= '0') || (*p <= 'f' && *p >= 'a') || (*p == 'x' && Num == 1)) {
            if(*p >= 'a')HexDec = 16;

            Num++;
        } else if(*p == '\0')break;
        else return 1;

        p++;
    }

    p = Str;

    if(HexDec == 16) {
        if(Num < 3)return 2;

        if(*p == '0' && (*(p + 1) == 'x')) {
            p += 2;
            Num -= 2;
        } else return 3;
    } else if(Num == 0)return 4;

    while(1) {
        if(Num)Num--;

        if(*p <= '9' && *p >= '0')Temp = *p - '0';
        else Temp = *p - 'a' + 10;

        *Res += Temp * Q_Sh_Pow(HexDec, Num);
        p++;

        if(*p == '\0')break;
    }

    return 0;
}
/*
功能：从母字符串中截得子字符串
入参：Base 母字符串首地址 Start 待截子字符的起始位置 End 待截字符串结束位置
返回：子字符串首地址
 */
static char *Q_Sh_StrCut(char *Base, unsigned int Start, unsigned int End)
{
    Base[End + 1] = 0;
    return (char *)((unsigned int)Base + Start);
}
/*
功能：对输入的命令字符串进行词法分析
入参： CmdStr 命令字符串首地址 pQShLexResu 分析结果存放处
返回：0 词法分析失败 1 词法分析成功
 */
static unsigned int Q_Sh_LexicalAnalysis(char *CmdStr, Q_SH_LEX_RESU_DEF *pQShLexResu)
{
    unsigned int Index = 0, TmpPos = 0, ParamNum = 0;

    while(1) {
        if(!((CmdStr[Index] >= 'a' && CmdStr[Index] <= 'z') || (CmdStr[Index] >= 'A' && CmdStr[Index] <= 'Z') || (CmdStr[Index] >= '0' && CmdStr[Index] <= '9') || CmdStr[Index] == '_' || CmdStr[Index] == '(')) {
            pQShLexResu->CallType = QSCT_ERROR;
            printf("Lexical Analysis LX_ERROR: Function or Call name is illegal!!!\r\n");
            return 0;
        }

        if(CmdStr[Index] == '(') {
            if(Index == 0) {
                pQShLexResu->CallType = QSCT_ERROR;
                printf("Lexical Analysis LX_ERROR: Function or Call name is empty!!!\r\n");
                return 0;
            }

            pQShLexResu->CallEnd = (Index - 1);
            TmpPos = ++Index;
            break;
        }

        Index++;
    }

    while(1) {
        if(CmdStr[Index] < 33 || CmdStr[Index] > 126) {
            pQShLexResu->CallType = QSCT_ERROR;
            printf("Lexical Analysis LX_ERROR: parameter include illegal character!!!\r\n");
            return 0;
        }

        if(CmdStr[Index] == ',') {
            if(Index == TmpPos) {
                pQShLexResu->CallType = QSCT_ERROR;
                printf("Lexical Analysis LX_ERROR: parameter include illegal comma!!!\r\n");
                return 0;
            }

            if(CmdStr[Index - 1] == '\"') {
                if(ParamNum < QSH_FUN_PARA_MAX_NUM) {
                    pQShLexResu->ParaTypes[ParamNum] = QSFPT_STRING;
                    pQShLexResu->ParaStart[ParamNum] = TmpPos + 1;
                    pQShLexResu->ParaEnd[ParamNum] = Index - 2;
                    ParamNum++;
                } else {
                    ParamNum++;
                }

                TmpPos = Index + 1;
            } else {
                if(ParamNum < QSH_FUN_PARA_MAX_NUM) {
                    pQShLexResu->ParaTypes[ParamNum] = QSFPT_NOSTRING;
                    pQShLexResu->ParaStart[ParamNum] = TmpPos;
                    pQShLexResu->ParaEnd[ParamNum] = Index - 1;
                    ParamNum++;
                } else {
                    ParamNum++;
                }

                TmpPos = Index + 1;
            }
        } else if(CmdStr[Index] == ')') {
            if(Index == pQShLexResu->CallEnd + 2) {
                pQShLexResu->ParaNum = 0;
                break;
            }

            if(Index == TmpPos) {
                pQShLexResu->CallType = QSCT_ERROR;
                printf("Lexical Analysis LX_ERROR: parameter include illegal comma!!!\r\n");
                return 0;
            }

            if(CmdStr[Index - 1] == '\"') {
                if(ParamNum < QSH_FUN_PARA_MAX_NUM) {
                    pQShLexResu->ParaTypes[ParamNum] = QSFPT_STRING;
                    pQShLexResu->ParaStart[ParamNum] = TmpPos + 1;
                    pQShLexResu->ParaEnd[ParamNum] = Index - 2;
                    ParamNum++;
                } else {
                    ParamNum++;
                }

                pQShLexResu->ParaNum = ParamNum;
                break;
            } else {
                if(ParamNum < QSH_FUN_PARA_MAX_NUM) {
                    pQShLexResu->ParaTypes[ParamNum] = QSFPT_NOSTRING;
                    pQShLexResu->ParaStart[ParamNum] = TmpPos;
                    pQShLexResu->ParaEnd[ParamNum] = Index - 1;
                    ParamNum++;
                } else {
                    ParamNum++;
                }

                pQShLexResu->ParaNum = ParamNum;
                break;
            }
        }

        Index++;
    }

    pQShLexResu->CmdStr = CmdStr;
    pQShLexResu->CallStart = 0;

    if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "lf") == 0)
        pQShLexResu->CallType = QSCT_LIST_FUN;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "lv") == 0)
        pQShLexResu->CallType = QSCT_LIST_VAR;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "get") == 0)
        pQShLexResu->CallType = QSCT_READ_VAR;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "set") == 0)
        pQShLexResu->CallType = QSCT_WRITE_VAR;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "read") == 0)
        pQShLexResu->CallType = QSCT_READ_REG;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "write") == 0)
        pQShLexResu->CallType = QSCT_WRITE_REG;
    else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd), "help") == 0)
        pQShLexResu->CallType = QSCT_HELP_FUN;
    else
        pQShLexResu->CallType = QSCT_CALL_FUN;

    return 1;
}
/*
功能：对词法分析的结果进行语法分析，并执行
入参： pQShLexResu 词法分析结果存放处
返回：0 失败 1 成功
 */
static unsigned int Q_Sh_ParseAnalysis(Q_SH_LEX_RESU_DEF *pQShLexResu)
{
    QSH_RECORD *pRecord;
    unsigned int FunPara[QSH_FUN_PARA_MAX_NUM];
    unsigned int FunResu;
    unsigned int VarSet;
    unsigned int VarAddr;
    unsigned char i;
    unsigned char Ret = 1;

    switch(pQShLexResu->CallType) {
    case QSCT_CALL_FUN: {
        pRecord = Q_Sh_SearchFun(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->CallStart, pQShLexResu->CallEnd));

        if(pRecord == 0) {
            printf("Parse Analysis LX_ERROR: the input function have not been regist!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaNum > QSH_FUN_PARA_MAX_NUM) {
            printf("Parse Analysis LX_ERROR: function's parameter number can't over four!!!\r\n");
            Ret = 0;
            break;
        }

        for(i = 0; i < pQShLexResu->ParaNum; i++) {
            if(pQShLexResu->ParaTypes[i] == QSFPT_STRING) {
                FunPara[i] = (unsigned int)Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[i], pQShLexResu->ParaEnd[i]);
            } else {
                if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[i], pQShLexResu->ParaEnd[i]), &(FunPara[i])) != 0) {
                    printf("Parse Analysis LX_ERROR: the input number string is illegal!!!\r\n");
                    Ret = 0;
                    break;
                }
            }
        }

        FunResu = ((QSH_FUNC_TYPE)(pRecord->addr))(FunPara[0], FunPara[1], FunPara[2], FunPara[3]);
        printf("return %d\r\n", FunResu);
    }
    break;

    case QSCT_READ_VAR: {
        pRecord = Q_Sh_SearchVar(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[0], pQShLexResu->ParaEnd[0]));

        if(pRecord == 0) {
            printf("Parse Analysis LX_ERROR: the input variable have not been regist!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaNum != 1) {
            printf("Parse Analysis LX_ERROR: this call must have only one parameter!!!\r\n");
            Ret = 0;
            break;
        }

        if(strcmp((char*)pRecord->typedesc, "uint8_t") == 0)
            printf("%s=%d\r\n", pRecord->name, *(unsigned char *)(pRecord->addr));
        else if(strcmp((char*)pRecord->typedesc, "uint16_t") == 0)
            printf("%s=%d\r\n", pRecord->name, *(unsigned short *)(pRecord->addr));
        else if(strcmp((char*)pRecord->typedesc, "uint32_t") == 0)
            printf("%s=%d\r\n", pRecord->name, *(unsigned int *)(pRecord->addr));
        else {
            printf("the describe of variable's type is illegal!!!\r\n");
            Ret = 0;
            break;
        }
    }
    break;

    case QSCT_WRITE_VAR: {
        pRecord = Q_Sh_SearchVar(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[0], pQShLexResu->ParaEnd[0]));

        if(pRecord == 0) {
            printf("Parse Analysis LX_ERROR: the input variable have not been regist!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaNum != 2) {
            printf("Parse Analysis LX_ERROR: this call must have two parameter!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaTypes[1] == QSFPT_STRING) {
            printf("Parse Analysis LX_ERROR: this call's second parameter can't be string type!!!\r\n");
            Ret = 0;
            break;
        }

        if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[1], pQShLexResu->ParaEnd[1]), &VarSet) != 0) {
            printf("Parse Analysis LX_ERROR: the input number string is illegal!!!\r\n");
            Ret = 0;
            break;
        }

        if(strcmp((char*)pRecord->typedesc, "uint8_t") == 0) {
            *(unsigned char *)(pRecord->addr) = VarSet;
            printf("%s=%d\r\n", pRecord->name, *(unsigned char *)(pRecord->addr));
        } else if(strcmp((char*)pRecord->typedesc, "uint16_t") == 0) {
            *(unsigned short *)(pRecord->addr) = VarSet;
            printf("%s=%d\r\n", pRecord->name, *(unsigned short *)(pRecord->addr));
        } else if(strcmp((char*)pRecord->typedesc, "uint32_t") == 0) {
            *(unsigned int *)(pRecord->addr) = VarSet;
            printf("%s=%d\r\n", pRecord->name, *(unsigned int *)(pRecord->addr));
        } else {
            printf("the describe of variable's type is illegal!!!\r\n");
            Ret = 0;
            break;
        }
    }
    break;

    case QSCT_READ_REG: {
        if(pQShLexResu->ParaNum != 1) {
            printf("Parse Analysis LX_ERROR: this call must have only one parameter!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaTypes[0] == QSFPT_STRING) {
            printf("Parse Analysis LX_ERROR: this call's parameter can not be string type!!!\r\n");
            Ret = 0;
            break;
        }

        if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[0], pQShLexResu->ParaEnd[0]), &VarAddr) != 0) {
            printf("Parse Analysis LX_ERROR: the input number string is illegal!!!\r\n");
            Ret = 0;
            break;
        }

        printf("*(0x%x)=0x%x\r\n", VarAddr, *(unsigned int *)VarAddr);
    }
    break;

    case QSCT_WRITE_REG: {
        if(pQShLexResu->ParaNum != 2) {
            printf("Parse Analysis LX_ERROR: this call must have two parameter!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaTypes[0] == QSFPT_STRING) {
            printf("Parse Analysis LX_ERROR: this call's first parameter can not be string type!!!\r\n");
            Ret = 0;
            break;
        }

        if(pQShLexResu->ParaTypes[1] == QSFPT_STRING) {
            printf("Parse Analysis LX_ERROR: this call's second parameter can not be string type!!!\r\n");
            Ret = 0;
            break;
        }

        if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[0], pQShLexResu->ParaEnd[0]), &VarAddr) != 0) {
            printf("Parse Analysis LX_ERROR: the input number string is illegal!!!\r\n");
            Ret = 0;
            break;
        }

        if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr, pQShLexResu->ParaStart[1], pQShLexResu->ParaEnd[1]), &VarSet) != 0) {
            printf("Parse Analysis LX_ERROR: the input number string is illegal!!!\r\n");
            Ret = 0;
            break;
        }

        *(unsigned int *)VarAddr = VarSet;
        printf("*(0x%x)=0x%x\r\n", VarAddr, *(unsigned *)VarAddr);
    }
    break;

    case QSCT_LIST_FUN: {
        if(pQShLexResu->ParaNum > 0) {
            printf("Parse Analysis LX_ERROR: this call must have no parameter!!!\r\n");
            Ret = 0;
            break;
        }

        Q_Sh_ListFuns();
    }
    break;

    case QSCT_HELP_FUN: {
        if(pQShLexResu->ParaNum > 0) {
            printf("Parse Analysis LX_ERROR: this call must have no parameter!!!\r\n");
            Ret = 0;
            break;
        }

        Q_Sh_HelpFuns();

    }
    break;

    case QSCT_LIST_VAR: {
        if(pQShLexResu->ParaNum > 0) {
            printf("Parse Analysis LX_ERROR: this call must have no parameter!!!\r\n");
            Ret = 0;
            break;
        }

        Q_Sh_ListVars();
    }
    break;

    default: {
        printf("Parse Analysis LX_ERROR: can't get here!!!\r\n");
        Ret = 0;
    }
    }

    return Ret;
}
/*
shell密码存放处，开机后第一次使用shell需要输入此密码
 */
static const char Q_ShellPassWord[] = "123456";

/*
功能：shell对外的接口，执行命令
入参：IfCtrl 指示是否接收到一个控制字符 CmdStr 从串口得到的命令字符串
返回：1 成功 0 失败
 */
unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl, char *CmdStr)
{
    Q_SH_LEX_RESU_DEF Q_Sh_LexResu;
    static unsigned char IfWaitInput = 0;

    if(shell->ifpass) {
        if(IfCtrl == 1) {
            if(((unsigned short *)CmdStr)[0] == 0x445b)
            {}
            else if(((unsigned short *)CmdStr)[0] == 0x435b)
            {}
            else
                printf("CtrlCode:%x\n\r", ((unsigned short *)CmdStr)[0]);

            return 0;
        }

        if(Q_Sh_LexicalAnalysis(CmdStr, &Q_Sh_LexResu) == 0) //词法分析
            return 0;

        if(Q_Sh_ParseAnalysis(&Q_Sh_LexResu) == 0)  //语法分析
            return 0;

        return 1;
    } else {
        if(IfWaitInput == 0) {
            printf("Please input shell password:");
            IfWaitInput = 1;
        } else {


            if(strcmp(CmdStr, (char*)Q_ShellPassWord) == 0) {
                shell->ifpass = 1;
                printf("password right!\r\n");
            } else {
                printf("password wrong!\r\n");
                printf("Please input shell password again:");
            }
        }

        return 0;
    }
}

#else
/*
当USE_Q_SHELL_FUNCTION=0即shell功能被关闭后，为了不需要修改原程序而加入
 */
unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl, char *CmdStr)
{
    return 0;
}

#endif

static uint8_t call_back_switch = shell_mode;
/*********************************************************************
 * @fn       Q_Sh_init()()
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
void  q_sh_setmode(uint8_t mode)
{
    call_back_switch = mode;

}

/*********************************************************************
 * @fn       Q_Sh_init()()
 *
 * @brief   This function expose operation to the shell.
 *
 * @param   void
 *
 * @return  none
 */
int  Q_Sh_init()
{
    shell = LX_Malloc(sizeof(finsh_shell_t));

    if(shell == NULL);

    {
        return LX_ERROR;
    }
    return LX_OK;

}

/*
             * handle control key
             * up key  : 0x1b 0x5b 0x41
             * down key: 0x1b 0x5b 0x42
             * right key:0x1b 0x5b 0x43
             * left key: 0x1b 0x5b 0x44
 */


//#include "YModem.h"

int  Q_Sh_rx_callback(char data)
{
    char Recieve = data;

    if(call_back_switch == shell_mode) {
        /*check if this varies has been initiated ot not */
        if(shell == NULL) {
            return LX_ERROR;
        }

        if(Recieve == 0x1b) {
            shell->status = WAIT_SPEC_KEY;
            goto end;
        } else if(shell->status == WAIT_SPEC_KEY) {
            if(Recieve == 0x5b) {
                shell->status = WAIT_FUNC_KEY;
                goto end;
            }

            shell->status = WAIT_NORMAL;
        } else if(shell->status == WAIT_FUNC_KEY) {
            shell->status = WAIT_NORMAL;

            if(Recieve == 0x44) { /* left key */
                if(shell->line_curpos) {
                    printf("\b");
                    shell->line_curpos --;
                }

                goto end;
            } else if(Recieve == 0x43) { /* right key */
                if(shell->line_curpos < shell->line_position) {
                    printf("%c", shell->line[shell->line_curpos]);
                    shell->line_curpos ++;
                }

                goto end;

            }

        }


        if(Recieve == 13) { //回车

            shell->line[shell->line_position] = 0;
            printf("\n\r");

            if(shell->line_position > 0 || !(shell->ifpass)) {
                //shell_send_message(SHELL_TASK_ID , shell->line , strlen(shell->line)+1);
                //Q_Sh_CmdHandler(0,shell->line);
            }

            shell->line_position = 0;
            shell->line_curpos = 0;
            printf("\n\rWSHOS>>");
        }
        /* handle backspace key */
        else if(Recieve == 0x7f || Recieve == 0x08) { //回删
            if(shell->line_curpos > 0) {
                shell->line_position--;
                shell->line_curpos--;

                if(shell->line_position > shell->line_curpos) {
                    uint16_t i;
                    memmove(&shell->line[shell->line_curpos],
                            &shell->line[shell->line_curpos + 1],
                            shell->line_position - shell->line_curpos);
                    shell->line[shell->line_position] = 0;

                    printf("\b%s  \b", &shell->line[shell->line_curpos]);

                    /* move the cursor to the origin position */
                    for(i = shell->line_curpos; i <= shell->line_position; i++)
                        printf("\b");
                } else {
                    shell->line[(shell->line_position)] = 0;
                    printf("\b \b");
                }

            }
        }

        if(shell->line_position >= SHELL_CMD_SIZE) {
            shell->line_position = 0; //超出范围，就停止加入新的了。
            shell->line_curpos = 0; //超出范围，就停止加入新的了�
        }

        if(Recieve >= 0x20) {


            if(shell->line_position > shell->line_curpos) {
                uint16_t i;

                memmove(&shell->line[shell->line_curpos + 1],
                        &shell->line[shell->line_curpos],
                        shell->line_position - shell->line_curpos);
                shell->line[shell->line_curpos] = Recieve;
                shell->line[shell->line_position + 1] = 0;
                printf("%s", &shell->line[shell->line_curpos]);

                /* move the cursor to the origin position */
                for(i = shell->line_curpos; i < shell->line_position; i++)
                    printf("\b");
            } else {
                shell->line[(shell->line_position)] = Recieve;
                shell->line[shell->line_curpos + 1] = 0;
                printf("%c", shell->line[(shell->line_position)]);
            }

            shell->line_position++;
            shell->line_curpos++;
        }




end:
        return LX_OK;
    } else if(call_back_switch == ymodem_mode) {
        //YmodenReceOneChar(data);

    }

    return LX_OK;
}

