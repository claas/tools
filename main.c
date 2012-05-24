#define UART3_THR	(*(volatile unsigned int *)0x49020000)
#define UART3_RHR	(*(volatile unsigned int *)0x49020000)
#define UART3_LSR	(*(volatile unsigned int *)0x49020014)
#define TX_FIFO_E	(1<<5)
#define RX_FIFO_E	(1<<0)

// Prototypes
char	dputchar( char c );
int	dputs( const char *s );
char	dgetchar( void );
char*	dgets( char *s, int size );

int main() {
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
	dputchar( 'R' );
	dputchar( 'T' );
	dputchar( 'E' );
	dputchar( 'M' );
	dputchar( 'S' );
	dputchar( '\n' );
	dputchar( '\r' );
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
int dputs( const char *s ) {
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
