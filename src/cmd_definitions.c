#include "cmd_definitions.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <vitasdk.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_OF(arr) (sizeof(arr) / sizeof(arr[0]))

const cmd_definition cmd_definitions[] = {
    {.name = "help",    .description = "Display this help screen",      .arg_count = 0, .executor = &cmd_help,    .alloced = 1},
    {.name = "destroy", .description = "Kill all running applications", .arg_count = 0, .executor = &cmd_destroy, .alloced = 0},
    {.name = "launch",  .description = "Launch an app by Title ID",     .arg_count = 1, .executor = &cmd_launch,  .alloced = 0},
    {.name = "reboot",  .description = "Reboot the console",            .arg_count = 0, .executor = &cmd_reboot,  .alloced = 0},
    {.name = "screen",  .description = "Turn the screen on or off",     .arg_count = 1, .executor = &cmd_screen,  .alloced = 0}
};

const cmd_definition *cmd_get_definition(char *cmd_name) {
  for (unsigned int i = 0; i < COUNT_OF(cmd_definitions); i++) {
    if (!strcmp(cmd_name, cmd_definitions[i].name)) {
      return &(cmd_definitions[i]);
    }
  }

  return NULL;
}

char* cmd_help(char **arg_list, size_t arg_count) {
  char* out;
  char* buf;
  int longest_cmd = 0;
  int longest_desc = 0;

  for (int i = 0; i < COUNT_OF(cmd_definitions); ++i)
  {
    int cmd_length = strlen(cmd_definitions[i].name);
    int desc_length = strlen(cmd_definitions[i].description);

    if (cmd_length > longest_cmd) longest_cmd = cmd_length;
    if (desc_length > longest_desc) longest_desc = desc_length;
  }

  // since we know how many characters a line will hold lets save it
  int line_length = (longest_cmd + longest_desc + 4);

  // we will store COUNT_OF(cmd_definitions) + 1 lines since the help message exists
  out = malloc(line_length * (COUNT_OF(cmd_definitions) + 1));
  buf = malloc(line_length);

  snprintf(buf, line_length, "%-*s\t\t%s\n", longest_cmd, "Command", "Description");
  strncpy(out, buf, line_length);
  for (int i = 0; i < COUNT_OF(cmd_definitions); ++i)
  {
    snprintf(buf, line_length, "%-*s\t\t%s\n", longest_cmd, cmd_definitions[i].name, cmd_definitions[i].description);
    strncat(out, buf, line_length);
  }

  free(buf);

  return out;
}

char* cmd_destroy(char **arg_list, size_t arg_count) {
  sceAppMgrDestroyOtherApp();
  return "Apps destroyed.\n";
}

char* cmd_launch(char **arg_list, size_t arg_count) {
  char uri[32];

  snprintf(uri, 32, "psgm:play?titleid=%s", arg_list[1]);

  if (sceAppMgrLaunchAppByUri(0x20000, uri) < 0)
    return "Error: cannot launch the app. Is the TITLEID correct?\n";

  return "Launched.\n";
}

char* cmd_reboot(char **arg_list, size_t arg_count) {
  scePowerRequestColdReset();
  return "Rebooting...\n";
}

char* cmd_screen(char **arg_list, size_t arg_count) {
  char *state = arg_list[1];

  if (!strcmp(state, "on")) {
    scePowerRequestDisplayOn();
    return "Turning display on...\n";
  } else if (!strcmp(state, "off")) {
    scePowerRequestDisplayOff();
    return "Turning display off...\n";
  }

  return "Error: param should be 'on' or 'off'\n";
}
