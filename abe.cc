#include <string>
#include <vector>
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



static PyObject* setup(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
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

// TODO char* to GbyteArray*
//GByteArray* eat

static PyObject* keygen(PyObject *self, PyObject *args) {
    (void)self;

    char *pk, *mk;
    int pk_len, mk_len;
    PyObject *attr_tuple;

    if (!PyArg_ParseTuple(args, "t#t#O",
                &pk, &pk_len,
                &mk, &mk_len,
                &attr_tuple
                )) {
        return nullptr;
    }


    //auto pub = bswabe_pub_unserialize(pk)
    
    std::vector<std::string> attrs;
    auto attrlen = PyTuple_Size(attr_tuple);
    for (int i = 0; i < attrlen; ++i) {
        attrs.push_back(PyString_AsString(PyTuple_GetItem(attr_tuple, i)));
    }

    //pub = bswabe_pub_unserrialize();

    PyMem_Free(pk);
    PyMem_Free(mk);
    PyMem_Free(attr_tuple);
    return PyLong_FromLong(mk_len);
}

static PyMethodDef AbeModuleMethods[] = {
    //{
        //"foo",
        //_foo,
        //METH_VARARGS,
        //""
    //},
    {
        "setup",
        setup,
        METH_NOARGS,
        ""
    },
    {
        "keygen",
        keygen,
        METH_VARARGS,
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
