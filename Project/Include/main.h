
/*
  Filename: main.h
  Description: ---
*/

#ifndef __MAIN_H__
# define __MAIN_H__

//# define MAIN_STATE_UNINIT		(0)
# define MAIN_STATE_INIT		      (0)
# define MAIN_STATE_INIT_MASTER		(1)
# define MAIN_STATE_INIT_CHILD		(2)
# define MAIN_STATE_COM	        	(3)
 #define MAIN_STATE_COM_SYNCUP    (4)

#define SetMainState(newstate) ClearEvent(~((EventMaskType)0)); \
                               cycles = 0; \
                               mainstate = (newstate);
                              
extern uint8_t MyID;

int main(void);

#endif /* __MAIN_H__ */
