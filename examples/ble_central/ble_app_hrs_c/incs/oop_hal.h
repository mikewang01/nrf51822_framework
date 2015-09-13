#ifndef __OOP_HAL_H__
#define __OOP_HAL_H__



/*class defination header*/
#define DEF_CLASS(__NAME)\
	typedef struct __##__NAME	__##__NAME;\
	struct  __##__NAME{

#define END_DEF_CLASS(__NAME)\
	};\
	INIT_DECLARATION(__NAME);

/*transfer struct to class to regaring c++*/
#define CLASS(__NAME)	  __##__NAME



/*implement the function of new in c++*/
#define NEW(__CLASSP, __NAME) \
	do{\
		__CLASSP = (CLASS(__NAME)*)LX_Malloc(sizeof(CLASS(__NAME)));\
		if (init_##__NAME(__CLASSP) < 0){\
			LX_Free(__CLASSP);\
		}\
	 }\
	while(0)
		
#define DELETE(__CLASSP, __NAME) \
	do{\
		if (__CLASSP != NULL){\
			((CLASS(__NAME)*)(__CLASSP))->de_init(((CLASS(__NAME)*)(__CLASSP)));\
			(__CLASSP) = NULL;\
	 	}\
	  }while(0)

/*initiate function declaration*/
#define INIT_DECLARATION(__NAME)\
	extern int init_##__NAME(CLASS(__NAME) *arg)

#endif/*__OOP_HAL_H__*/
	




	
	
	
	

	