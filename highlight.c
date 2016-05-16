/* highlight.c -- MCabber Plugin to highlight specific messages. */

/*
 * This plugin has one command: '/highlight'.
 *
 * Give it an regexp as an argument, and it will set
 * the attention flag on any message that matches it.
 *
 * Simple.
 */

#include <mcabber/modules.h>
#include <mcabber/commands.h>
#include <mcabber/logprint.h>
#include <mcabber/screen.h>
#include <mcabber/compl.h>
#include <mcabber/hooks.h>

#include <string.h>

#define HIGHLIGHT_VERSION     "0.0.1"
#define HIGHLIGHT_DESCRIPTION "Highlight received messages if they match JID and/or message content."

static struct highlight_module_globals {
    GSList *    match_list;
    guint       hook;
} globals;

static guint highlight_process_message(const gchar *hook, hk_arg_t *args, gpointer data) {
    const gchar *jid, *resource, *message;
    gboolean groupchat, delayed, errored;
    GSList *item = NULL;

    /* If we don't have any matches, abort early. */
    if (globals.match_list == NULL)
        goto finished;

    /* See +247 dd8ae0abfc68:tip:mcabber/hooks.c */
    jid       = args[0].value;
    resource  = args[1].value;
    message   = args[2].value;

    delayed   = (strlen(args[4].value) > 0);
    errored   = (strcmp(args[5].value, "true") == 0);
    groupchat = (strcmp(args[3].value, "true") == 0);

    /* We don't want to process either delayed or error messages. */
    if (delayed || errored)
        goto finished;

    /* Iterate through our list of regexps and see if they match this message. */
    for (item = globals.match_list; item; item = g_slist_next(item)) {
        GRegex *regex = (GRegex *) item->data;

        if (g_regex_match(regex, message, 0, NULL)) {
            /* Set the attention flag if necessary. */
            scr_setattentionflag_if_needed(jid, FALSE, ROSTER_UI_PRIO_ATTENTION_MESSAGE, prio_max);

            return HOOK_HANDLER_RESULT_HIGHLIGHT_MESSAGE;
        }
    }

finished:
    /* Always continue processing messages. */
    return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

static void highlight_command(char *argument) {
    GRegexCompileFlags flags = 0;
    GRegex *regex = NULL;
    GError *error = NULL;

    flags |= G_REGEX_RAW;
    flags |= G_REGEX_OPTIMIZE;
    flags |= G_REGEX_CASELESS;
    flags |= G_REGEX_NO_AUTO_CAPTURE;

    /* Compile the trimmed argument into a regular expression to match against. */
    regex = g_regex_new(argument, flags, G_REGEX_MATCH_NOTEMPTY, &error);

    if (error) {
        /* Error in regexp, log and abort. */
        scr_log_print(LPRINT_LOGNORM, "highlight: Error compiling regex: %s.", error->message);
        g_error_free(error);
    } else {
        /* Add it to the list of matchers. */
        scr_log_print(LPRINT_LOGNORM, "highlight: Added highlight for %s.", argument);
        globals.match_list = g_slist_append(globals.match_list, (gpointer) regex);
    }
}

static void highlight_init() {
    /* Let everyone know we're here. */
    scr_log_print(LPRINT_NORMAL, "highlight: plugin enabled.");

    /* Register our message handler so we can start processing incoming messages. */
    globals.hook = hk_add_handler(highlight_process_message, HOOK_PRE_MESSAGE_IN, G_PRIORITY_DEFAULT, NULL);

    /* Add our commands for highlighting. */
    cmd_add("highlight", "", 0, 0, highlight_command, NULL);
}

static void highlight_uninit() {
    /* Let everyone know we're checking out. */
    scr_log_print(LPRINT_NORMAL, "highlight: plugin disabled.");

    /* De-register our message handler as we're done with it now. */
    hk_del_handler(HOOK_PRE_MESSAGE_IN, globals.hook);

    /* Remove our commands. */
    cmd_del("highlight");

    /* Nullify all the pointers. */
    globals.match_list = NULL;
}

/* Module description and information. */
module_info_t info_highlight = {
    .branch         = MCABBER_BRANCH,
    .api            = MCABBER_API_VERSION,
    .version        = HIGHLIGHT_VERSION,
    .description    = HIGHLIGHT_DESCRIPTION,
    .init           = highlight_init,
    .uninit         = highlight_uninit,
    .requires       = NULL,
    .next           = NULL,
};
