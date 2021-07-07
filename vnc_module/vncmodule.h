/**
* \file vncmodule.h
*
*/
#ifndef _VNC_MODULE_H_
#define _VNC_MODULE_H_

#pragma warning(disable:4127)       //__multiarray_api.h(1645): warning C4127: conditional expression is constant
#pragma warning(disable:4244)       //'argument' : conversion from 'npy_intp' to 'int', possible loss of data
//#pragma warning(disable:4505)       // unreferenced local function has been removed

#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy\arrayobject.h>

#define VNC_USE_STDINT_H
#include "headers/vncamt.h"

#define DEBUG_ON
#define LOG_ON
#include "headers/dbg.h"


VNCViewerSDK gVNCSdk; // VNC Viewer SDK

#define MAX_VIEWERS     3
#define IP_MAX_LEN      15

static PyObject *py_StatusCallback = NULL;
static PyObject *py_CredentialsCallback = NULL;
static PyObject *py_ServerInitCallback = NULL;

static PyObject *py_ImageRectangleCallback = NULL;
static PyObject *py_CopyRectangleCallback = NULL;
static PyObject *py_FillRectangleCallback = NULL;
static PyObject *py_DesktopResizeCallback = NULL;


// WE ALLWAYS WANT TO DRAW OUR VIEWER //BOOLEAN drawViewer;     // True:Use default // False:Draw your stuff
typedef struct _VNC_VIEWER_CONTEXT
{
    char *ipAddress;                    // Unique, right?
    PyObject *self;                     // Python Viewer Instance    [!] if some UBER unknown probloem occurs, probably i'll need to incref this. not sure yet :)
    VNCViewer *pViewer;                 // The Viewer
    VNCViewerCallbacks vncCallbacks;    // Viewer's callbacks    

}VIEWER_CONTEXT, *PVIEWER_CONTEXT;

typedef struct _LIST_T
{
    struct _LIST_T *next;
    struct _LIST_T *prev;

    PVIEWER_CONTEXT context;
}LIST_T, *PLIST_T;


PLIST_T gViewerList = NULL;

/*
 * Insert a new node in the front of list. Return TRUE if success
 */
BOOLEAN 
ListInsert(
    __in char *IpAddress,
    __in VNCViewer *Viewer,
    __in VNCViewerCallbacks Callbacks
    );

/*
 * Delete node from list. Return TRUE if success
 */
BOOLEAN
ListDeleteNode(
    __in PLIST_T Node           // Node to be deleted
    );

/*
 * Returns pointer if IP found in list, NULL if not found
 */
PLIST_T 
ListGetNode(
    __in char *IpAdress
    );

#endif