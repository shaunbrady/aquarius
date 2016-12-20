/* Macros. */

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/* Some screen stuff. */
/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   24
/* The attribute of an character. */
#define ATTRIBUTE               7
/* The video memory address. */
#define VIDEO                   0xB8000

/* Variables. */
/* Save the X position. */
static int xpos;
/* Save the Y position. */
static int ypos;
/* Point to the video memory. */
static volatile unsigned char *video;
/* colors our display; starts at ASCII blue  */
int fab_color=9;

/* Forward declarations. */
void cmain ();
static void cls (void);
static void itoa (char *buf, int base, int d);
static void putchar (int c);
void printf (const char *format, ...);


/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
cmain ()
{
    //cls();
    int i;
    for(i=0;i<20;i++){
         /*printf("External Process\n"); */
        
    }
    return;
}

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
static void cls (void){
    int i;

    video = (unsigned char *) VIDEO;

    for (i = 0; i < COLUMNS * LINES * 2; i++)
        *(video + i) = 0;

    xpos = 0;
    ypos = 0;
}

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
static void itoa (char *buf, int base, int d){
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    /* If %d is specified and D is minus, put `-' in the head. */
    if (base == 'd' && d < 0){
        *p++ = '-';
        buf++;
        ud = -d;
    }
    else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0. */
    do{
        int remainder = ud % divisor;

        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }while (ud /= divisor);

    /* Terminate BUF. */
    *p = 0;

    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2){
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}


/* Put the character C on the screen. */
static void putchar (int c){
    if (c == '\n' || c == '\r'){
        newline:
        if(fab_color==0xF){
            fab_color=9;
        }
        else{fab_color++;}
        xpos = 0;
        ypos++;
        if (ypos >= LINES){
            ypos--;
            int x,y=0;
            for(y=0;y <= LINES; y++){
                for(x=0; x <= COLUMNS; x++){
                    *(video + (x + y * COLUMNS) * 2) = *(video + (x +( y+1) * COLUMNS) * 2);
                }
            }
        }
        return;
    }
    *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
    //*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;
    *(video + (xpos + ypos * COLUMNS) * 2 + 1) = fab_color;

    xpos++;
    if (xpos >= COLUMNS)
        goto newline;
}


/* Format a string and print it on the screen, just like the libc
   function printf. */
void printf (const char *format, ...){
    char **arg = (char **) &format;
    int c;
    char buf[20];

    arg++;

    while ((c = *format++) != 0){
        if (c != '%')
            putchar (c);
        else{
            char *p;

            c = *format++;
            switch (c){
                case 'd':
                case 'u':
                case 'x':
                    itoa (buf, c, *((int *) arg++));
                    p = buf;
                    goto string;
                    break;

                case 's':
                    p = *arg++;
                    if (! p)
                        p = "(null)";

                    string:
                    while (*p){
                        putchar (*p++);
                    }
                    break;

                default:
                    putchar (*((int *) arg++));
                    break;
            }
        }
    }
}

