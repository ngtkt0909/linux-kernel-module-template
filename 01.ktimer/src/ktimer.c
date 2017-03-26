/**
 * @file		ktimer.c
 * @brief		Linux Kernel Module Template
 *
 * @author		T. Ngtk
 * @copyright	Copyright (c) 2016 T. Ngtk
 *
 * @par License
 *	Released under the MIT and GPL Licenses.
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-MIT
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-GPL
 */

#include <linux/module.h>	/* MODULE_*, module_* */
#include <linux/timer.h>	/* init_timer(), add_timer(), del_timer_sync(), timer_list */

/*------------------------------------------------------------------------------
	Prototype Declaration
------------------------------------------------------------------------------*/
static int ktimerInit(void);
static void ktimerExit(void);

static int sRegisterTimer(struct timer_list *timer);
static int sUnregisterTimer(struct timer_list *timer);
static void sTimerHandler(unsigned long data);

/*------------------------------------------------------------------------------
	Defined Macros
------------------------------------------------------------------------------*/
#define D_DEV_NAME		"01.ktimer"		/**< device name */
#define D_PERIOD		(100)			/**< period of timer handler [ms] */

/*------------------------------------------------------------------------------
	Global Variables
------------------------------------------------------------------------------*/
static struct timer_list g_timer;		/**< timer_list structure */
static uint32_t g_counter;

/*------------------------------------------------------------------------------
	Macro Calls
------------------------------------------------------------------------------*/
MODULE_AUTHOR("T. Ngtk");
MODULE_LICENSE("Dual MIT/GPL");

module_init(ktimerInit);
module_exit(ktimerExit);

module_param(g_counter, int, S_IRUSR | S_IRGRP | S_IROTH);

/*------------------------------------------------------------------------------
	Functions (External)
------------------------------------------------------------------------------*/
/**
 * @brief Kernel Module Init
 *
 * @param nothing
 *
 * @retval 0		success
 * @retval others	failure
 */
static int ktimerInit(void)
{
	printk(KERN_INFO "%s loading ...\n", D_DEV_NAME);

	/* register handler on kernel timer */
	sRegisterTimer(&g_timer);

	return 0;
}

/**
 * @brief Kernel Module Exit
 *
 * @param nothing
 *
 * @retval nothing
 */
static void ktimerExit(void)
{
	printk(KERN_INFO "%s unloading ...\n", D_DEV_NAME);

	/* unregister handler from kernel timer */
	sUnregisterTimer(&g_timer);
}

/*------------------------------------------------------------------------------
	Functions (Internal)
------------------------------------------------------------------------------*/
/**
 * @brief Kernel Timer Registration
 *
 * @param [in]		timer	timer_list structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sRegisterTimer(struct timer_list *timer)
{
	int expire_tick;

	/* initialize timer_list structure */
	init_timer(timer);

	expire_tick = HZ * D_PERIOD / 1000;
	if (expire_tick == 0) {
		expire_tick = 1;
	}

	timer->data = jiffies;					/* current jiffies */
	timer->expires = jiffies + expire_tick;	/* executing handler jiffies */
	timer->function = sTimerHandler;		/* pointer to handler */

	/* register handler on kernel timer */
	add_timer(timer);

	return 0;
}

/**
 * @brief Kernel Timer Unregistration
 *
 * @param [in]		timer	timer_list structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sUnregisterTimer(struct timer_list *timer)
{
	/* unregister handler from kernel timer */
	return del_timer_sync(timer);
}

/**
 * @brief Kernel Timer Handler
 *
 * @param [in]		data	current jiffies
 *
 * @retval nothing
 */
static void sTimerHandler(unsigned long data)
{
	g_counter++;

	/* register handler on kernel timer */
	sRegisterTimer(&g_timer);
}
