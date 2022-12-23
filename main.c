#include <malloc.h>
#include <ncurses.h>
#include <dirent.h>


typedef struct {
    unsigned width, height;
} Ui;

typedef struct {
    enum {
        REGULAR,
        DIRECTORY,
    } kind;
    char *name;
} File;

typedef struct {
    size_t size;
    unsigned length;
    void **elements;
} List;

Ui *newUi(unsigned width, unsigned height) {
    Ui *ui = malloc(sizeof(Ui));
    ui->width = width;
    ui->height = height;
    return ui;
}

File *newFile(int kind, char *name) {
    File *file = malloc(sizeof(File));
    file->kind = kind;
    file->name = name;
    return file;
}

List *newList(size_t size) {
    List *list = malloc(sizeof(List));
    list->size = size;
    list->length = 0;
    list->elements = (void *) malloc(list->size);
    return list;
}

int listAppend(List *list, void *element) {
    list->elements = realloc(list->elements, list->size + (list->length * sizeof(element)));
    list->elements[list->length++] = element;
    return list->length;
}

List *listFiles(void) {
    List *files = newList(sizeof(File));
    DIR *directory = opendir(".");
    struct dirent *dir;
    if (!directory) return NULL;
    while ((dir = readdir(directory)) != NULL) {
        printf("%s\n", dir->d_name);
    }
    closedir(directory);
    return files;
}

void setup(void) {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, 1);
}

void draw(void) {
    unsigned y, x;
    getmaxyx(stdscr, y, x);
    Ui *ui = newUi(y, x);
    printw("(%d, %d)", ui->width, ui->height);
    getch();
}

int main(void) {
//    setup();
//    draw();
//    endwin();
    listFiles();
    return 0;
}
