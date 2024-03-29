/* Copyright (c) 2021-2023, Michael Santos <michael.santos@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>

#if defined(NOPRIVEXEC_prctl)
#include <sys/prctl.h>
#elif defined(NOPRIVEXEC_procctl)
#include <sys/procctl.h>
#elif defined(NOPRIVEXEC_pledge)
#include <string.h>
#define PLEDGENAMES
#include <sys/pledge.h>
#else
#pragma message "unsupported platform"
#endif

#define NOPRIVEXEC_VERSION "1.0.1"

static int disable_setuid(void);
static void usage(void);

static const struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                             {NULL, 0, NULL, 0}};

int main(int argc, char *argv[]) {
  int ch;

  while ((ch = getopt_long(argc, argv, "+h", long_options, NULL)) != -1) {
    switch (ch) {
    case 'h':
    default:
      usage();
    }
  }

  argc -= optind;
  argv += optind;

  if (argc < 1) {
    usage();
  }

  if (disable_setuid() < 0)
    err(111, "disable_setuid");

  (void)execvp(argv[0], argv);

  err(errno == ENOENT ? 127 : 126, "%s", argv[0]);
}

#if defined(NOPRIVEXEC_prctl)
static int disable_setuid(void) {
  return prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}
#elif defined(NOPRIVEXEC_procctl)
static int disable_setuid(void) {
  int data = PROC_NO_NEW_PRIVS_ENABLE;
  return procctl(P_PID, 0, PROC_NO_NEW_PRIVS_CTL, &data);
}
#elif defined(NOPRIVEXEC_pledge)
static int disable_setuid(void) {
  char execpromises[1024] = {0};
  int i;

  for (i = 0; pledgenames[i].name != NULL; i++) {
    if ((strlcat(execpromises, pledgenames[i].name, sizeof(execpromises)) >=
         sizeof(execpromises)) ||
        (strlcat(execpromises, " ", sizeof(execpromises)) >=
         sizeof(execpromises))) {
      errno = EINVAL;
      return -1;
    }
  }
  return pledge(NULL, execpromises);
}
#else
static int disable_setuid(void) {
  errno = EPERM;
  return -1;
}
#endif

static noreturn void usage(void) {
  errx(EXIT_FAILURE,
       "[OPTION] <COMMAND> <...>\n"
       "version: %s (%s)\n"
       "-h, --help                usage summary",
       NOPRIVEXEC_VERSION, NOPRIVEXEC);
}
