/*
 *  Simple test program -- simplified version of sample test hello.
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems/irq-extension.h>

#define OMAP34XX_CORE_L4_IO_BASE	0x48000000
#define I2C_BASE1					(OMAP34XX_CORE_L4_IO_BASE + 0x070000)
#define GPTIMER_BASE1				(OMAP34XX_CORE_L4_IO_BASE + 0x318000)
#define INTC_BASE1					(OMAP34XX_CORE_L4_IO_BASE + 0x200000)

#define I2C_DEFAULT_BASE			I2C_BASE1
#define GPTIMER1_DEFAULT_BASE		GPTIMER_BASE1
#define INTC_DEFAULT_BASE			INTC_BASE1

struct gptimer {
	unsigned int TIDR;		// R  32 0x0000
	unsigned int RES[ 3 ];	// NONE { 0x0004, 0x0008, 0x000C }
	unsigned int TIOCP_CFG;	// RW 32 0x0010
	unsigned int TISTAT;	// R  32 0x0014
	unsigned int TISR;		// RW 32 0x0018
	unsigned int TIER;		// RW 32 0x001C
	unsigned int TWER;		// RW 32 0x0020
	unsigned int TCLR;		// RW 32 0x0024
	unsigned int TCRR;		// RW 32 0x0028
	unsigned int TLDR;		// RW 32 0x002C
	unsigned int TTGR;		// RW 32 0x0030
	unsigned int TWPS;		// R  32 0x0034
	unsigned int TMAR;		// RW 32 0x0038
	unsigned int TCAR1;		// R  32 0x003C
	unsigned int TSICR;		// RW 32 0x0040
	unsigned int TCAR2;		// R  32 0x0044
	unsigned int TPIR;		// RW 32 0x0048
	unsigned int TNIR;		// RW 32 0x004C
	unsigned int TCVR;		// RW 32 0x0050
	unsigned int TOCR;		// RW 32 0x0054
	unsigned int TOWR;		// RW 32 0x0058
};


struct blub {
	unsigned int INTCPS_ITR;
	unsigned int INTCPS_MIR;
	unsigned int INTCPS_MIR_CLEAR;
	unsigned int INTCPS_MIR_SET;
	unsigned int INTCPS_ISR_SET;
	unsigned int INTCPS_ISR_CLEAR;
	unsigned int INTCPS_PENDING_IRQ;
	unsigned int INTCPS_PENDING_FIQ;
};

struct intc {
	unsigned int INTCPS_REVISION;		// RW 32 0x0000
	unsigned int RESERVED1[ 3 ];		// NONE { 0x0004, 0x0008, 0x000C }
	unsigned int INTCPS_SYSCONFIG;		// RW 32 0x0010
	unsigned int INTCPS_SYSSTATUS;		// R  32 0x0014
	unsigned int RESERVED2[ 10 ];		// NONE { 0x0018, 0x001C, 0x0020, 0x0024, 0x0028, 0x002C, 0x0030, 0x0034, 0x0038, 0x003C }
	unsigned int INTCPS_SIR_IRQ;		// R  32 0x0040
	unsigned int INTCPS_SIR_FIQ;		// R  32 0x0044
	unsigned int INTCPS_CONTROL;		// RW 32 0x0048
	unsigned int INTCPS_PROTECTION;		// RW 32 0x004C
	unsigned int INTCPS_IDLE;			// RW 32 0x0050
	unsigned int RESERVED3[ 3 ];		// NONE { 0x0054, 0x0058, 0x005C }
	unsigned int INTCPS_IRQ_PRIORITY;	// RW 32 0x0060
	unsigned int INTCPS_FIQ_PRIORITY;	// RW 32 0x0064
	unsigned int INTCPS_THRESHOLD;		// RW 32 0x0068
	unsigned int RESERVED4[ 5 ];		// NONE { 0x006C, 0x0070, 0x0074, 0x0078, 0x007C, }
	struct blub bla[ 3 ];
	unsigned int RESERVED5[ 8 ];
	unsigned int INTCPS_ILR[ 96 ];		// R  32 0x0100
};

static struct gptimer *gptimer_base = (struct gptimer *)GPTIMER1_DEFAULT_BASE;
static struct intc *intc_base = (struct intc *)INTC_DEFAULT_BASE;



#define UART3_THR	(*(volatile unsigned int *)0x49020000)
#define UART3_RHR	(*(volatile unsigned int *)0x49020000)
#define UART3_LSR	(*(volatile unsigned int *)0x49020014)
#define TX_FIFO_E	(1<<5)
#define RX_FIFO_E	(1<<0)

void check_reg( unsigned int addr ) {
	unsigned int reg = *(volatile unsigned int *)addr;
	printf( "Register %p = 0x%8X\n", (void*)addr, reg );
}

// This function sends the character c over UART3
// and will return also c
char dputchar( char c ) {
	// wait until TX-Buffer is empty
	while( ( UART3_LSR & TX_FIFO_E ) == 0 )
	;

	// send character
	UART3_THR = c;
	return c;
}

// This function sends a Null-terminates String via dputchar().
// It will return the number of transmitted chars.
int debug( const char *s ) {
	int i = 0;
	while( *s != 0 ) {
		dputchar( *s++ );
		i++;
	}
	return i;
}

void test( int num ) {
  debug( "Hello world!\n\r" );
}

#define ENABLE_GPT_PRESCALER		( 1 << 5 )
#define ENABLE_GPT_OVF_INTERRUPT	( 1 << 1 )
#define SET_GPT_PRESCALER(x) 		( x << 2 )
#define ENABLE_GPT_AUTORELOAD		( 1 << 1 )
#define ENABLE_GPT					( 1 << 0 )

/*
//enable sequence
//1. Write 0xXXXXBBBB in WDTi.WSPR.
//2. Write 0xXXXX4444 in WDTi.WSPR.

//The watchdog timer overflow rate is expressed by: OVF_Rate = (0xFFFF FFFF - WDTn.WLDR + 1) * (wd-
//functional clock period) * PS

#define ENABLE_WDT_FIRST_STEP 0x0000BBBB
#define ENABLE_WDT_SECOND_STEP 0x00004444

#define DISABLE_WDT_FIRST_STEP 0x0000AAAA
#define DISABLE_WDT_SECOND_STEP 0x00005555

#define WDTIMER2_WISR	(*(volatile unsigned int *)0x48314018)
#define WDTIMER3_WISR	(*(volatile unsigned int *)0x49030018)

#define WDTIMER2_WIER	(*(volatile unsigned int *)0x4831401C)
#define WDTIMER3_WIER	(*(volatile unsigned int *)0x4903001C)

#define WDTIMER2_WCLR	(*(volatile unsigned int *)0x48314024)
#define WDTIMER3_WCLR	(*(volatile unsigned int *)0x49030024)

#define WDTIMER2_WCRR	(*(volatile unsigned int *)0x48314028)
#define WDTIMER3_WCRR	(*(volatile unsigned int *)0x49030028)

#define WDTIMER2_WTGR	(*(volatile unsigned int *)0x48314030)
#define WDTIMER3_WTGR	(*(volatile unsigned int *)0x49030030)

#define WDTIMER2_WLDR	(*(volatile unsigned int *)0x4831402C)
#define WDTIMER3_WLDR	(*(volatile unsigned int *)0x4903002C)

#define WDTIMER2_WSPR	(*(volatile unsigned int *)0x48314048)
#define WDTIMER3_WSPR	(*(volatile unsigned int *)0x49030048)

void enable_wdts() {
	WDTIMER2_WSPR = ENABLE_WDT_FIRST_STEP;
	WDTIMER3_WSPR = ENABLE_WDT_FIRST_STEP;
	udelay( 1000 );
	WDTIMER2_WSPR = ENABLE_WDT_SECOND_STEP;
	WDTIMER3_WSPR = ENABLE_WDT_SECOND_STEP;
	WDTIMER2_WIER = ENABLE_WDT_INTERRUPT;
	WDTIMER3_WIER = ENABLE_WDT_INTERRUPT;
}
void disable_wdts() {
	WDTIMER2_WSPR = DISABLE_WDT_FIRST_STEP;
	WDTIMER3_WSPR = DISABLE_WDT_FIRST_STEP;
	udelay( 1000 );
	WDTIMER2_WSPR = DISABLE_WDT_SECOND_STEP;
	WDTIMER3_WSPR = DISABLE_WDT_SECOND_STEP;
}
*/
void enable_gptimer() {
	writel( 0xFFFF0000, &gptimer_base->TLDR );
	writel( 0x00000001, &gptimer_base->TTGR );
	writel( ENABLE_GPT_OVF_INTERRUPT, &gptimer_base->TIER );
	writel( ENABLE_GPT_PRESCALER | SET_GPT_PRESCALER( 1 ) | ENABLE_GPT_AUTORELOAD | ENABLE_GPT, &gptimer_base->TCLR );
	return;
}

void print_gptimer() {
	printf( "printing timer registers for timer GPT1\n" );
	printf( "====================================================\n" );
	printf( "TIDR (addr = %p) = %X\n", (void*)&gptimer_base->TIDR, readl( &gptimer_base->TIDR ) );
	printf( "TIOCP_CFG (addr = %p) = %X\n", (void*)&gptimer_base->TIOCP_CFG, readl( &gptimer_base->TIOCP_CFG ) );
	printf( "TISTAT (addr = %p) = %X\n", (void*)&gptimer_base->TISTAT, readl( &gptimer_base->TISTAT ) );
	printf( "TISR (addr = %p) = %X\n", (void*)&gptimer_base->TISR, readl( &gptimer_base->TISR ) );
	printf( "TIER (addr = %p) = %X\n", (void*)&gptimer_base->TIER, readl( &gptimer_base->TIER ) );
	printf( "TWER (addr = %p) = %X\n", (void*)&gptimer_base->TWER, readl( &gptimer_base->TWER ) );
	printf( "TCLR (addr = %p) = %X\n", (void*)&gptimer_base->TCLR, readl( &gptimer_base->TCLR ) );
	printf( "TCRR (addr = %p) = %X\n", (void*)&gptimer_base->TCRR, readl( &gptimer_base->TCRR ) );
	printf( "TLDR (addr = %p) = %X\n", (void*)&gptimer_base->TLDR, readl( &gptimer_base->TLDR ) );
	printf( "TTGR (addr = %p) = %X\n", (void*)&gptimer_base->TTGR, readl( &gptimer_base->TTGR ) );
	printf( "TWPS (addr = %p) = %X\n", (void*)&gptimer_base->TWPS, readl( &gptimer_base->TWPS ) );
	printf( "TMAR (addr = %p) = %X\n", (void*)&gptimer_base->TMAR, readl( &gptimer_base->TMAR ) );
	printf( "TCAR1 (addr = %p) = %X\n", (void*)&gptimer_base->TCAR1, readl( &gptimer_base->TCAR1 ) );
	printf( "TSICR (addr = %p) = %X\n", (void*)&gptimer_base->TSICR, readl( &gptimer_base->TSICR ) );
	printf( "TCAR2 (addr = %p) = %X\n", (void*)&gptimer_base->TCAR2, readl( &gptimer_base->TCAR2 ) );
	//printf( "TPIR (addr = %p) = %X\n", (void*)&gptimer_base->TPIR, readl( &gptimer_base->TPIR ) );
	//printf( "TNIR (addr = %p) = %X\n", (void*)&gptimer_base->TNIR, readl( &gptimer_base->TNIR ) );
	//printf( "TCVR (addr = %p) = %X\n", (void*)&gptimer_base->TCVR, readl( &gptimer_base->TCVR ) );
	//printf( "TOCR (addr = %p) = %X\n", (void*)&gptimer_base->TOCR, readl( &gptimer_base->TOCR ) );
	//printf( "TOWR (addr = %p) = %X\n", (void*)&gptimer_base->TOWR, readl( &gptimer_base->TOWR ) );

}

void print_intc() {
	int i;
	printf( "printing timer registers for timer INTC\n" );
	printf( "====================================================\n" );
	printf( "INTCPS_REVISION (addr = %p) = %X\n", (void*)&intc_base->INTCPS_REVISION, readl( &intc_base->INTCPS_REVISION ) );
	printf( "INTCPS_SYSCONFIG (addr = %p) = %X\n", (void*)&intc_base->INTCPS_SYSCONFIG, readl( &intc_base->INTCPS_SYSCONFIG ) );
	printf( "INTCPS_SYSSTATUS (addr = %p) = %X\n", (void*)&intc_base->INTCPS_SYSSTATUS, readl( &intc_base->INTCPS_SYSSTATUS ) );
	printf( "INTCPS_SIR_IRQ (addr = %p) = %X\n", (void*)&intc_base->INTCPS_SIR_IRQ, readl( &intc_base->INTCPS_SIR_IRQ ) );
	printf( "INTCPS_SIR_FIQ (addr = %p) = %X\n", (void*)&intc_base->INTCPS_SIR_FIQ, readl( &intc_base->INTCPS_SIR_FIQ ) );
	printf( "INTCPS_CONTROL (addr = %p) = %X\n", (void*)&intc_base->INTCPS_CONTROL, readl( &intc_base->INTCPS_CONTROL ) );
	printf( "INTCPS_PROTECTION (addr = %p) = %X\n", (void*)&intc_base->INTCPS_PROTECTION, readl( &intc_base->INTCPS_PROTECTION ) );
	printf( "INTCPS_IDLE (addr = %p) = %X\n", (void*)&intc_base->INTCPS_IDLE, readl( &intc_base->INTCPS_IDLE ) );
	printf( "INTCPS_IRQ_PRIORITY (addr = %p) = %X\n", (void*)&intc_base->INTCPS_IRQ_PRIORITY, readl( &intc_base->INTCPS_IRQ_PRIORITY ) );
	printf( "INTCPS_FIQ_PRIORITY (addr = %p) = %X\n", (void*)&intc_base->INTCPS_FIQ_PRIORITY, readl( &intc_base->INTCPS_FIQ_PRIORITY ) );
	printf( "INTCPS_THRESHOLD (addr = %p) = %X\n", (void*)&intc_base->INTCPS_THRESHOLD, readl( &intc_base->INTCPS_THRESHOLD ) );
	for( i = 0; i < 3; i++ ) {
		printf( "INTCPS_ITR%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_ITR, readl( &intc_base->bla[ i ].INTCPS_ITR ) );
		printf( "INTCPS_MIR%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_MIR, readl( &intc_base->bla[ i ].INTCPS_MIR ) );
		printf( "INTCPS_MIR_CLEAR%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_MIR_CLEAR, readl( &intc_base->bla[ i ].INTCPS_MIR_CLEAR ) );
		printf( "INTCPS_MIR_SET%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_MIR_SET, readl( &intc_base->bla[ i ].INTCPS_MIR_SET ) );
		printf( "INTCPS_ISR_SET%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_ISR_SET, readl( &intc_base->bla[ i ].INTCPS_ISR_SET ) );
		printf( "INTCPS_ISR_CLEAR%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_ISR_CLEAR, readl( &intc_base->bla[ i ].INTCPS_ISR_CLEAR ) );
		printf( "INTCPS_PENDING_IRQ%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_PENDING_IRQ, readl( &intc_base->bla[ i ].INTCPS_PENDING_IRQ ) );
		printf( "INTCPS_PENDING_FIQ%i (addr = %p) = %X\n", i, (void*)&intc_base->bla[ i ].INTCPS_PENDING_FIQ, readl( &intc_base->bla[ i ].INTCPS_PENDING_FIQ ) );
	}
	for( i = 0; i < 96; i++ ) {
		printf( "INTCPS_ILR%i (addr = %p) = %X\n", i, (void*)&intc_base->INTCPS_ILR[ i ], readl( &intc_base->INTCPS_ILR[ i ] ) );
	}
}

static int counter = 0;

rtems_isr my_isr( rtems_vector_number vector ) {
	counter++;
	printf( "I am my_isr handler, i handle the timer interrupt! I have been called %i times now!\n ", counter );
}


static void timer_interrupt_handler_install( int number ) {
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    number,
    "my_isr",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) my_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

struct exception_vector {
	unsigned int regs[ 14 ]; //0x4020FFC8, 0x4020FFCC, 0x4020FFD0, 0x4020FFD4, 0x4020FFD8, 0x4020FFDC, 0x4020FFE0, 0x4020FFE4, 0x4020FFE8, 0x4020FFEC, 0x4020FFF0, 0x4020FFF4, 0x4020FFF8, 0x4020FFFC
};

#define EXVEC_DEFAULT_BASE_RAM (0x4020FFC8)
#define EXVEC_DEFAULT_BASE_ROM (0x00014008)

static struct exception_vector *exvec_base_ram = (struct exception_vector *)EXVEC_DEFAULT_BASE_RAM;
static struct exception_vector *exvec_base_rom = (struct exception_vector *)EXVEC_DEFAULT_BASE_ROM;

void bla() {

	int i;

	unsigned int instructions[ 14 ] = { 0xe5841000, 0xe59f401c, 0xe5842000, 0xe12fff1e, 0xfd000070, 0xd8004d40, 0xd8004a20, 0xd8004a10, 0xfd0000a4, 0xfd00009c, 0xfd0000a0, 0xfd000064, 0xfd000060, 0x07ffffff };
	//unsigned int isr_addr = 0x800044A8;
	unsigned int reg_addr = 0x4020FFC8;

	printf( "printing the rom exception vector table!\n" );
	printf( "====================================================\n" );
	for( i = 0; i < 14; i++ ) {
		printf( "regs%i (addr = %p) = %X\n", i, (void*)&exvec_base_rom->regs[ i ], readl( &exvec_base_rom->regs[ i ] ) );
	}

	for( i = 0; i < 14; i++, reg_addr+=4 ) {
		writel( instructions[ i ], reg_addr );
		printf( "regs%i (addr = %p) = %X\n", i, (void*)&exvec_base_ram->regs[ i ], readl( &exvec_base_ram->regs[ i ] ) );
	}
	printf( "printing the ram exception vector table!\n" );
	printf( "====================================================\n" );
	for( i = 0; i < 14; i++ ) {
		printf( "regs%i (addr = %p) = %X\n", i, (void*)&exvec_base_ram->regs[ i ], readl( &exvec_base_ram->regs[ i ] ) );
	}

}

rtems_task Init( rtems_task_argument ignored ) {

	//printk( "*** HELLO WORLD TEST PRINTK ***\n" );
	//debug( "*** HELLO WORLD TEST DEBUG ***\n" );
	printf( "reg (addr = %p) = %X\n", (void*)0x00000018, readl( 0x00000018 ) );
	int j = 0;
	for( j = 17; j < 96; j++ ) {
		timer_interrupt_handler_install( j );
	}

	bla();
	print_gptimer();
	print_intc();

	writel( 0xFFFFFFDF, &intc_base->bla[ 1 ].INTCPS_MIR );

	enable_gptimer();
	rtems_interrupt_level level;
	rtems_interrupt_disable( level );

	print_gptimer();
	print_intc();

/*  disable_wdts();

  WDTIMER2_WCLR = ENABLE_WDT_PRESCALER | SET_WDT_PRESCALER( 1 );
  WDTIMER3_WCLR = ENABLE_WDT_PRESCALER | SET_WDT_PRESCALER( 1 );

  WDTIMER2_WLDR = 0xFFFFFFF0;
  WDTIMER3_WLDR = 0xFFFFFFF0;

  WDTIMER2_WTGR = 0x00000001;
  WDTIMER3_WTGR = 0x00000001;

  check_reg( 0x48314024 );
  check_reg( 0x49030024 );
  check_reg( 0x48314030 );
  check_reg( 0x49030030 );
  check_reg( 0x4831402C );
  check_reg( 0x4903002C );
  check_reg( 0x48314018 );
  check_reg( 0x49030018 );

  enable_wdts();

  printf( "WDTIMER2_WCRR = %X\n", WDTIMER2_WCRR );
  printf( "WDTIMER3_WCRR = %X\n", WDTIMER2_WCRR );*/

  /*i2c_init( CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE );

  imw( 0x4B, 0x29, 0x00 );

  imw( 0x4B, 0x1C, 0x36 );
  imw( 0x4B, 0x1D, 0x54 );
  imw( 0x4B, 0x1E, 0x20 );
  imw( 0x4B, 0x1F, 0x07 );
  imw( 0x4B, 0x20, 0x08 );
  imw( 0x4B, 0x21, 0x12 );

  imw( 0x4B, 0x29, 0x01 );

  imd( 0x4B, 0x2A, 1 );*/


	int i;
	for( i = 0; i < 10; i++ ) {
		//udelay( 500000 );
		//imd( 0x4B, 0x1C, 10 );
		//printf( "WDTIMER2_WCRR = %X\n", WDTIMER2_WCRR );
		//printf( "WDTIMER3_WCRR = %X\n", WDTIMER2_WCRR );
		//check_reg( 0x48314018 );
		//check_reg( 0x49030018 );
		print_gptimer();
		print_intc();
	}
	rtems_interrupt_enable( level );
	print_gptimer();
	print_intc();

/*
  imw 0x4A 0x0D 0x00
  imw 0x4A 0x00 0x36
  imw 0x4A 0x01 0x54
  imw 0x4A 0x02 0x90
  imw 0x4A 0x03 0x05
  imw 0x4A 0x04 0x09
  imw 0x4A 0x05 0x97
  imw 0x4A 0x0D 0x01
  imd 0x4A 0x00 10
*/

  //printf( "*** HELLO WORLD TEST PRINTF ***\n" );
  exit( 0 );
}

/* configuration information */

#define CONFIGURE_CONFDEFS_DEBUG

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
