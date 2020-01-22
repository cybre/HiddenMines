#include <stdbool.h>

static const char columns[] = "ABCDEFGHIJ";

static void game(struct nk_context *ctx, int my_matrix[10][10],
        int their_matrix[10][10], int distance_matrix[10][10],
        bool can_guess, char winner_name[16])
{
    int i, j;
    struct nk_color neutral_color = nk_rgb(255, 255, 255);
    struct nk_color missed_color = nk_rgb(255, 40, 0);
    struct nk_color hit_color = nk_rgb(0, 255, 0);
    struct nk_color ship_color = nk_rgb(139, 69, 19);

    struct nk_style_window window;
    window = ctx->style.window;
    ctx->style.window.padding = nk_vec2(1, 1);
    ctx->style.window.spacing = nk_vec2(1, 1);

    if (strlen(winner_name) != 0) {
        if (nk_begin(ctx, "WINNER", nk_rect(0, 0, 1200, 560), NK_WINDOW_NO_SCROLLBAR)) {
            nk_layout_row_static(ctx, 560, 1200, 1);
            nk_labelf_colored(ctx, NK_TEXT_CENTERED, nk_rgb(255, 255, 255), "%s je pobedio!", winner_name);
        }
        nk_end(ctx);
        return;
    }

    if (nk_begin(ctx, "My table", nk_rect(0, 0, 600, 560), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, 50, 50, 11);

        nk_label_colored(ctx, "/", NK_TEXT_CENTERED, neutral_color);
        for (i = 0; i < 10; i++) {
            nk_labelf_colored(ctx, NK_TEXT_CENTERED, nk_rgb(255, 255, 255), "%c", columns[i]);
        }

        struct nk_style_button button;
        button = ctx->style.button;
        ctx->style.button.border = 0;
        ctx->style.button.rounding = 0;
        for (i = 0; i < 10; i++) {
            nk_labelf_colored(ctx, NK_TEXT_CENTERED, nk_rgb(255, 255, 255), "%d", i + 1);

            for (j = 0; j < 10; j++) {
                struct nk_color field_color;
                switch((FieldStatus)my_matrix[i][j]){
                    case FIELD_STATUS_HIT:
                        field_color = missed_color;
                        break;
                    case FIELD_STATUS_SHIP:
                        field_color = ship_color;
                        break;
                    default:
                        field_color = neutral_color;
                }
                nk_button_color(ctx, field_color);
            }
        }
        ctx->style.button = button;
    }
    nk_end(ctx);

    if (nk_begin(ctx, "Their table", nk_rect(600, 0, 600, 560), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, 50, 50, 11);

        nk_label_colored(ctx, "/", NK_TEXT_CENTERED, neutral_color);
        for (i = 1; i <= 10; ++i) {
            nk_labelf_colored(ctx, NK_TEXT_CENTERED, nk_rgb(255, 255, 255), "%c", columns[i - 1]);
        }

        struct nk_style_button button;
        button = ctx->style.button;
        ctx->style.button.border = 0;
        ctx->style.button.rounding = 0;
        ctx->style.button.normal = nk_style_item_color(missed_color);
        ctx->style.button.active = ctx->style.button.normal;
        ctx->style.button.hover = ctx->style.button.normal;

        for (i = 0; i < 10; i++) {
            nk_labelf_colored(ctx, NK_TEXT_CENTERED, nk_rgb(255, 255, 255), "%d", i + 1);

            for (j = 0; j < 10; j++) {
                switch((FieldStatus)their_matrix[i][j]){
                    case FIELD_STATUS_HIT: {
                        nk_button_color(ctx, hit_color);
                        break;
                    }
                    case FIELD_STATUS_MISS: {
                        char distance[2];
                        sprintf(distance, "%d", distance_matrix[i][j]);
                        nk_button_label(ctx, distance);
                        break;
                    }
                    default: {
                        if (nk_button_color(ctx, neutral_color) && can_guess) {
                            send_guess(i, j);
                            continue;
                        }
                    }
                }
            }
        }
        ctx->style.button = button;
    }
    nk_end(ctx);

    ctx->style.window = window;
}