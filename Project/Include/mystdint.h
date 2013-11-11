
/*
  Filename: mystdint.h
  Description: creates uint8_t etc for release project
*/

#ifndef __MYSTDINT_H__
# define __MYSTDINT_H__

# ifdef __INT8_T_TYPE__
typedef __INT8_T_TYPE__  sint8_t;
typedef __UINT8_T_TYPE__ uint8_t;
# endif /* __INT8_T_TYPE__ */

# ifdef __INT16_T_TYPE__
typedef __INT16_T_TYPE__  sint16_t;
typedef __UINT16_T_TYPE__ uint16_t;
#endif /* __INT16_T_TYPE__ */

# ifdef __INT32_T_TYPE__
typedef __INT32_T_TYPE__  sint32_t;
typedef __UINT32_T_TYPE__ uint32_t;
# endif /* __INT32_T_TYPE__ */

#endif /* __MYSTDINT_H__ */
