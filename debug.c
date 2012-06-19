#define UART3_THR	(*(volatile unsigned int *)0x49020000)
#define UART3_RHR	(*(volatile unsigned int *)0x49020000)
#define UART3_LSR	(*(volatile unsigned int *)0x49020014)
#define TX_FIFO_E	(1<<5)
#define RX_FIFO_E	(1<<0)

// Prototypes
char	dputchar( char c );
int	debug( const char *s );

int main() {
	debug( "Hello world!\n\r" );
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
