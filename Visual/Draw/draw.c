#include "draw.h"
#include "../Resources/xresources.h"
#include "../../Game_Logic/Projectiles/projectile.h"
#include "../../Game_Logic/Aliens/enemy.h"
#include <time.h>
#include <stdio.h>
#include <string.h> 

extern struct XResources resources;
extern unsigned long orange_pixel;
//Dibujo de la nave
void drawShip() {
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    XClearWindow(resources.display, resources.window);

    ShipPosition shipPos = getShipPosition();
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 30, shipPos.y - 7, 60, 14);
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 22, shipPos.y + 7, 15, 7);
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x + 10, shipPos.y + 7, 15, 7);
    XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 7, shipPos.y - 7, 14, 7);
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 3, shipPos.y - 22, 6, 14);
}
//Dibujo de los disparos efectuados
void drawProjectiles() {
    XSetForeground(resources.display, resources.gc, orange_pixel);
    Projectile *projectiles = getProjectiles();
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        if (projectiles[i].active) {
            XFillRectangle(resources.display, resources.window, resources.gc,
                           projectiles[i].x - 3, projectiles[i].y - 10, 6, 10);
        }
    }
}
//Dibujo de los aliens(mejorar estética)
void drawEnemies() {
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    Enemy *enemies = getEnemies();
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        if (enemies[i].active) {
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 10, enemies[i].y - 10, 20, 20);
            if (enemies[i].type==ENEMY_TYPE_1)
            {
               XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 5, enemies[i].y - 5, 4, 4);
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x + 1, enemies[i].y - 5, 4, 4);
            XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
            }
            else if (enemies[i].type==ENEMY_TYPE_2)
            {
                 XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 5, enemies[i].y - 5, 4, 4);
            XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
            }
            
            
        }
    }
}

//Pantalla de selección de modo de juego
void drawSelectModeScreen() {
    XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
    XFillRectangle(resources.display, resources.buffer, resources.gc, 0, 0, resources.WIDTH, resources.HEIGHT);

    const char* message = "Select Game Mode:";
    const char* mode1 = "1. Progressive";
    const char* mode2 = "2. Alternate";
    const char* mode3 = "3. Random";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.buffer, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 60, message, strlen(message));

    int mode1_width = XTextWidth(font, mode1, strlen(mode1));
    XDrawString(resources.display, resources.buffer, resources.gc, (resources.WIDTH - mode1_width) / 2, resources.HEIGHT / 2 - 20, mode1, strlen(mode1));

    int mode2_width = XTextWidth(font, mode2, strlen(mode2));
    XDrawString(resources.display, resources.buffer, resources.gc, (resources.WIDTH- mode2_width) / 2, resources.HEIGHT / 2 + 20, mode2, strlen(mode2));

    int mode3_width = XTextWidth(font, mode3, strlen(mode3));
    XDrawString(resources.display, resources.buffer, resources.gc, (resources.WIDTH - mode3_width) / 2, resources.HEIGHT / 2 + 60, mode3, strlen(mode3));

    XFreeFont(resources.display, font);

    // Copia el contenido del buffer a la ventana
    XCopyArea(resources.display, resources.buffer, resources.window, resources.gc, 0, 0, resources.WIDTH, resources.HEIGHT, 0, 0);
    XFlush(resources.display);
}

//Pantalla Final(Game Over)
void drawGameOver() {
    XClearWindow(resources.display, resources.window);
    const char* message = "Game Over";
    const char* restart_message = "Restart";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        fprintf(stderr, "No se pudo cargar la fuente grande, usando la fuente predeterminada.\n");
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    int button_x = (resources.WIDTH - restart_text_width) / 2 - 10;
    int button_y = resources.HEIGHT / 2 + 30;
    int button_width = restart_text_width + 20;
    int button_height = 30;

    XDrawRectangle(resources.display, resources.window, resources.gc, button_x, button_y, button_width, button_height);
    XDrawString(resources.display, resources.window, resources.gc, button_x + 10, button_y + 20, restart_message, strlen(restart_message));

    XFreeFont(resources.display, font);
    XFlush(resources.display);
}

//Pantalla del nivel 1(Victoria)
void drawWinnLevelOne(){
    XClearWindow(resources.display, resources.window);
    const char* message = "You have passed Level 1!";
    const char* restart_message = "Press Right-Click to start Level 2";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar una fuente más grande
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de victoria en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    // Dibujar el mensaje de pasar de nivel
    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - restart_text_width) / 2, resources.HEIGHT / 2 + 20, restart_message, strlen(restart_message));

    XFreeFont(resources.display, font);
    XFlush(resources.display);
}

//Pantalla del nivel 2(Victoria)
void drawWinnLevelTwo(){
    XClearWindow(resources.display, resources.window);
    const char* message = "You have passed Level 2!";
    const char* restart_message = "Press Right-Click to start Level 3";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar una fuente más grande
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de victoria en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    // Dibujar el mensaje de pasar de nivel
    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - restart_text_width) / 2, resources.HEIGHT / 2 + 20, restart_message, strlen(restart_message));

    XFreeFont(resources.display, font);
    XFlush(resources.display);
}

//Pantalla del nivel 3(Victoria)
void drawWinnLevelThree(){
    XClearWindow(resources.display, resources.window);
    const char* message = "You have passed Level 3!";
    const char* restart_message = "Press Right-Click to complete Game";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar una fuente más grande
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de victoria en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    // Dibujar el mensaje de finalizar juego
    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - restart_text_width) / 2, resources.HEIGHT / 2 + 20, restart_message, strlen(restart_message));

    XFreeFont(resources.display, font);
    XFlush(resources.display);
}

void drawWinn(){
    XClearWindow(resources.display, resources.window);
    const char* message = "You Won";
    const char* restart_message = "Press Right-Click to restart";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar una fuente más grande
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return;
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de victoria en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    // Dibujar el mensaje de reinicio
    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - restart_text_width) / 2, resources.HEIGHT / 2 + 20, restart_message, strlen(restart_message));

    XFreeFont(resources.display, font);
    XFlush(resources.display);
}


void *drawLoop(void *arg) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 12000000L;

    while (1) {

        if (resources.game_state == GAME_SELECT_MODE) {
            drawSelectModeScreen();
            nanosleep(&ts, NULL);
        }else if (resources.game_state == GAME_OVER) {
            drawGameOver();  
            while (resources.game_state == GAME_OVER) {
                nanosleep(&ts, NULL);
            }
        } else if (resources.game_state == GAME_WIN_LEVEL) {
            if (resources.current_level==LEVEL_1)
            {
               drawWinnLevelOne();
            }
            else if (resources.current_level==LEVEL_2)
            {
                drawWinnLevelTwo();
            }
            else
            {
                drawWinnLevelThree();
            }
            
            while (resources.game_state == GAME_WIN_LEVEL) {
                nanosleep(&ts, NULL);
            }
        }
        else if (resources.game_state == GAME_WIN)
        {
            drawWinn();
            while (resources.game_state == GAME_WIN) {
                nanosleep(&ts, NULL);
            }
        }
         else if (resources.game_state == GAME_RUNNING) {
            pthread_mutex_lock(&resources.mutex);
            resources.should_draw = 1;
            pthread_mutex_unlock(&resources.mutex);

            drawShip();
            updateProjectiles();
            updateEnemies(); 
            drawProjectiles();
            drawEnemies();
            XFlush(resources.display);

            nanosleep(&ts, NULL); 
            
        }
    }
    return NULL;
}