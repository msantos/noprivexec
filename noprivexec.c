/* Copyright (c) 2021, Michael Santos <michael.santos@gmail.com>
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
#include <unistd.h>

#if defined(NOPRIVEXEC_seccomp)
#include <sys/prctl.h>
#elif defined(NOPRIVEXEC_pledge)
#include <string.h>
#define PLEDGENAMES
#include <sys/pledge.h>
#else
#pragma message "unsupported platorm"
#endif

#define NOPRIVEXEC_VERSION "0.1.0"

int disable_setuid(void);

static const struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                             {NULL, 0, NULL, 0}};

static void usage();

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

  err(127, "%s", argv[0]);
}

#if defined(NOPRIVEXEC_seccomp)
int disable_setuid(void) { return prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0); }
#elif defined(NOPRIVEXEC_pledge)
int disable_setuid(void) {
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
int disable_setuid(void) {
  errno = EPERM;
  return -1;
}
#endif

static void usage() {
  errx(EXIT_FAILURE,
       "[OPTION] <COMMAND> <...>\n"
       "version: %s (%s)\n"
       "-h, --help                usage summary",
       NOPRIVEXEC_VERSION, NOPRIVEXEC);
}
