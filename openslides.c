#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Python.h>

#define PYTHON_DLL_PATH L"\\Dlls\\python35.dll"

static void (*py_initialize)(void) = 0;
static void (*py_finalize)(void) = 0;
static void (*py_set_program_name)(wchar_t *) = 0;
static void (*py_set_python_home)(wchar_t *) = 0;
static int (*py_run_simple_string_flags)(const char *, PyCompilerFlags *) = 0;
static void (*py_sys_set_argv_ex)(int, wchar_t **, int) = 0;
static int (*py_main)(int, wchar_t **) = 0;
static int *py_ignore_environment_flag = 0;

static const char *run_openslides_code =
    "import openslides_gui.gui;"
    "openslides_gui.gui.main()";

/* determine the path to the executable
 * NOTE: Py_GetFullProgramPath() can't be used because
 *       this would trigger pythons search-path initialization
 *       But we need this to initialize PYTHONHOME before this happens
 */
static wchar_t *
_get_module_name()
{
    size_t size = 1;
    wchar_t *name = NULL;
    int i;

    /* a path > 40k would be insane, it is more likely something
     * else has gone very wrong on the system
     */
    for (i = 0;i < 10; i++)
    {
	DWORD res;
	wchar_t *n;

	n = realloc(name, size * sizeof(wchar_t));
	if (!n)
	{
	    free(name);
	    return NULL;
	}
	name = n;

	res = GetModuleFileNameW(NULL, name, size);
	if (res != 0 && res < size)
	{
	    return name;
	}
	else if (res == size)
	{
	    /* NOTE: Don't check GetLastError() == ERROR_INSUFFICIENT_BUFFER
	     *       here, it isn't set consistently across all platforms
	     */

	    size += 4096;
	}
	else
	{
	    DWORD err = GetLastError();
	    fprintf(stderr, "WARNING: GetModuleFileName() failed "
		"(res = %d, err = %d)", res, err);
	    free(name);
	    return NULL;

	}
    }

    return NULL;
}

static void
_fatal_error(const wchar_t *text)
{
    MessageBoxW(NULL, text, L"Fatal error", MB_OK | MB_ICONERROR);
    exit(1);
}

static void
_fatal_error_fmt(const wchar_t *fmt, ...)
{
    int size = 512;
    wchar_t *buf  = malloc(size * sizeof(wchar_t));
    va_list args;
    int chars_written;

    if (!buf)
	abort();

    va_start(args, fmt);
    for (;;)
    {
	chars_written = vswprintf(buf, size, fmt, args);
	if (chars_written > -1 && chars_written < size)
	    break;
	else if (chars_written > size)
	    size = chars_written + 1;
	else
	    size *= 2;

	buf = realloc(buf, size * sizeof(wchar_t));
	if (!buf)
	    abort();
    }
    va_end(args);

    _fatal_error(buf);
}

static void *
_load_func(HMODULE module, const char *name)
{
    void *address = GetProcAddress(module, name);
    if (!address)
	_fatal_error_fmt(L"Failed to look up symbol %s", name);
    return address;
}

static void
_load_python(const wchar_t *pyhome)
{
    size_t pyhome_len = wcslen(pyhome);
    size_t dll_len = wcslen(PYTHON_DLL_PATH);
    size_t size = pyhome_len + dll_len + 1;
    wchar_t *buf = malloc(size * sizeof(wchar_t));
    HMODULE py_dll;

    if (!buf)
	abort();
    memcpy(buf, pyhome, pyhome_len * sizeof(wchar_t));
    memcpy(buf + pyhome_len, PYTHON_DLL_PATH, dll_len * sizeof(wchar_t));
    buf[size - 1] = L'\0';

    py_dll = LoadLibraryW(buf);
    if (!py_dll)
    {
	DWORD error = GetLastError();
	_fatal_error_fmt(L"Failed to load %s (error %d)", buf, error);
    }

    py_initialize = (void (*)(void))_load_func(py_dll, "Py_Initialize");
    py_finalize = (void (*)(void))_load_func(py_dll, "Py_Finalize");
    py_set_program_name = (void (*)(wchar_t *))
	_load_func(py_dll, "Py_SetProgramName");
    py_set_python_home = (void (*)(wchar_t *))
	_load_func(py_dll, "Py_SetPythonHome");
    py_run_simple_string_flags = (int (*)(const char *, PyCompilerFlags *))
	_load_func(py_dll, "PyRun_SimpleStringFlags");
    py_sys_set_argv_ex = (void (*)(int, wchar_t **, int))
	_load_func(py_dll, "PySys_SetArgvEx");
    py_main = (int (*)(int, wchar_t **))_load_func(py_dll, "Py_Main");
    py_ignore_environment_flag = (int *)
	_load_func(py_dll, "Py_IgnoreEnvironmentFlag");
}

static int
_run()
{
    if (py_run_simple_string_flags(run_openslides_code, NULL) != 0)
	_fatal_error(L"Failed to execute openslides");

    return 0;
}


#ifdef OPENSLIDES_CONSOLE
int
wmain(void)
#else
int WINAPI
wWinMain(HINSTANCE inst, HINSTANCE prev_inst, LPWSTR cmdline, int show)
#endif
{
    int returncode;
    int run_py_main = __argc > 1;
    wchar_t *py_home, *sep = NULL;

    py_home = _get_module_name();
    if (!py_home)
	_fatal_error(L"Could not determine portable root directory");

    sep = wcsrchr(py_home, L'\\');
    /* should always be the true */
    if (sep)
	*sep = L'\0';

    if (!__wargv || !*__wargv)
	_fatal_error(L"Empty argument vector");

    _load_python(py_home);
    py_set_program_name(__wargv[0]);
    py_set_python_home(py_home);
    *py_ignore_environment_flag = 1;

    if (run_py_main)
    {
	/* we where given extra arguments, behave like python.exe */
	returncode =  py_main(__argc, __wargv);
    }
    else
    {
	/* no arguments given => start openslides gui */
	py_initialize();
	py_sys_set_argv_ex(__argc, __wargv, 0);

	returncode = _run();
	py_finalize();
    }

    free(py_home);

    return returncode;
}
