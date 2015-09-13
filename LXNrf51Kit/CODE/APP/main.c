#include "debug.h"
#include "app.h"


int main(void)
{
    DEBUG_WARNING("\r\n\r\n------------------ Reset! ---------------------\r\n\r\n");
    
    APP_Run();

	/* Program shall NEVER reach here */
}
