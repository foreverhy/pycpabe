#include <string>
#include <python2.7/Python.h>
#include <unistd.h>
#include <glib.h>
#include <pbc/pbc.h>
#include <pbc/pbc_random.h>
#include <bswabe.h>
#include <cstdio>

int foo(int a, int b) {
    return a + b;
}

//void setup(std::string &pub, std::string &msk) {
    //bswabe_pub_t *pub_key;
    //bswabe_msk_t *msk_key;

    //bswabe_setup(&pub_key, &msk_key);
    //GByteArray *pub_str = bswabe_pub_serialize(pub_key);
    //GByteArray *msk_str = bswabe_msk_serialize(msk_key);
    //pub = std::string(pub_str->data, pub_str->len);
    //msk = std::string(msk_str->data, msk_str->len);
//}

static PyObject* _foo(PyObject *self, PyObject *args) {
    (void)self;
    int _a, _b;
    if (!PyArg_ParseTuple(args, "ii", &_a, &_b)) {
        return nullptr;
    }
    int res = foo(_a, _b);
    return PyLong_FromLong(res);
}

static PyObject* setup(PyObject *self, PyObject *args) {
    (void)self;
    bswabe_pub_t *pub;
    bswabe_msk_t *msk;

    bswabe_setup(&pub, &msk);
    GByteArray *p = bswabe_pub_serialize(pub);
    GByteArray *m = bswabe_msk_serialize(msk);
    PyObject *ret = PyTuple_Pack(
            2,
            PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(p->data), p->len),
            PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(m->data), m->len)
            );

    g_byte_array_free(p, 1);
    g_byte_array_free(m, 1);
    free(pub);
    free(msk);
    return ret;
}

static PyMethodDef AbeModuleMethods[] = {
    {
        "foo",
        _foo,
        METH_VARARGS,
        ""
    },
    {
        "setup",
        setup,
        METH_NOARGS,
        ""
    },
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initabe(void) {
    (void) Py_InitModule("abe", AbeModuleMethods);
}

int main() {
    PyObject *tmp = setup(NULL, NULL);
    (void)tmp;
    return 0;
}
