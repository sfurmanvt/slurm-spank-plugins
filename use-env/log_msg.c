/*****************************************************************************
 *
 *  Copyright (C) 2007-2008 Lawrence Livermore National Security, LLC.
 *  Produced at Lawrence Livermore National Laboratory.
 *  Written by Mark Grondona <mgrondona@llnl.gov>.
 *
 *  UCRL-CODE-235358
 * 
 *  This file is part of chaos-spankings, a set of spank plugins for SLURM.
 * 
 *  This is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "use-env.h"

struct log_ctx {
	int quiet;
	int verbose;
	char *prefix;
};

static struct log_ctx log_ctx = { 0, 0, NULL };

int log_msg_init (const char *prefix)
{
	if (prefix)
		log_ctx.prefix = strdup (prefix);
	return (0);
}

void log_msg_fini ()
{
	if (log_ctx.prefix)
		free (log_ctx.prefix);
}

int log_msg_verbose ()
{
	return (log_ctx.verbose++);
}

int log_msg_set_verbose (int level)
{
    return (log_ctx.verbose = level);
}

int log_msg_quiet ()
{
	return (log_ctx.quiet++);
}


static void 
vlog_msg (const char *prefix, int use_basename, const char *format, va_list ap)
{
    char  buf[4096];
    char *p;
    int   n;
    int   len;

    p = buf;
    len = sizeof (buf);

    /*  Prefix output with facility name.
     */
    if (log_ctx.prefix && (*log_ctx.prefix != '\0')) {
        n = snprintf (buf, len, "%s: ", log_ctx.prefix);
        if ((n < 0) || (n >= len)) {
            p += len - 1;
            len = 0;
        }
        else {
            p += n;
            len -= n;
        }
    }

    /*  Add a log level prefix.
     */
    if ((len > 0) && (prefix)) {
        n = snprintf (p, len, "%s: ", prefix);
        if ((n < 0) || (n >= len)) {
            p += len - 1;
            len = 0;
        }
        else {
            p += n;
            len -= n;
        }
    }

    /* Add file and line number information
     */
    if (len > 0 && (lex_file () != NULL)) {
        char *file = strdup (lex_file ());
        char *name = use_basename ? basename (file) : file;

        n = snprintf (p, len, "%s: %d: ", name, lex_line());

        if ((n < 0) || (n >= len)) {
            p += len - 1;
            len = 0;
        }
        else {
            p += n;
            len -= n;
        }
        free (file);
    }

    if ((len > 0) && (format)) {
        n = vsnprintf (p, len, format, ap);
        if ((n < 0) || (n >= len)) {
            p += len - 1;
            len = 0;
        }
        else {
            p += n;
            len -= n;
        }
    }

    /*  Add suffix for truncation if necessary.
     */
    if (len <= 0) {
        char *q;
        const char *suffix = "+";
        q = buf + sizeof (buf) - 1 - strlen (suffix);
        p = (p < q) ? p : q;
        strcpy (p, suffix);
        p += strlen (suffix);
    }

    *p = '\0';

    fprintf (stderr, "%s", buf);

    return;
}


int log_err (const char *format, ...)
{
    va_list ap;

    if (log_ctx.quiet)
        return (-1);

    va_start (ap, format);
    vlog_msg ("Error", 0, format, ap);
    va_end (ap);
    return (-1);
}

void log_msg (const char *format, ...)
{
    va_list ap;

    if (log_ctx.quiet)
        return;

    va_start (ap, format);
    vlog_msg (NULL, 1, format, ap);
    va_end (ap);
    return;
}

void log_verbose (const char *format, ...)
{
    va_list ap;

    if (log_ctx.quiet || !log_ctx.verbose)
        return;

    va_start (ap, format);
    vlog_msg (NULL, 1, format, ap);
    va_end (ap);
    return;
}

void log_debug (const char *format, ...)
{
    va_list ap;

    if ((log_ctx.quiet) || (log_ctx.verbose < 2))
        return;

    va_start (ap, format);
    vlog_msg (NULL, 1, format, ap);
    va_end (ap);
    return;
}

void log_debug2 (const char *format, ...)
{
    va_list ap;

    if ((log_ctx.quiet) || (log_ctx.verbose < 3))
        return;

    va_start (ap, format);
    vlog_msg (NULL, 1, format, ap);
    va_end (ap);
    return;
}

void log_debug3 (const char *format, ...)
{
    va_list ap;

    if ((log_ctx.quiet) || (log_ctx.verbose < 4))
        return;

    va_start (ap, format);
    vlog_msg (NULL, 1, format, ap);
    va_end (ap);
    return;
}



/*
 * vi: ts=4 sw=4 expandtab
 */
