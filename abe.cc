#include <string>
#include <vector>
#include <python2.7/Python.h>
#include <unistd.h>
#include <glib.h>
#include <pbc/pbc.h>
#include <pbc/pbc_random.h>
#include <bswabe.h>
#include <cstdio>

#include "cpabe.h"

#include "private.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

int foo(int a, int b) {
    return a + b;
}


namespace {


void do_free_prv(bswabe_prv_t *prv) {
    element_clear(prv->d);
    for (size_t i = 0; i < prv->comps->len; ++i) {
        bswabe_prv_comp_t c;
        c = g_array_index(prv->comps, bswabe_prv_comp_t,  i);
        element_clear(c.d);
        element_clear(c.dp);
    }
    g_array_free(prv->comps, 1);
    free(prv);
}

}





static PyObject* setup(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    bswabe_pub_t *pub;
    bswabe_msk_t *msk;

    bswabe_setup(&pub, &msk);
    GByteArray *p = bswabe_pub_serialize(pub);
    GByteArray *m = bswabe_msk_serialize(msk); PyObject *ret = NULL;
    ret = PyTuple_Pack(
            2,
            PyString_FromStringAndSize(reinterpret_cast<const char*>(p->data), p->len),
            PyString_FromStringAndSize(reinterpret_cast<const char*>(m->data), m->len)
            );

    g_byte_array_free(m, 1);
    g_byte_array_free(p, 1);
    bswabe_msk_free(msk);
    bswabe_pub_free(pub);
    return ret;
}


// keygen(pub_path, msk_path, attrs)
static PyObject* keygen(PyObject *self, PyObject *args) {
    (void)self;
    

    char *pub_path, *msk_path;
    PyObject *attr_tuple;

    if (!PyArg_ParseTuple(args, "ssO", &pub_path, &msk_path, &attr_tuple) ) {
        return NULL;
    }
    
    std::vector<std::string> attrs;
    int attrlen = PyTuple_Size(attr_tuple);
    for (int i = 0; i < attrlen; ++i) {
        attrs.push_back(PyString_AsString(PyTuple_GetItem(attr_tuple, i)));
    }
    std::vector<char*> attributes;
    for (int i = 0; i < attrlen; ++i) {
        attributes.push_back(const_cast<char*>(attrs[i].data()));
    }
    attributes.push_back(NULL);


    bswabe_pub_t *pub = bswabe_pub_unserialize(suck_file(pub_path), 1);
    bswabe_msk_t *msk = bswabe_msk_unserialize(pub, suck_file(msk_path), 1);
    bswabe_prv_t *prv = bswabe_keygen(pub, msk, attributes.data());

    GByteArray *prv_gbyte = bswabe_prv_serialize(prv);

    PyObject *ret = NULL;
    std::string content(reinterpret_cast<char*>(prv_gbyte->data), prv_gbyte->len);
    //ret = PyString_FromStringAndSize(reinterpret_cast<const char*>(prv_gbyte->data), prv_gbyte->len);
    //ret = PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(prv_gbyte->data), prv_gbyte->len);


    g_byte_array_free(prv_gbyte, 1);
    do_free_prv(prv);
    bswabe_msk_free(msk);
    bswabe_pub_free(pub);

    return PyString_FromStringAndSize(content.data(), content.size());
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
    //PyObject *tmp = setup(NULL, NULL);
    //(void)tmp;
    return 0;
}
