#include <malloc.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define NORMAL_PAIR  0
#define SPECIAL_PAIR 1
#define EXIT_PAIR    2

static unsigned former_position = 4;

typedef struct {
    bool exit;
    unsigned width, height, curY, curX;
    char *textBarValue;
} Ui;

typedef struct {
    enum {
        REGULAR,
        DIRECTORY,
    } kind;
    char *name;
} File;

typedef struct {
    unsigned length;
    File **elements;
} List;

Ui *newUi(unsigned width, unsigned height) {
    Ui *ui = malloc(sizeof(Ui));
    ui->exit = false;
    ui->width = width;
    ui->height = height;
    ui->curY = 1;
    ui->curX = 1;
    ui->textBarValue = malloc(sizeof(char));
    return ui;
}

File *newFile(int kind, char *name) {
    File *file = malloc(sizeof(File));
    file->kind = kind;
    file->name = name;
    return file;
}

List *newList(void) {
    List *list = malloc(sizeof(List));
    list->length = 0;
    list->elements = (File **) malloc(sizeof(File));
    return list;
}

int listAppend(List *list, File *element) {
    list->elements = realloc(list->elements, (sizeof(list->elements) + sizeof(File)) * sizeof(File));
    list->elements[list->length++] = element;
    return list->length;
}

List *listFiles(void) {
    List *files = newList();
    DIR *directory = opendir(".");
    struct dirent *dir;
    if (!directory) return NULL;
    while ((dir = readdir(directory)) != NULL) {
        int kind = REGULAR;
        if (dir->d_type == DT_DIR) kind = DIRECTORY;
        File *file = newFile(kind, dir->d_name);
        listAppend(files, file);
    }
    closedir(directory);
    return files;
}

void setup(void) {
    initscr();
    noecho();
    keypad(stdscr, 1);
    start_color();
    init_pair(NORMAL_PAIR,   COLOR_WHITE, COLOR_BLACK);
    init_pair(SPECIAL_PAIR,  COLOR_GREEN, COLOR_BLACK);
    init_pair(EXIT_PAIR,     COLOR_RED,   COLOR_BLACK);
}

void drawFiles(Ui *ui) {
    attron(COLOR_PAIR(NORMAL_PAIR));
    ui->curY = 3;
    ui->curX = 1;
    int extras = 0;
    List *files = listFiles();
    for (unsigned int i = 0; i < files->length; i++) {
        if (strlen(ui->textBarValue) == 0) {
            if (ui->curX - extras >= ui->width) {
                ui->curY += 2;
                ui->curX = 1;
            }
            mvaddstr(ui->curY, ui->curX, files->elements[i]->name);
            ui->curX += strlen(files->elements[i]->name) + 4;
            extras += 4;
        } else {
            if (strncmp(files->elements[i]->name, ui->textBarValue, (int) (strlen(files->elements[i]->name) / 2)) == 0)
            mvaddstr(ui->curY, ui->curX, files->elements[i]->name);
            extras += 4;
        }
    }
    attroff(COLOR_PAIR(NORMAL_PAIR));
}

void drawTextbar(Ui *ui) {
    ui->curY += 2;
    ui->curY = ui->height - 2;
    ui->curX = 1;
    mvaddstr(ui->curY, ui->curX, ">>");
    move(ui->curY, former_position);
}

void draw(void) {
    unsigned y, x;
    getmaxyx(stdscr, x, y);
    Ui *ui = newUi(y, x);
    while (!ui->exit) {
        clear();
        ui->curY = 1;
        attron(COLOR_PAIR(EXIT_PAIR));
        mvaddstr(ui->curY, ui->curX, "press ESC to exit");
        attroff(COLOR_PAIR(EXIT_PAIR));
        ui->curY = ui->height - 4;
        attron(COLOR_PAIR(SPECIAL_PAIR));
        mvaddstr(ui->curY, ui->curX, "type to find file(s)");
        attroff(COLOR_PAIR(SPECIAL_PAIR));
        drawFiles(ui);
        drawTextbar(ui);
        mvaddstr(ui->height - 2, 4, ui->textBarValue);
        unsigned key = getch();
        switch (key) {
            case 27:
                ui->exit = true;
                break;
            case KEY_BACKSPACE:
                if (former_position < 5) break;
                mvdelch(ui->curY, --former_position);
                ui->textBarValue[strlen(ui->textBarValue) - 1] = 0;
                break;
            case 10:
                break;
            default:
                mvaddch(ui->curY, former_position++, key);
                ui->textBarValue = realloc(ui->textBarValue, (strlen(ui->textBarValue) + 2) * sizeof(char));
                strcat(ui->textBarValue, (char []) {key, 0});
                break;
        }
    }
}

int main(void) {
    setup();
    draw();
    endwin();
    return 0;
}
