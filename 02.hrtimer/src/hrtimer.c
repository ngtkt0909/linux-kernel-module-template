/**
 * @file		hrtimer.c
 * @brief		Linux Kernel Module Template
 *
 * @author		T. Ngtk
 * @copyright	Copyright (c) 2017 T. Ngtk
 *
 * @par License
 *	Released under the MIT and GPL Licenses.
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-MIT
 *	- https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-GPL
 */

#include <linux/module.h>	/* MODULE_*, module_* */
#include <linux/hrtimer.h>	/* hrtimer_init(), hrtimer_start(), hrtimer_cancel(), hrtimer_forward() */
#include <linux/ktime.h>	/* ktime_set(), ktime_get() */

/*------------------------------------------------------------------------------
	Prototype Declaration
------------------------------------------------------------------------------*/
static int hrtimerInit(void);
static void hrtimerExit(void);

static int sRegisterTimer(struct hrtimer *timer);
static int sUnregisterTimer(struct hrtimer *timer);
static enum hrtimer_restart timer_handler(struct hrtimer *timer);

/*------------------------------------------------------------------------------
	Defined Macros
------------------------------------------------------------------------------*/
#define D_DEV_NAME	"02.hrtimer"				/**< device name */
#define D_PERIOD	(100000)					/**< period of timer handler [ns] */
#define D_KTIME		ktime_set(0, D_PERIOD)		/**< period of timer handler (ktime) */

/*------------------------------------------------------------------------------
	Global Variables
------------------------------------------------------------------------------*/
static struct hrtimer g_timer;					/**< hrtimer structure */
static int g_counter;

/*------------------------------------------------------------------------------
	Macro Calls
------------------------------------------------------------------------------*/
MODULE_AUTHOR("T. Ngtk");
MODULE_LICENSE("Dual MIT/GPL");

module_init(hrtimerInit);
module_exit(hrtimerExit);

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
static int hrtimerInit(void)
{
	printk(KERN_INFO "%s loading ...\n", D_DEV_NAME);

	/* register handeler on hrtimer */
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
static void hrtimerExit(void)
{
	printk(KERN_INFO "%s unloading ...\n", D_DEV_NAME);

	/* unregister handler from hrtimer */
	sUnregisterTimer(&g_timer);
}

/*------------------------------------------------------------------------------
	Functions (Internal)
------------------------------------------------------------------------------*/
/**
 * @brief High-resolution Timer Registration
 *
 * @param [in]		timer	hrtimer structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sRegisterTimer(struct hrtimer *timer)
{
	/* initialize hrtimer structure */
	//hrtimer_init(timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	hrtimer_init(timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer->function = timer_handler;

	/* start handler */
	hrtimer_start(timer, D_KTIME, HRTIMER_MODE_REL);

	return 0;
}

/**
 * @brief High-resolution Timer Unregistration
 *
 * @param [in]		timer	hrtimer structure
 *
 * @retval 0		success
 * @retval others	failure
 */
static int sUnregisterTimer(struct hrtimer *timer)
{
	/* stop handler */
	return hrtimer_cancel(timer);
}

/**
 * @brief High-resolution Timer Handler
 *
 * @param [in]		timer	hrtimer structure
 *
 * @retval HRTIMER_RESTART	repeat periodically execution
 */
static enum hrtimer_restart timer_handler(struct hrtimer *timer)
{
	ktime_t now;

	/*  */
	g_counter++;

	/* current time (case of using 'CLOCK_REALTIME') */
	//now = hrtimer_cb_get_time(timer);
	/* current time (case of using 'CLOCK_MONOTONIC') */
	now = ktime_get();

	/* forward timer */
	hrtimer_forward(timer, now, D_KTIME);

	/* restart handler execution */
	return HRTIMER_RESTART;
}
