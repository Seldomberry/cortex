#include "cortotool_environment.h"

char* cortotool_environmentPath(char* name) {
    char* cortoHome = corto_getenv("CORTO_HOME");
    if (!cortoHome) {
        corto_seterr("could not find CORTO_HOME");
        goto error;
    }
    char* path = NULL;
    if (name) {
        if (corto_asprintf(&path, "%s/env/%s", cortoHome, name) < 0) {
            goto error;
        }
    } else {
        if (corto_asprintf(&path, "%s/env", cortoHome) < 0) {
            goto error;
        }
    }
    return path;
error:
    return NULL;
}
                                                                                                                                                
corto_int16 cortotool_activate(int argc, char *argv[]) {
    if (argc < 2) {
        corto_error("did not provide an environment name to activate");
        goto error_no_env;
    }
    char* name = argv[1];
    char* path = cortotool_environmentPath(name);
    if (!path) {
        goto error_no_env;
    }
    if (!corto_fileTest(path)) {
        corto_error("environment %s does not exist", name);
        goto error_not_found;
    }
    corto_setenv("CORTO_ENVIRONMENT", name, FALSE);

    FILE* script = fopen("./.cortoactivateenvironment", "w");
    if (!script) {
        corto_error("could not open activate script");
        goto error_not_found;
    }
    char* oldPrompt = corto_getenv("PS1");
    if (!oldPrompt) {
        corto_error("could not find system prompt");
        goto error_not_found;
    }
    /* corto_setenv will parse variables; and that can strip parts of the prompt */
    setenv("CORTO_ENVIRONMENT_OLD_PROMPT", oldPrompt, FALSE);
    fprintf(
        script,
        "CORTO_HOME=\"$HOME/.corto/env/$CORTO_ENVIRONMENT\"\n"
        "CORTO_TARGET=\"$HOME/.corto/env/$CORTO_ENVIRONMENT\"\n"
        "CORTO_BUILD=\"/usr/local/lib/corto/$CORTO_VERSION/build\"\n"
        "export PS1=\"($CORTO_ENVIRONMENT) $CORTO_ENVIRONMENT_OLD_PROMPT\"\n"
        "exec \"${@:-$SHELL}\"\n"
        "# this file should be removed\n"
    );
    fclose(script);

    corto_pid pid = corto_procrun("sh", (char*[]){"sh", "./.cortoactivateenvironment", NULL});
    corto_int8 procResult = 0;
    if (corto_procwait(pid, &procResult) || procResult) {
        corto_error("script in %s environment failed", name);
        goto error_not_found;
    }

    if (corto_rm("./.cortoactivateenvironment")) {
        corto_error("cannot remove activate script");
        goto error_not_found;
    }

    corto_dealloc(path);
    return 0;
error_not_found:
    corto_dealloc(path);
error_no_env:
    return -1;
}

corto_int16 cortotool_listEnvironments(int argc, char* argv[]) {
    CORTO_UNUSED(argc);
    CORTO_UNUSED(argv);
    char* home = corto_getenv("HOME");
    if (!home) {
        corto_error("could not find user home directory");
        goto error_home;
    }
    char* envPath = cortotool_environmentPath(NULL);
    if (!envPath) {
        goto error_envPath;
    }
    corto_ll files = corto_opendir(envPath);
    if (!files) {
        corto_error("could not list env directories");
        goto error_envPath;
    }
    corto_iter iter = corto_llIter(files);
    while (corto_iterHasNext(&iter)) {
        char* name = corto_iterNext(&iter);
        char* path = NULL;
        corto_asprintf(&path, "%s/%s", envPath, name);
        if (corto_isdir(path)) {
            corto_print("%s", name);
        }
        corto_dealloc(path);
    }
    corto_closedir(files);
    corto_dealloc(envPath);
    return 0;
error_envPath:
    if (envPath) corto_dealloc(envPath);
error_home:
    return -1;
}
