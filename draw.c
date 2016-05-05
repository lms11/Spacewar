#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "draw.h"
#define DISPLAY_H 800
#define DISPLAY_W 800
#define UNIVERSE_H 8e7
#define UNIVERSE_W 8e7
#define SCALE_X (DISPLAY_W / UNIVERSE_W)
#define SCALE_Y (DISPLAY_H / UNIVERSE_H)

ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *planet_im;
ALLEGRO_BITMAP *player1_im;
ALLEGRO_BITMAP *player2_im;
ALLEGRO_BITMAP *projectile_im;
ALLEGRO_BITMAP *background;
ALLEGRO_EVENT_QUEUE *event_queue;


int drawInit () {
    if (!al_init ()) {
        fprintf (stderr, "Falha ao inicializar a Allegro.\n");
        return -1;
    }

    if (!al_init_image_addon ()) {
        fprintf (stderr, "Falha ao inicializar add-on allegro_image.\n");
        return -1;
    }

    display = al_create_display (DISPLAY_W, DISPLAY_H);
    if (!display) {
        fprintf (stderr, "Falha ao criar janela.\n");
        return -1;
    }

    event_queue = al_create_event_queue ();
    if (!event_queue) {
        fprintf (stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display (display);
        return -1;
    }

    planet_im = al_load_bitmap ("../Images/planets16/planet_28.png");
    if (!planet_im) {
        fprintf (stderr, "Falha ao iniciar a imagem do Planeta\n");
        al_destroy_display (display);
        al_destroy_event_queue (event_queue);
        return -1;
    }

    player1_im = al_load_bitmap ("../Images/player1.png");
    if (!player1_im) {
        fprintf (stderr, "Falha ao iniciar a imagem do Player 1\n");
        al_destroy_display (display);
        al_destroy_event_queue (event_queue);
        al_destroy_bitmap (planet_im);

        return -1;
    }

    player2_im = al_load_bitmap ("../Images/player2.png");
    if (!player2_im) {
        fprintf (stderr, "Falha ao iniciar a imagem do Player 2\n");
        al_destroy_display (display);
        al_destroy_event_queue (event_queue);
        al_destroy_bitmap (planet_im);
        al_destroy_bitmap (player1_im);

        return -1;
    }

    projectile_im = al_load_bitmap ("../Images/projectile.png");
    if (!projectile_im) {
        fprintf (stderr, "Falha ao iniciar a imagem dos Projeteis\n");
        al_destroy_display (display);
        al_destroy_event_queue (event_queue);
        al_destroy_bitmap (planet_im);
        al_destroy_bitmap (player1_im);
        al_destroy_bitmap (player2_im);

        return -1;
    }

    background = al_load_bitmap ("../Images/space.jpg");
    if (!projectile_im) {
        fprintf (stderr, "Falha ao iniciar a imagem de fundo\n");
        al_destroy_display (display);
        al_destroy_event_queue (event_queue);
        al_destroy_bitmap (planet_im);
        al_destroy_bitmap (player1_im);
        al_destroy_bitmap (player2_im);
        al_destroy_bitmap (projectile_im);

        return -1;
    }

    return 0;
}

static void destroyScene () {
    al_destroy_display (display);
    al_destroy_event_queue (event_queue);
    al_destroy_bitmap (planet_im);
    al_destroy_bitmap (player1_im);
    al_destroy_bitmap (player2_im);
    al_destroy_bitmap (projectile_im);
    al_destroy_bitmap (background);
}

static void draw (ALLEGRO_BITMAP *image, Body *body) {
    /* Posicoes em escala */
    double sx = body->position->x * SCALE_X + DISPLAY_W / 2.0;
    double sy = body->position->y * SCALE_Y + DISPLAY_H / 2.0;
    /* Pixels da imagem */
    double pw = al_get_bitmap_width (image) / 2.0;
    double ph = al_get_bitmap_height (image) / 2.0;
    /* Constante de Escala */
    double cteX = body->radius * SCALE_X / pw;
    double cteY = body->radius * SCALE_Y / ph;

    al_draw_scaled_rotated_bitmap (image, pw, ph, sx, sy, cteX, cteY, body->angle, 0);
}

static void drawBodies (Ship *player1, Ship *player2, Celula *head, Body *planet) {
    draw (player1_im, player1->body);
    draw (player2_im, player2->body);
    draw (planet_im, planet);
    Celula *aux = head->next;
    while (aux != NULL) {
        draw (projectile_im, aux->proj->body);
        aux = aux->next;
    }
}

void drawScene (double dt, double simulation, Ship *player1, Ship *player2, Celula *head, Body *planet) {
    int redraw = 1;
    /* Creating Timer */
    ALLEGRO_TIMER *timer = NULL;
    timer = al_create_timer (1.0 / dt);
    if (!timer)
        fprintf (stderr, "Erro ao inicalizar timer\n");

    al_register_event_source (event_queue, al_get_display_event_source (display));
    al_register_event_source (event_queue, al_get_timer_event_source (timer));
    al_clear_to_color (al_map_rgb (0, 0, 0));
    al_flip_display ();
    al_start_timer (timer);

    while (simulation > 0) {
        ALLEGRO_EVENT event;
        al_wait_for_event (event_queue, &event);
        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = 1;
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        if (redraw && al_is_event_queue_empty (event_queue)) {
            redraw = 0;

            al_clear_to_color (al_map_rgb (0, 0, 0));
            updatePositions (dt, player1, player2, head, planet);
            al_draw_bitmap (background, 0, 0 , 0);

            drawBodies (player1, player2, head, planet);
            simulation -= dt;
            al_flip_display ();
        }

    }

     al_destroy_timer (timer);
    destroyScene ();
}