#include <string>
#include <cstring>
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
#include "policy_lang.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

int foo(int a, int b) {
    return a + b;
}


namespace {

gint
comp_string( gconstpointer a, gconstpointer b)
{
	return std::strcmp(static_cast<const char*>(a), static_cast<const char*>(b));
}

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
    

    char *pub_path, *msk_path, *attr_str;

    if (!PyArg_ParseTuple(args, "sss", &pub_path, &msk_path, &attr_str) ) {
        return nullptr;
    }
    GSList *alist = nullptr;
    parse_attribute(&alist, attr_str);
    alist = g_slist_sort(alist, comp_string);
    
    std::vector<char*> attrs;
    for (auto ap = alist; ap; ap = ap->next) {
        attrs.push_back(static_cast<char*>(ap->data));
    }
    attrs.push_back(nullptr);

    auto pub = bswabe_pub_unserialize(suck_file(pub_path), 1);
    auto msk = bswabe_msk_unserialize(pub, suck_file(msk_path), 1);
    auto prv = bswabe_keygen(pub, msk, attrs.data());
    auto gb_prv = bswabe_prv_serialize(prv);

    auto ret = PyString_FromStringAndSize(reinterpret_cast<const char*>(gb_prv->data), gb_prv->len);

    g_byte_array_free(gb_prv, 1);
    do_free_prv(prv);
    bswabe_msk_free(msk);
    bswabe_pub_free(pub);

    return ret;
}

// cipher = encrypt(pub_path, message, attr)
static PyObject* encrypt(PyObject *self, PyObject *args) {
    (void)self;
    

    char *pub_path, *message, *policy; // needfree
    int msg_len;

    if (!PyArg_ParseTuple(args, "ss#s",
                &pub_path, 
                &message, &msg_len,
                &policy) ) {
        return NULL;
    }

    auto pub = bswabe_pub_unserialize(suck_file(pub_path), 1);
    // needfree
    
    bswabe_cph_t *cph = nullptr;
    element_t m;
    char *final_policy = parse_policy_lang(policy);

    if (!(cph = bswabe_enc(pub, m, final_policy)) ) {
        return nullptr;
    }
    free(final_policy);
    cerr << policy << endl;

    auto cph_buf = bswabe_cph_serialize(cph); // needfree
    bswabe_cph_free(cph);

    GByteArray *plt = g_byte_array_new();
    g_byte_array_set_size(plt, msg_len);
    std::memcpy(plt->data, message, msg_len);

    auto aes_buf = aes_128_cbc_encrypt(plt, m);

    g_byte_array_free(plt, 1);
    element_clear(m);

    std::string cipher;
    cipher.reserve(4 + 4 + aes_buf->len + 4 + cph_buf->len);
    for (int i = 3; i >= 0; --i) {
        cipher.push_back((msg_len & 0xff << (i << 3)) >> (i << 3));
    }
    for (int i = 3; i >= 0; --i) {
        cipher.push_back((aes_buf->len & 0xff << (i << 3)) >> (i << 3));
    }
    cipher.insert(cipher.end(), aes_buf->data, aes_buf->data + aes_buf->len);
    for (int i = 3; i >= 0; --i) {
        cipher.push_back((cph_buf->len & 0xff << (i << 3)) >> (i << 3));
    }
    cipher.insert(cipher.end(), cph_buf->data, cph_buf->data + cph_buf->len);

    bswabe_pub_free(pub);
    g_byte_array_free(cph_buf, 1);
    g_byte_array_free(aes_buf, 1);
    
    return PyString_FromStringAndSize(cipher.data(), cipher.size());
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
    {
        "encrypt",
        encrypt,
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
