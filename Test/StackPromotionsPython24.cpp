#include <Python.h>
#include <cstdio>
#include <string>
#include <algorithm>
int main(int argc, char** argv)
{
    Py_NoSiteFlag = 1;
    Py_Initialize();
    for (int i = 1; i < argc; ++i)
    {
        FILE* f = fopen(argv[i], "rb");
        if (!f) return 2;
        std::string text;
        char buffer[4096]; size_t n;
        while ((n = fread(buffer, 1, sizeof(buffer), f)) != 0) text.append(buffer, n);
        fclose(f);
        text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
        PyObject* code = Py_CompileString(text.c_str(), argv[i], Py_file_input);
        if (!code) { PyErr_Print(); return 1; }
        Py_DECREF(code);
        printf("PASS Python 2.4 syntax: %s\n", argv[i]);
    }
    Py_Finalize();
    return 0;
}
