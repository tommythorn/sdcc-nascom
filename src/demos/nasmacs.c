/*
 * Nasmacs editor
 *
 * A small editor for NASCOM
 *
 * Features:
 *
 * - access the screen memory directly for speed
 * - 16 lines: use the header line for editing also
 * - Compact: buffer is just the dense text + space for insertion
 * - Copy-paste
 * 
 * Keybinding (EMACS compatible):
 *   Arrow keys move as expected.
 *   Non-ctrl keys insert themselves.
 *   Ctrl-A: move to the beginning of the line.
 *   Ctrl-B: move backwards one char (= arrow left).
 *   Ctrl-D: delete the char under the cursor.
 *   Ctrl-E: move to the end of the line.
 *   Ctrl-F: move forward one char (= arrow right).
 *   Ctrl-H: backspace, delete the char before the cursor.
 *   Ctrl-K: Kill the line to the right of the cursor and store it in
 *           the kill buffer.  Repeated invocations appends to the
 *           kill buffer.
 *   Ctrl-N: next line (= arrow down).
 *   Ctrl-O: open line, inserts a newline after the cursor.
 *   Ctrl-P: previous line (= arrow up).
 *   Ctrl-Y: yank, inserts the contents of the kill-buffer.
 *
 * Expressed in the Model-View-Controller paradigm:
 *
 * - The model is text buffer along with two pointers which divides
 *   the buffer into three sections: everything before the insert
 *   cursor, available space, everything after the insert cursor.
 *   This is a classic model that allows constant-cost insertion
 *   and deletion at the code of linear cost insert cursor movement.
 *   (Note, insert cursor movements can be deferred until needed,
 *   but currently this isn't implemented for simplicity).
 *
 * - The view is handled exclusively by the view_update() function which
 *   currently rewrites the whole screen everytime, but a more
 *   sophisticated implementation will use past state and allow the
 *   controller to leave hints to minimize how much work it needs to
 *   do.
 *
 * - The controller translates keyboard input into model changes, eg.
 *   inserts a character into the buffer and updates the pointer.  The
 *   controller _never_ updates the screen.
 */


/*
OMGs: You cannot enter ` nor ~ on the Nascom keyboard?
A: @ is 0x40 - shift-@  (but I don't have a @ key!)
   ` is 0x60 - press Ctrl-space
   ~ is 0x7e - press Ctrl-> (greater-than)
   Instead of # the symbol £ is in its place, but # is at 0x9E
   \ is 0x5C - ?
*/
#include <stdio.h>
#include <string.h>

void exit(void);

/*
 * The "Model", that is, the editor state.
 */

#define BUF_SIZE 8192

char buffer[BUF_SIZE];
// SDCC BUG: gives compilation error
// demos/editor2.c:183: error 44: compare operand cannot be struct/union
// comparing type char [8192] fixed to type char generic* fixed
//   #define buffer_start buffer
//   #define buffer_start &buffer[0]
char *buffer_start = buffer;
char *buffer_end   = buffer + sizeof buffer;
char *buffer_cursor, *buffer_tail;
char *display_text_start;
int display_text_start_line = 0;

/* x,y can be derived from buffer_cursor, but is maintained incrementally */
unsigned buffer_x, buffer_y;
unsigned target_x; // Up- and down-motion targets to coloumn

/* Kill buffer (~ clip-board) */
char kill_buffer[256];
#define kill_buffer_end (kill_buffer + sizeof kill_buffer)
char *kill_cursor = kill_buffer;

void view_update()
{
    // SDCC BUG: neither of these compile correctly
    //static char **screen_cursor = (char **) 0xc29;
    //static const char **screen_cursor = (char **) 0xc29;
#define screen_cursor ((char **) 0xc29)
#define screen_start ((char *)0x80A)

    char *s = display_text_start;
    char *d = screen_start + 64 * 15;
    int next = -64 * 15;
    unsigned x, y;

    for (y = 0; y < 16; ++y) {
        for (x = 0;; ++s) {
            if (s == buffer_cursor) {
                s = buffer_tail;
                *screen_cursor = d + x;
            }

            if (s == buffer_end || *s == '\n')
                break;

            if (x < 48)
                d[x++] = *s;
        }

        memset(d + x, ' ', 48 - x);
        d += next;
        next = 64;

        if (s != buffer_end)
            ++s;
    }

    {
	char *saved = *screen_cursor;
	*screen_cursor = screen_start + 64 * 15 + 40;
	printf("L%d C%d", display_text_start_line + buffer_y, buffer_x);
	*screen_cursor = saved;
    }
}

void insert(char ch)
{
    *buffer_cursor++ = ch;
    if (ch == '\n') {
	buffer_x = 0, ++buffer_y;
	    if (display_text_start_line + 15 < buffer_y) {
		++display_text_start_line;
		do
		    ++display_text_start;
		while (display_text_start[-1] != '\n');
	    }
    }
    else
	++buffer_x;
}

void append(char *s)
{
    for (; *s; ++s)
	insert(*s);
}

unsigned get_line_length()
{
    char *p;

    for (p = buffer_cursor; p != buffer_start && p[-1] != '\n'; --p)
	;

    return buffer_cursor - p;
}

void move_backwards()
{
    if (buffer_start < buffer_cursor) {
        *--buffer_tail = *--buffer_cursor;
	if (*buffer_tail == '\n') {
	    --buffer_y, buffer_x = get_line_length();
	    if (buffer_y < display_text_start_line) {
		// XXX depends on buffer_cursor == cursor
		--display_text_start_line;
		do
		    --display_text_start;
		while (*display_text_start != '\n');
	    }
	}
	else
	    --buffer_x;
    }
}

void move_forwards()
{
    if (buffer_tail < buffer_end) {
        *buffer_cursor++ = *buffer_tail++;
	if (buffer_cursor[-1] == '\n') {
	    ++buffer_y, buffer_x = 0;
	    if (display_text_start_line + 15 < buffer_y) {
		++display_text_start_line;
		do
		    ++display_text_start;
		while (display_text_start[-1] != '\n');
	    }
	}
	else
	    ++buffer_x;
    }
}

void move_beginning_of_line()
{
    while (buffer_cursor != buffer_start && buffer_x != 0)
	move_backwards();
}

void move_up()
{
    move_beginning_of_line();
    move_backwards();
    while (buffer_cursor != buffer_start && target_x < buffer_x)
        move_backwards();
}

void move_end_of_line()
{
    while (buffer_tail != buffer_end && *buffer_tail != '\n')
	move_forwards();
}

void move_down()
{
    move_end_of_line();
    move_forwards();
    while (buffer_tail != buffer_end && buffer_x < target_x && *buffer_tail != '\n')
        move_forwards();
}

void backspace()
{
    if (buffer_start != buffer_cursor) {
	move_backwards();
	++buffer_tail;
    }
}

void delete_forward()
{
    if (buffer_tail != buffer_end) {
	move_forwards();
	backspace();
    }
}

void kill_line(int is_appending)
{
    if (buffer_tail == buffer_end)
	return;

    if (!is_appending)
	kill_cursor = kill_buffer;

    if (*buffer_tail == '\n')
	*kill_cursor++ = *buffer_tail++;
    else
	do
	    *kill_cursor++ = *buffer_tail++;
	while (kill_cursor != kill_buffer_end &&
	       buffer_tail != buffer_end &&
	       *buffer_tail != '\n');
}

void yank()
{
    char *p;

    for (p = kill_buffer; p != kill_cursor; ++p)
	insert(*p);
}

void init()
{
    buffer_cursor = display_text_start = buffer_start;
    buffer_tail = buffer_end;
    buffer_x = buffer_y = 0;

#if 0
    append("\n"
           "A\n"
           "AA\n"
           "AAA\n"
           "\n"
           "Lorem ipsum dolor sit amet, consectetur\n"
           "adipiscing elit, sed do eiusmod\n"
           "tempor incididunt ut labore et dolore magna\n"
           "aliqua. Ut enim ad minim veniam,\n"
           "quis nostrud exercitation ullamco laboris nisi\n"
           "ut aliquip ex ea commodo\n"
           "consequat.\n"
           "\n"
           "!\n"
           "!\n"
           "!\n"
           "!\n"
           "!\n"
           "!\n"
           "!\n"
           "!\n"
           "\n"
           "Duis aute irure dolor in reprehenderit in\n"
           "voluptate velit esse\n"
           "cillum dolore eu fugiat nulla pariatur.\n"
           "Excepteur sint occaecat cupidatat non\n"
           "proident, sunt in culpa qui officia deserunt\n"
           "mollit anim id est laborum.\n"
           "\n"
           "#include <stdio.h>\n"
           "int main(int argc, char **argv)\n"
           "{\n"
           "    printf(\"Hello World!\\n\");\n"
           "    return 0;\n"
           "}\n"
           "\n"
           "\n"
           "And there was Light!\n"
           "\n"
           );


    {
	int ch;

	for (ch = ' '; ch <= 255; ++ch) {
	    insert(ch);
	    if ((ch & 31) == 31)
		insert('\n');
	}
    }
#endif
}

int main()
{
    int previous_ch = 0;
    init();

    for (;;) {
	int ch;
	int vertical_move = 0;

        view_update();

	switch (ch = getchar()) {
	default:
	    if (' ' <= ch || ch == '\n')
		insert(ch);
	    else {
		printf("*** ch = %d (0x%x) ***", ch, ch);
		getchar();
	    }
	    break;
	case 0x01: // ^A, move-beginning-of-line
	    move_beginning_of_line();
	    break;
	case 0x02: // ^B, backward-char
        case 0x11:
            move_backwards();
	    break;
	case 0x04: // ^D, delete-forward
	    delete_forward();
	    break;
	case 0x05: // ^E, move-end-of-line
	    move_end_of_line();
	    break;
        case 0x06: // ^F, forward-char
        case 0x12:
            move_forwards();
	    break;
	case 0x08: // ^H, backspace
            backspace();
	    break;
	case 0x10: // ^P, previous-line
        case 0x13:
            move_up();
	    vertical_move = 1;
	    break;
	case 0x0b: // ^K, kill-line
	    kill_line(previous_ch == ch);
	    break;
	case 0x0e: // ^N, next-line
	case 0x14:
            move_down();
	    vertical_move = 1;
	    break;
	case 0x0F: // ^O, open-line
	    insert('\n');
	    move_backwards();
	    break;
	case 0x19: // ^Y, yank
	    yank();
	    break;
	}

	if (!vertical_move)
	    target_x = buffer_x;

	previous_ch = ch;
    }
}
