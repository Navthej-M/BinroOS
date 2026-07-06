#include <drivers/video/cli/colors.h>

static term_color_t CLI_THEME_COLOR_BG = COLOR_CATPPUCCIN_BG;
static term_color_t CLI_THEME_COLOR_FG = COLOR_WHITE;

uint32_t color_get_cli_theme_bg(void) { return CLI_THEME_COLOR_BG; }
uint32_t color_get_cli_theme_fg(void) { return CLI_THEME_COLOR_FG; }
void color_set_cli_theme_bg(term_color_t color) { CLI_THEME_COLOR_BG = color; }
void color_set_cli_theme_fg(term_color_t color) { CLI_THEME_COLOR_FG = color; }
