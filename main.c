#define __arch_getb(a)			(*(volatile unsigned char *)(a))
#define __arch_getw(a)			(*(volatile unsigned short *)(a))
#define __arch_getl(a)			(*(volatile unsigned int *)(a))

#define __arch_putb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)		(*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))
/*
 * TODO: The kernel offers some more advanced versions of barriers, it might
 * have some advantages to use them instead of the simple one here.
 */
//#define dmb()		__asm__ __volatile__ ("" : : : "memory")
//#define __iormb()	dmb()
//#define __iowmb()	dmb()
#define __iormb()
#define __iowmb()

#define writeb(v,c)	({ unsigned char  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
#define writew(v,c)	({ unsigned short __v = v; __iowmb(); __arch_putw(__v,c); __v; })
#define writel(v,c)	({ unsigned int __v = v; __iowmb(); __arch_putl(__v,c); __v; })

#define readb(c)	({ unsigned char  __v = __arch_getb(c); __iormb(); __v; })
#define readw(c)	({ unsigned short __v = __arch_getw(c); __iormb(); __v; })
#define readl(c)	({ unsigned int __v = __arch_getl(c); __iormb(); __v; })


#define UART3_THR	(*(volatile unsigned int *)0x49020000)
#define UART3_RHR	(*(volatile unsigned int *)0x49020000)
#define UART3_LSR	(*(volatile unsigned int *)0x49020014)
#define TX_FIFO_E	(1<<5)
#define RX_FIFO_E	(1<<0)

#define OMAP34XX_CORE_L4_IO_BASE	0x48000000
#define GPTIMER_BASE1				(OMAP34XX_CORE_L4_IO_BASE + 0x318000)
#define INTC_BASE1					(OMAP34XX_CORE_L4_IO_BASE + 0x200000)

#define GPTIMER1_DEFAULT_BASE		GPTIMER_BASE1
#define INTC_DEFAULT_BASE			INTC_BASE1


#define ENABLE_GPT_PRESCALER		( 1 << 5 )
#define ENABLE_GPT_OVF_INTERRUPT	( 1 << 1 )
#define SET_GPT_PRESCALER(x) 		( x << 2 )
#define ENABLE_GPT_AUTORELOAD		( 1 << 1 )
#define ENABLE_GPT					( 1 << 0 )

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

// Prototypes
char	dputchar( char c );
int	debug( const char *s );

void enable_gptimer() {
	writel( 0xFFFF0000, &gptimer_base->TLDR );
	writel( 0x00000001, &gptimer_base->TTGR );
	writel( ENABLE_GPT_OVF_INTERRUPT, &gptimer_base->TIER );
	writel( ENABLE_GPT_PRESCALER | SET_GPT_PRESCALER( 1 ) | ENABLE_GPT_AUTORELOAD | ENABLE_GPT, &gptimer_base->TCLR );
	return;
}

int main() {

	writel( 0xFFFFFFDF, &intc_base->bla[ 1 ].INTCPS_MIR );
	enable_gptimer();

	// Now test the IO-Functions
	//char buffer[ 100 ];
	/*while( 1 ) {
		dputs( "Write a line: " );
		dgets( buffer, 100 );
		// Just echo the input
		dputs( "\n\rYou've written '" );
		dputs( buffer );
		dputs( "'\n\r" );
	}*/
	do {
		debug( "Hello world!\n\r" );
	} while( 1 );
/*
	dputchar( 'R' );
	dputchar( 'T' );
	dputchar( 'E' );
	dputchar( 'M' );
	dputchar( 'S' );
	dputchar( '\n' );
	dputchar( '\r' );
*/
	return 0;
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

// This function will wait until one character is present
// and returns this character
char dgetchar() {
	// Wait until one character is present
	while( ( UART3_LSR & RX_FIFO_E ) == 0 )
	;

	// read the character
	return UART3_RHR;
}

// This function read a line (terminated by a '\n') and check if there's no
// buffer-overflow.
// It will return the start-address of the string
char* dgets( char *s, int size ) {
	char *ptr = s;
	while( size > 1 ) {    // check if there's no buffer overflow!
		*ptr = dgetchar();
		if( *ptr == '\n' || *ptr == '\r' )   // If a newline occures, stop reading!
			break;
		ptr++;
		size--;
	}
	*ptr = 0; // generate a null-terminated string
	return s;
}
