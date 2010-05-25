/*
 *  urldecode  --  filter from URL-encoded text to plain text
 *  
 *  Description: this program reads from standard input and decodes %HH
 *  sequences in their byte equivalent according to the RFC 3986.
 *  
 *  Moreover, the '+' char is translated into space, as this is the common
 *  convention still in use on the WEB.
 *  
 *  Invalid and incomplete sequences, for example "%0q" or "abc%", are
 *  left untoched.  The result is sent to the standard output.
 *  
 *  Example: "a%20b" becomes "a b".
 *  
 *  Author: Umberto Salsi <salsi@icosaedro.it>
 *  
 *  Version: 2008-04-27
 *  
 *  Updates: www.icosaedro.it/apache/urldecode.c
 *  
 *  References: Uniform Resource Identifier (URI): Generic Syntax, RFC 3986
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int c, c1, c2;
/* c is the current char being examined while c1,c2 are the next two
   following chars. */


void next()
{
	if( c == EOF )
		return;

	c = c1;
	c1 = c2;
	c2 = getchar();
}


int main()
{
	/* Initialize c,c1,c2: */
	c = getchar();
	c1 = EOF;
	c2 = EOF;
	if( c != EOF ){
		c1 = getchar();
		if( c1 != EOF ){
			c2 = getchar();
		}
	}

	while( c != EOF ){

		if( c == '%' ){

			if( isxdigit(c1) && isxdigit(c2) ){

				/* Valid %HH sequence found. */

				c1 = tolower(c1);
				c2 = tolower(c2);

				if( c1 <= '9' )
					c1 = c1 - '0';
				else
					c1 = c1 - 'a' + 10;
				if( c2 <= '9' )
					c2 = c2 - '0';
				else
					c2 = c2 - 'a' + 10;

				putchar( 16 * c1 + c2 );

				next();
				next();
				next();

			} else {

				/* Invalid or incomplete sequence. */

				putchar('%');
				next();
			}

		} else if( c == '+' ){

			putchar(' ');
			next();

		} else {

			putchar(c);
			next();

		}
	}
	
	return 0;
}

/* The end. */
