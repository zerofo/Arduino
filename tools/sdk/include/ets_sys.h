/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _ETS_SYS_H
#define _ETS_SYS_H

#include "c_types.h"
#include "eagle_soc.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This "print character function" prototype is modeled after the argument for
 * ets_install_putc1() found in "ESP8266_NONOS_SDK/include/osapi.h". This
 * deviates away from the familiar C library definition of putchar; however, it
 * agrees with the code we are working with. Note, in the ROM some "printf
 * character functions" always return 0 (uart_tx_one_char and ets_putc), some
 * return last character printed (buildin _putc1), and some return nothing
 * (ets_write_char). Using a void return type safely represents them all.
 */
typedef void (*fp_putc_t)(char);

typedef uint32_t ETSSignal;
typedef uint32_t ETSParam;

typedef struct ETSEventTag ETSEvent;

struct ETSEventTag {
    ETSSignal sig;
    ETSParam  par;
};

typedef void (*ETSTask)(ETSEvent *e);

/* timer related */
typedef uint32_t ETSHandle;
typedef void ETSTimerFunc(void *timer_arg);

typedef struct _ETSTIMER_ {
    struct _ETSTIMER_    *timer_next;
    uint32_t              timer_expire;
    uint32_t              timer_period;
    ETSTimerFunc         *timer_func;
    void                 *timer_arg;
} ETSTimer;

/* interrupt related */

// The xtos_1int handler calls with param1 as the arg, param2 as a pointer
// to an exception frame in memory.
typedef void (*int_handler_t)(void*, void*);

#define ETS_SLC_INUM        1
#define ETS_SDIO_INUM       1
#define ETS_SPI_INUM        2
#define ETS_GPIO_INUM       4
#define ETS_UART_INUM       5
#define ETS_UART1_INUM      5
#define ETS_CCOMPARE0_INUM  6
#define ETS_SOFT_INUM       7
#define ETS_WDT_INUM        8
#define ETS_FRC_TIMER1_INUM 9  /* use edge*/

// The xtos_1int handler calls with param1 as the arg, param2 as a pointer
// to an exception frame in memory.

void ets_intr_lock(void);
void ets_intr_unlock(void);

void NmiTimSetFunc(void (*func)(void));

#define ETS_INTR_LOCK() \
    ets_intr_lock()

#define ETS_INTR_UNLOCK() \
    ets_intr_unlock()

#define ETS_INTR_ENABLE(inum) \
    ets_isr_unmask((1<<inum))

#define ETS_INTR_DISABLE(inum) \
    ets_isr_mask((1<<inum))

inline bool ETS_INTR_WITHINISR()
{
    uint32_t ps;
    __asm__ __volatile__("rsr %0,ps":"=a" (ps));
    // PS.INTLEVEL check
    return ((ps & 0x0f) != 0);
}

inline uint32_t ETS_INTR_ENABLED(void)
{
    uint32_t enabled;
    __asm__ __volatile__("esync; rsr %0,intenable":"=a" (enabled));
    return enabled;
}

inline uint32_t ETS_INTR_PENDING(void)
{
    uint32_t pending;
    __asm__ __volatile__("esync; rsr %0,interrupt":"=a" (pending));
    return pending;
}

#define ETS_CCOMPARE0_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_CCOMPARE0_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_CCOMPARE0_ENABLE() \
    ETS_INTR_ENABLE(ETS_CCOMPARE0_INUM)

#define ETS_CCOMPARE0_DISABLE() \
    ETS_INTR_DISABLE(ETS_CCOMPARE0_INUM)


#define ETS_FRC_TIMER1_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_FRC_TIMER1_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_FRC_TIMER1_NMI_INTR_ATTACH(func) \
    NmiTimSetFunc(func)

#define ETS_GPIO_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_GPIO_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_GPIO_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_GPIO_INUM)

#define ETS_GPIO_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_GPIO_INUM)


#define ETS_UART_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_UART_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_UART_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_UART_INUM)

#define ETS_UART_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_UART_INUM)

#define ETS_FRC1_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_FRC_TIMER1_INUM)

#define ETS_FRC1_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_FRC_TIMER1_INUM)


#define ETS_SPI_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_SPI_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_SPI_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_SPI_INUM)

#define ETS_SPI_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_SPI_INUM)


#define ETS_SLC_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_SLC_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_SLC_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_SLC_INUM)

#define ETS_SLC_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_SLC_INUM)


#define ETS_SDIO_INTR_ATTACH(func, arg) \
    ets_isr_attach(ETS_SDIO_INUM, (int_handler_t)(func), (void *)(arg))

#define ETS_SDIO_INTR_ENABLE() \
    ETS_INTR_ENABLE(ETS_SDIO_INUM)

#define ETS_SDIO_INTR_DISABLE() \
    ETS_INTR_DISABLE(ETS_SDIO_INUM)


void *pvPortMalloc(size_t xWantedSize, const char* file, int line) __attribute__((malloc, alloc_size(1)));
void *pvPortRealloc(void* ptr, size_t xWantedSize, const char* file, int line) __attribute__((alloc_size(2)));
void vPortFree(void *ptr, const char* file, int line);
void *ets_memcpy(void *dest, const void *src, size_t n);
void *ets_memmove(void *dest, const void *src, size_t n);
void *ets_memset(void *s, int c, size_t n);
void ets_timer_arm_new(ETSTimer *a, int b, int c, int isMstimer);
void ets_timer_setfn(ETSTimer *t, ETSTimerFunc *fn, void *parg);
void ets_timer_disarm(ETSTimer *a);
int ets_strncmp(const char *s1, const char *s2, int len);
int ets_strcmp(const char *s1, const char *s2);
int ets_strlen(const char *s);
char *ets_strcpy(char *dest, const char *src);
char *ets_strncpy(char *dest, const char *src, size_t n);
char *ets_strstr(const char *haystack, const char *needle);
int ets_sprintf(char *str, const char *format, ...)  __attribute__ ((format (printf, 2, 3)));
int os_snprintf(char *str, size_t size, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
int ets_printf(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));
void ets_install_putc1(fp_putc_t routine);
void ets_isr_mask(int intr);
void ets_isr_unmask(int intr);
void ets_isr_attach(int intr, int_handler_t handler, void *arg);
void ets_intr_lock();
void ets_intr_unlock();
int ets_vsnprintf(char * s, size_t n, const char * format, va_list arg)  __attribute__ ((format (printf, 3, 0)));
int ets_vprintf(fp_putc_t print_function, const char * format, va_list arg) __attribute__ ((format (printf, 2, 0)));

/*
 * ets_putc(), a "print character function" in ROM, prints a character to a
 * UART. It always returns 0; however, the prototype here is defined with void
 * return to make compatible with other usages of fp_putc_t. ets_putc() provides
 * a "raw", print as is, interface. '\r' and '\n' are each printed exactly as is
 * w/o addition. For a "cooked" interface use ets_uart_putc1().
 * The use of this function requires a prior setup call to uart_buff_switch() to
 * select the UART.
 */
void ets_putc(char);

/*
 * ets_uart_putc1(), a "print character function" in ROM, prints a character to
 * a UART. It returns the character printed; however, the prototype here is
 * defined with void return to make compatible with other usages of fp_putc_t.
 * This function provides additional processing to characters '\r' and '\n'. It
 * filters out '\r'.  When called with '\n', it prints characters '\r' and '\n'.
 * This is sometimes refered to as a "cooked" interface. For a "raw", print as
 * is, interface use ets_putc(). The use of this function requires a prior setup
 * call to uart_buff_switch() to select the UART.
 * ets_uart_putc1() is used internally by ets_uart_printf. It is also the
 * function that gets installed by ets_uart_install_printf through a call to
 * ets_install_putc1.
 */
void ets_uart_putc1(char);

bool ets_task(ETSTask task, uint8 prio, ETSEvent *queue, uint8 qlen);
bool ets_post(uint8 prio, ETSSignal sig, ETSParam par);
void ets_update_cpu_frequency(uint32_t ticks_per_us);


#ifdef __cplusplus
}
#endif

#endif /* _ETS_SYS_H */
