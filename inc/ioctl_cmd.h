#ifndef IOCTL_CMD_H_
#define IOCTL_CMD_H_

/*!
 * \file 	ioctl_cmd.h
 * \brief 	Header file for the module
 * \author 	O.D
 * \version 0.1
 * \date 	September 4th, 2018
 */
 
#include <linux/ioctl.h>

#define MOD_VER "0.0.0"
#define MOD_LIC "GPL"
#define MOD_DESCRIPT "test module using ioctl"
#define MOD_ALIAS "perso_ioctl"

#define FIRST_MINOR 0
#define MINOR_CNT 1

typedef struct
{
	int status;
	int action;
} mod_arg_t;

#define MOD_GET_VAR _IOR('m', 1, mod_arg_t *)
#define MOD_CLR_VAR _IO('m', 2)
#define MOD_SET_VAR _IOW('m', 3, mod_arg_t *)

int ioctl_init(void);
void ioctl_exit(void);

#endif
