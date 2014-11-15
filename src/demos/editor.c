/*
 * Let's make an editor like Nascom always deserved (sorry Anders, BLS
 * Pascal had a good one, but I want better).
 *
 * Features:
 * - FAST: access the screen memory directly
 * - 16 lines: use the crazy header line
 * - Compact: buffer is just the dense text + space for insertion
 * - Initial features: cursor navigation, self-insert charactors,
 *   delete, page up, page down.
 *
 * The text is stored in the buffer with everything before the
 * insertion cursor residing from beginning of buffer, everything
 * after the cursor is flushed to the end of the buffer, and the hole
 * in the middle is space for inserted text.  As the insertion cursor
 * is moved, text is moved between the two halves.
 *
 * Lines are limited to 127 characters.  This enables us to prefix
 * every line with a byte giving the length of the line.  This byte
 * has bit 7 set so we can distinguish it when scanning backwards.
 *
 * Step 1: display buffer and page down and up.
 */


/*
OMGs: You cannot enter ` nor ~ on the Nascom keyboard?
A: @ is 0x40 - shift-@  (but I don't have a @ key!)
   ` is 0x60 - press Ctrl-space
   ~ is 0x7e - press Ctrl-> (greater-than)
   Instead of # the symbol £ is in its place, but # is at 0x9E
*/
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 8192

static char text_buffer[BUF_SIZE];
static char *text_end;
static int last_none_blank[16];
static char *display_text_start;

void refresh()
{
    int y;
    char *s = display_text_start;
    char *d = (char *)0x80A + 64 * 15;
    int next = -64 * 15;

    // Optimize later: s == text_end isn't necessary if buffer is
    // padded with 16 newlines
    // + instead of filling the whole line, track the length of the
    // non-blank displayed line
    // + A different representation could be faster, eg. if lines were
    // limited to 128 and all lines prefixed with a byte giving the
    // length + 128 (to allow scanning backwards as well).

    for (y = 0; y < 16; ++y) {
        unsigned l = 0, n = 0;

        if (s < text_end) {
            l = (unsigned char) *s++ - 0x80;
            n = l <= 48 ? l : 48;
            memcpy(d, s, n);
            s += l;
        }

        memset(d + n, ' ', 48 - n);
        d += next;
        next = 64;
    }
}

void append(char *s)
{
    char *l = text_end++;

    for (;; ++s) {
        if (*s == 0 || *s == '\n') {
            *l = 0x80 + text_end - l - 1;
            if (*s == 0)
                return;
            l = text_end++;
            ++s;
        }
        else
            *text_end++ = *s;
    }
}

void init()
{
    text_end = display_text_start = text_buffer;

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

}


int main()
{
    int ch;

    init();

    while (display_text_start < text_end) {
        refresh();
        ch = getchar();
        display_text_start += (unsigned char) *display_text_start - 0x7F;
    }

    printf("\nText buffer at %04x\n", (int) text_buffer);

    return 0;
}
