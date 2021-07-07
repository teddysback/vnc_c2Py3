#include "vncmodule.h"
#include <time.h>

///
/// LIST MANAGEMENT
///
BOOLEAN
ListInsert(
    __in char *IpAddress,
    __in PyObject *Self,
    __in VNCViewer *Viewer,
    __in VNCViewerCallbacks Callbacks
    )
{
    BOOLEAN bFail;
    PLIST_T newNode;
    
    bFail = FALSE;
    newNode = NULL;

    // Check args
    if (NULL == IpAddress || NULL == Viewer || NULL == Self)
    {
        LOG_ERROR(ERROR_INVALID_PARAMETER, "Check args! They must not be NULL");
        return bFail;
    }
    
    // Alloc 
    newNode = (PLIST_T)malloc(sizeof(LIST_T));
    if (NULL == newNode)
    {
        LOG_ERROR(0, "malloc LIST_NODE failed!");
        bFail = TRUE;
        free(newNode);        
        goto end_this;
    }

    newNode->context = (PVIEWER_CONTEXT)malloc(sizeof(VIEWER_CONTEXT));
    if (NULL == newNode->context)
    {
        LOG_ERROR(0, "malloc VIEWER_CONTEXT failed!");
        bFail = TRUE;
        free(newNode);
        free(newNode->context);
        goto end_this;
    }

    // Set info
    newNode->context->self = Self;
    newNode->context->pViewer = Viewer;       
    newNode->context->vncCallbacks = Callbacks;
    newNode->context->ipAddress = (char*)malloc(sizeof(char) * (strlen(IpAddress) + 1));
    strcpy(newNode->context->ipAddress, IpAddress);
    
    // Link new node to list
    newNode->prev = NULL;
    newNode->next = gViewerList;
    if (gViewerList)
    {
        gViewerList->prev = newNode;
    }
    gViewerList = newNode;
    

end_this:
    return !bFail;
}

BOOLEAN
ListDeleteNode(
    __in PLIST_T Node
    )
{
    BOOLEAN bFail;

    bFail = FALSE;
        
    if (NULL == Node)
    {
        LOG_ERROR(ERROR_INVALID_PARAMETER, "Node should not be NULL");
        return bFail;
    }

    // is head?
    if (Node == gViewerList)
    {
        gViewerList = Node->next;
    }
    // NOT last?
    if (Node->next != NULL)
    {
        Node->next->prev = Node->prev;
    }// NOT first
    if (Node->prev != NULL)
    {
        Node->prev->next = Node->next;
    }
    free(Node->context);
    free(Node);

    return !bFail;
}

PLIST_T
ListGetNode(
    __in char *IpAdress
    )
{
    PLIST_T temp;
    temp = gViewerList;

    while (temp)
    {
        if (!strcmp(temp->context->ipAddress, IpAdress))
        {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

/*
 * Print viewers' ip. Private method for debug use 
 */
VOID
ListPrint(
    void
    )
{
    PLIST_T temp;
    DWORD number;

    temp = NULL;
    number = 0;

    temp = gViewerList;
    while (temp)
    {
        number++;
        DEBUG("viewer[%d] ip[%s]", number, temp->context->ipAddress);
        temp = temp->next;
    }

    return;
}

///
/// TEST
///
static PyObject *
test_func(
    PyObject* self, 
    PyObject* args
    )
{
    UNREFERENCED_PARAMETER(self);
    UNREFERENCED_PARAMETER(args);

    ListPrint();
    return Py_BuildValue("i", 1);
}

///
/// ESSENSIAL Callbacks
///
/* VNCViewerServerInitCallback */
void VNCCALL
ServerInitCallback(
    VNCViewer *pViewer,
    void *pContext,
    vnc_uint16_t width,
    vnc_uint16_t height,
    const char *desktopName,
    const VNCPixelFormat *pServerNativePixelFormat
    )
{
    UNREFERENCED_PARAMETER(pViewer);
    
    //DEBUG("[dbg] >>>THIS IS [%s]<<<", context->ipAddress);
    DEBUG("[dbg]red max   %d redshift    %d", pServerNativePixelFormat->redMax, pServerNativePixelFormat->redShift);
    DEBUG("[dbg]green max %d green shift %d", pServerNativePixelFormat->greenMax, pServerNativePixelFormat->greenShift);
    DEBUG("[dbg]blue max  %d blue shift  %d", pServerNativePixelFormat->blueMax, pServerNativePixelFormat->blueShift);

    DEBUG("[dbg]bps %d", pServerNativePixelFormat->bitsPerPixel);


    if (py_ServerInitCallback == NULL)
    {
        LOG_ERROR(0, "ServerInit Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject  *arglist, *result;
        PVIEWER_CONTEXT context;

        
        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        /* Call py func with desired args */
        arglist = Py_BuildValue("(Oiisiiiiiii)", context->self, width, height, desktopName,
            pServerNativePixelFormat->redMax, pServerNativePixelFormat->greenMax, pServerNativePixelFormat->blueMax,
            pServerNativePixelFormat->redShift, pServerNativePixelFormat->greenShift, pServerNativePixelFormat->blueShift,
            pServerNativePixelFormat->bigEndianFlag);

        /* [!] should create a 'pixel buffer' into which you can copy desktop updates.
        If you previously created a pixel buffer, you should discard it.
        The new pixel buffer will be refreshed shortly. */

        result = PyObject_CallObject(py_ServerInitCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "PyObject_CallObject resul is NULL");
        }
        Py_DECREF(arglist);
        Py_XDECREF(result);

        /* Release GIL */
        PyGILState_Release(state);
    }
    //DEBUG("%s FINISHED ", __FUNCTION__);
    return;
}

/* VNCViewerStatusCallback */
vnc_int32_t VNCCALL
StatusCallback(
    VNCViewer *pViewer,
    void *pContext,
    VNCViewerStatus status,
    void *pMoreStatus
    )
{
    UNREFERENCED_PARAMETER(pViewer);

    const char *ViewerStatusString[] =
    {
        "VNCViewerStatus: None",
        "VNCViewerStatus: DNS Failure",
        "VNCViewerStatus: Network Unreachable",
        "VNCViewerStatus: Host Unreachable",
        "VNCViewerStatus: Connection TimedOut",
        "VNCViewerStatus: Connection Refused",
        "VNCViewerStatus: Server Unsupported",
        "VNCViewerStatus: Authentication Failure",
        "VNCViewerStatus: Server Signature Failure",
        "VNCViewerStatus: Disconnected",
        "VNCViewerStatus: Exited",
        "VNCViewerStatus: Stopped",
        "VNCViewerStatus: Connection Closed"
    };

    if (py_StatusCallback == NULL)
    {
        LOG_ERROR(0, "%s not provided", __func__);
    }
    else
    {
        PyGILState_STATE state;
        PyObject *arglist;
        PyObject *result;
        PVIEWER_CONTEXT context;


        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        /* Call py func with desired args */
        switch (status)
        {
        case VNCViewerStatusAuthenticationFailure: // pMoreStatus points to a NUL-terminated string    
            arglist = Py_BuildValue("(Oiss)", context->self, status, ViewerStatusString[status], pMoreStatus);
            break;
        case VNCViewerStatusConnectionClosed:      // pMoreStatus points to a VNCConnClose structure
            arglist = Py_BuildValue("(Oiss)", context->self, status, ViewerStatusString[status], ((VNCConnClose *)pMoreStatus)->reasonString);
            break;
        default:                                   // pMoreStatus is unused and will be NULL
            arglist = Py_BuildValue("(Oiss)", context->self, status, ViewerStatusString[status], pMoreStatus);
            break;
        }

        result = PyObject_CallObject(py_StatusCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "PyObject_CallObject resul is NULL");
        }

        Py_DECREF(arglist);
        Py_XDECREF(result);      

        /* Release GIL */
        PyGILState_Release(state);
    }
   
    return 0;
}

/* VNCViewerCredentialsCallback */
vnc_int32_t VNCCALL
CredentialsCallback(
    VNCViewer *pViewer,
    void *pContext,
    char **pUsername,
    char **pPassword
    )
{
    UNREFERENCED_PARAMETER(pViewer);
    
    if (py_CredentialsCallback == NULL)
    {
        LOG_ERROR(0, "Credentials Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject *result;
        PyObject *user_str, *pass_str;
        PyObject *user, *pass;
        PyObject *arglist;
        char *username, *password;
        PVIEWER_CONTEXT context;


        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        /* Call py func */
        DEBUG(">> %s >> %p", context->ipAddress, context->self);
        arglist = Py_BuildValue("(O)", context->self);
        result = PyObject_CallObject(py_CredentialsCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "PyObject_CallObject resul is NULL");
        }
        if (PyTuple_Check(result))
        {
            /* Set user & pass */
            user = PyTuple_GetItem(result, 0);
            pass = PyTuple_GetItem(result, 1);

            user_str = PyObject_Str(user);
            pass_str = PyObject_Str(pass);

            username = PyUnicode_AsUTF8(user_str);
            password = PyUnicode_AsUTF8(pass_str);

            if (pUsername)
            {
                *pUsername = (char*)gVNCSdk.vncAllocString(username, strlen(username));
            }
            if (pPassword)
            {
                *pPassword = (char*)gVNCSdk.vncAllocString(password, strlen(password));
            }
        }
        else
        {
            LOG_ERROR(0, "[ERROR] You should return a tuple: (\"user\", \"pass\")");
            if (pUsername)
            {
                *pUsername = (char*)gVNCSdk.vncAllocString("junk", 4);
            }
            if (pPassword)
            {
                *pPassword = (char*)gVNCSdk.vncAllocString("junk", 4);
            }
        }
        /* Release GIL */
        PyGILState_Release(state);
    }
    return 0;
}

///
/// DESKTOP UPDATE Callbacks
///
void VNCCALL
ImageRectangleCallback(
    VNCViewer *pViewer,
    void *pContext,
    const VNCRectangle *pRectangle,
    const vnc_uint8_t *pixelData,
    size_t pixelDataLength
    )
{
    UNREFERENCED_PARAMETER(pViewer);

    if (py_ImageRectangleCallback == NULL)
    {
        LOG_ERROR(0, "[ERROR] ImageRectangle Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject *pixelMatrix;
        PyObject *arglist;
        PyObject *result;
        PVIEWER_CONTEXT context;
        npy_intp dimensions;
        npy_intp *size;

        pixelMatrix = NULL;
        arglist = NULL;
        result = NULL;
        size = NULL;
        context = NULL;
        dimensions = 2;  // 2D numpy array

        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        size = (npy_intp*)malloc(sizeof(npy_intp) * 2);
        size[0] = pRectangle->bottomRight.x - pRectangle->topLeft.x; // X axis - pixel no
        size[1] = pRectangle->bottomRight.y - pRectangle->topLeft.y; // Y axis

        pixelMatrix = PyArray_SimpleNewFromData(dimensions, size, NPY_UINT16, (char*)pixelData);// doc - inc ref +++ [!]NPY_UINT16
        Py_INCREF(pixelMatrix);

        // cred ca ar trebui sa creez ob pentru topleft etc, inainte sa le trimit
        // MUST READ DOCs
        arglist = Py_BuildValue("(O(ii)(ii)Oi)", context->self,
            pRectangle->topLeft.x, pRectangle->topLeft.y,
            pRectangle->bottomRight.x, pRectangle->bottomRight.y,
            pixelMatrix, pixelDataLength);
        if (arglist == NULL)
        {
            DEBUG("[DEBUG - error] ");
        }

        result = PyObject_CallObject(py_ImageRectangleCallback, arglist);
        if (result == NULL)
        {
            // LOG_ERROR(0, "[ERROR] %s resul is NULL", __FUNCTION__);
        }
        Py_DECREF(arglist);
        Py_XDECREF(result);

        /* Release GIL */
        PyGILState_Release(state);
    }
    return;
}

/* VNCViewerCopyRectangleCallback  */
void VNCCALL
CopyRectangleCallback(
    VNCViewer *pViewer,
    void *pContext,
    const VNCRectangle *pDestination,
    const VNCPoint *pSource
    )
{
    UNREFERENCED_PARAMETER(pViewer);

    if (py_CopyRectangleCallback == NULL)
    {
        LOG_ERROR(0, "[ERROR] CopyRectangle Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject  *arglist, *result;
        PVIEWER_CONTEXT context;

        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        arglist = Py_BuildValue("(O(iiii)(ii))", context->self,
            pDestination->topLeft.x, pDestination->topLeft.y,
            pDestination->bottomRight.x, pDestination->bottomRight.y,
            pSource->x, pSource->y
            );
        result = PyObject_CallObject(py_CopyRectangleCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "[ERROR] PyObject_CallObject resul is NULL");
        }
        Py_DECREF(arglist);
        Py_XDECREF(result);

        /* Release GIL */
        PyGILState_Release(state);
    }

    return;
}

/* VNCViewerFillRectangleCallback */
void VNCCALL
FillRectangleCallback(
    VNCViewer *pViewer,
    void *pContext,
    const VNCRectangle *pRectangle,
    const vnc_uint8_t *pixelData,
    size_t pixelDataLength
    )
{
    UNREFERENCED_PARAMETER(pViewer);

    if (py_FillRectangleCallback == NULL)
    {
        LOG_ERROR(0, "[ERROR] FillRectangle Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject *pyPixelData;
        PyObject *arglist;
        PyObject *result;
        PVIEWER_CONTEXT context;

        pyPixelData = NULL;
        arglist = NULL;
        result = NULL;
        context = NULL;

        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        pyPixelData = PyByteArray_FromStringAndSize((const char *)pixelData, pixelDataLength);
        Py_INCREF(pyPixelData);


        /* Call py func with desired args */
        arglist = Py_BuildValue("(O(ii)(ii)O)", context->self,
            pRectangle->topLeft.x, pRectangle->topLeft.y,
            pRectangle->bottomRight.x, pRectangle->bottomRight.y,
            pyPixelData);

        result = PyObject_CallObject(py_FillRectangleCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "[ERROR] %s resul is NULL", __FUNCTION__);
        }
        Py_DECREF(arglist);
        Py_XDECREF(result);

        /* Release GIL */
        PyGILState_Release(state);
    }

    return;
}

/* VNCViewerDesktopResizeCallback */
void VNCCALL
DesktopResizeCallback(
    VNCViewer *pViewer,
    void *pContext,
    vnc_uint16_t width,
    vnc_uint16_t height
    )
{
    UNREFERENCED_PARAMETER(pViewer);

    if (py_DesktopResizeCallback == NULL)
    {
        LOG_ERROR(0, "[ERROR] DesktopResize Callback not provided");
    }
    else
    {
        PyGILState_STATE state;
        PyObject *arglist;
        PyObject *result;
        PVIEWER_CONTEXT context;

        context = NULL;

        /* Acquire  GIL */
        state = PyGILState_Ensure();
        context = (PVIEWER_CONTEXT)pContext;

        /* Call py func with desired args */
        arglist = Py_BuildValue("(O(ii))",context->self, width, height);
        result = PyObject_CallObject(py_DesktopResizeCallback, arglist);
        if (result == NULL)
        {
            LOG_ERROR(0, "[ERROR] %s result is NULL. arglist: (%d,%d)", __FUNCTION__, width, height);
        }
        Py_DECREF(arglist);
        Py_XDECREF(result);

        /* Release GIL */
        PyGILState_Release(state);
    }

    return;
}




///
/// init & uninit
///
// [!] init is made in pymodule init
// [!] Should have cleanup :Uninit: & :FreeLibrary:

///
/// Viewer Management
///

// - register CRIT callbacks 
// - register OTHER callbacks, or init to 0
// - create viewer (cb, size of cb)

// - set/register stuff

// - gen rsa
// - set rsa
// - free string

// ---- START

// - destroy viewer



///
/// Helpers
///
void
InitializeCallbacks(
    __inout VNCViewerCallbacks *Callbacks
    )
{
    ZeroMemory(Callbacks, sizeof VNCViewerCallbacks);

    // mandatory cb
    Callbacks->pServerInitCallback = ServerInitCallback;
    Callbacks->pStatusCallback = StatusCallback;
    Callbacks->pCredentialsCallback = CredentialsCallback;
    // desktop update callbacks
    Callbacks->pImageRectangleCallback = ImageRectangleCallback;
    Callbacks->pCopyRectangleCallback = CopyRectangleCallback;
    Callbacks->pFillRectangleCallback = FillRectangleCallback;
    Callbacks->pDesktopResizeCallback = DesktopResizeCallback;
   
    /* ADD HERE CALLBACKS WHEN IMPLEMENTED (@reg_cb + globals) */

}

BOOLEAN
RegisterCallback(
    __in char *FunctionName,
    __in PyObject *Value
    )
{
    BOOLEAN bFound = TRUE;

    /* Register callback */
    if (!stricmp(FunctionName, "StatusCallback"))
    {
        Py_XINCREF(Value);              /* Add a reference to new callback */
        Py_XDECREF(py_StatusCallback);  /* Dispose of previous callback */
        py_StatusCallback = Value;      /* Remember new callback */
    }
    else if (!stricmp(FunctionName, "ServerInitCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_ServerInitCallback);
        py_ServerInitCallback = Value;
    }
    else if (!stricmp(FunctionName, "CredentialsCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_CredentialsCallback);
        py_CredentialsCallback = Value;
    }
    else if (!stricmp(FunctionName, "ImageRectangleCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_ImageRectangleCallback);
        py_ImageRectangleCallback = Value;
    }
    else if (!stricmp(FunctionName, "CopyRectangleCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_CopyRectangleCallback);
        py_CopyRectangleCallback = Value;
    }
    else if (!stricmp(FunctionName, "FillRectangleCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_FillRectangleCallback);
        py_FillRectangleCallback = Value;
    }
    else if (!stricmp(FunctionName, "DesktopResizeCallback"))
    {
        Py_XINCREF(Value);
        Py_XDECREF(py_DesktopResizeCallback);
        py_DesktopResizeCallback = Value;
    }
    /* [ADD CALLBACKS HEREEEEE] */
    else // default
    {
        DEBUG("[DEBUG] me no likey: %s", FunctionName);
        bFound = FALSE;
        // [!] will you handle this or just print shit like a bitch
        // [!!!] NEEAEAAAAPARAT fail daca nu primesc callback urile care le vreau
    }

    return bFound;
}

///
/// Module FUNCTIONS 
///
static PyObject *
module_CreateViewer(
    PyObject* self,
    PyObject* args
    )
{
    int retCode;
    char *funcName;
    char *serverName; 
    PLIST_T node;
    Py_ssize_t pos;
    PyObject *dict, *key, *value;
    PyObject *key_str;    
    PyObject *viewer_self;
    VNCViewer *viewer;
    VNCViewerCallbacks vncCallbacks;
    
    UNREFERENCED_PARAMETER(self);
    pos = 0;
    retCode = 0;

    
    // Check arguments
    if (!PyArg_ParseTuple(args, "O!sO", &PyDict_Type, &dict, &serverName, &viewer_self))
    {        
        PyErr_Format(PyExc_TypeError, "You need to pass a dictionary with callback functions");
        return NULL;
    }
   
    // Callback
    memset(&vncCallbacks, 0, sizeof(vncCallbacks));
    InitializeCallbacks(&vncCallbacks);
    
    //Register Callbacks
    while (PyDict_Next(dict, &pos, &key, &value))
    {
        if (!PyCallable_Check(value))
        {
            PyErr_SetString(PyExc_TypeError, "Parameter must be callable");
            return NULL;
        }

        /* Prepare key and check value, then register the callback*/
        key_str = PyObject_Str(key);
        funcName = PyUnicode_AsUTF8(key_str);
        if (!RegisterCallback(funcName, value))
        {
            // FAIULLLL 
            // trebuie sa arunc niste erori.. sa dau eject la tot daca nu am callback uri
            PyErr_Format(PyExc_TypeError, "Where the hell did you get this function name? Double check this: [%s]", funcName);
            return NULL;
        }
        // *** CHECK IF ALL CALLBACKS ARE SET *** ??
        // if not failed!
    }
   
    // Create Viewer
    viewer = gVNCSdk.vncViewerCreate(&vncCallbacks, sizeof(vncCallbacks), FALSE);// FALSE = Always draw you own shit
    if (NULL == viewer)
    {   
        LOG_ERROR(0, "Viewer could not be created!");
        retCode = 1;
        goto end_this;
    }

    // You should keep this
    if (!ListInsert(serverName, viewer_self, viewer, vncCallbacks))
    {
        LOG_ERROR(0, "Somehow the insert failed"); // [!] destroy viewer / and other:-??
        retCode = 1;
        goto end_this;
    }

    // Set that Context ;)
    node = ListGetNode(serverName);
    if (NULL == node)
    {
        LOG_ERROR(0, "ListGetNode failed for [%s]. List probably corrupt.", serverName);// [!] destroy viewer / and other:-??
        retCode = 1;
        goto end_this;
    }
    gVNCSdk.vncViewerSetContext(node->context->pViewer, node->context);
    
end_this:
    return Py_BuildValue("i", retCode);
}


static PyObject *
module_StartViewer(
    PyObject* self,
    PyObject* args
    )
{
    char *serverName;
    int retCode;
    PLIST_T node;

    node = NULL;
    retCode = 0;
    UNREFERENCED_PARAMETER(self);


    if (!PyArg_ParseTuple(args, "s", &serverName))  
    {
        return NULL;
    }

    node = ListGetNode(serverName);
    if (NULL == node)
    {
        LOG_ERROR(0, "ListGetNode failed for [%s]. You entered a bad IP or list is corrupt.", serverName);
        retCode = 1;
        goto end_this;
    }

    gVNCSdk.vncViewerStart(node->context->pViewer, serverName);
    
end_this:
    return Py_BuildValue("i", retCode);
}


static PyObject *
module_StopViewer(
    PyObject* self,
    PyObject* args
    )
{
    int retCode;
    char *serverName;
    PLIST_T node;

    retCode = 0;
    node = NULL;
    UNREFERENCED_PARAMETER(self);


    if (!PyArg_ParseTuple(args, "s", &serverName))
    {
        return NULL;
    }
    DEBUG("will stop %s", serverName);
    node = ListGetNode(serverName);
    if (NULL == node)
    {
        LOG_ERROR(0, "ListGetNode failed for [%s]. You entered a bad IP or list is corrupt.", serverName);
        retCode = 1;
        goto end_this;
    }
    gVNCSdk.vncViewerStop(node->context->pViewer);
    DEBUG(" VIEWER STOPPED");

end_this:
    return Py_BuildValue("i", retCode);    
}

static PyObject *
module_DestroyViewer(
    PyObject* self,
    PyObject* args
    )
{
    int retCode;
    char *serverName;
    PLIST_T node;

    retCode = 0;
    node = NULL;

    UNREFERENCED_PARAMETER(self);


    if (!PyArg_ParseTuple(args, "s", &serverName))
    {
        return NULL;
    }
    DEBUG("to be rekt %s", serverName);
    node = ListGetNode(serverName);
    if (NULL == node)
    {
        LOG_ERROR(0, "ListGetNode failed for [%s]. You entered a bad IP or list is corrupt.", serverName);
        retCode = 1;
        goto end_this;
    }
    gVNCSdk.vncViewerDestroy(node->context->pViewer);
    DEBUG("sdk destroied %s", node->context->ipAddress);
    ListDeleteNode(node);
    DEBUG("node deleted");

end_this:
    return Py_BuildValue("i", retCode);
}

///
/// Let's play a bit more
///
static PyObject *
module_Mouse(
    PyObject* self,
    PyObject* args
    )
{
    int retCode;
    int x, y, mouse;
    char *serverName;
    PLIST_T node;

    x = 0;
    y = 0;
    retCode = 0;
    node = NULL;

    UNREFERENCED_PARAMETER(self);


    if (!PyArg_ParseTuple(args, "siii", &serverName, &mouse, &x, &y))
    {
        return NULL;
    }    
    node = ListGetNode(serverName);
    if (NULL == node)
    {
        LOG_ERROR(0, "ListGetNode failed for [%s]. You entered a bad IP or list is corrupt.", serverName);
        retCode = 1;
        goto end_this;
    }

    //VNCViewerPrePointerEventCallback    
    DEBUG("%p [%s] (%d,%d)", node->context->pViewer, node->context->ipAddress, x, y);
    PyGILState_STATE state;
    state = PyGILState_Ensure();
    __try
    {                
        gVNCSdk.vncViewerSendPointerEvent(node->context->pViewer, (VNCPointerDeviceButton)mouse, x, y);
        DEBUG(">>> DDDOOONNNNEEE <<<");
    }
    __finally
    {
        PyGILState_Release(state);
    }
    
    

end_this:
    return Py_BuildValue("i", retCode);
}

///
/// Mandatory py module stuff
///
static PyMethodDef VncModuleMethods[] = 
{
    { "test", test_func, METH_VARARGS, "test shitz" },

    { "create_viewer",  module_CreateViewer,  METH_VARARGS, "create a viewer" },
    { "start_viewer",   module_StartViewer,   METH_VARARGS, "start a viewer" },
    { "stop_viewer",    module_StopViewer,    METH_VARARGS, "stop a viewer" },
    { "destroy_viewer", module_DestroyViewer, METH_VARARGS, "destroy a viewer" },
    { "mouse",          module_Mouse,         METH_VARARGS, "manage mouse events ^_^" },
    //{ "free", free, METH_VARARGS, "free library"},

    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef vncmodule =
{
    PyModuleDef_HEAD_INIT,
    "vncmodule",        /* name of module */
    NULL,            /* module documentation, may be NULL */
    -1,              /* size of per-interpreter state of the module,
                     or -1 if the module keeps state in global variables. */
    VncModuleMethods
};

PyMODINIT_FUNC
PyInit_vncmodule(
    void
    )
{
    PyObject *module;                                // the Module
    HMODULE hVNCDll;                                 // DLL handle
    VNCViewerSDKInitializeType* VncSdkInitialize;    // Function point


    module = PyModule_Create(&vncmodule);
    if (module == NULL)
        return NULL;
    import_array();

    /* Start VNCAMT.DLL Init here*/
    // Load library
    hVNCDll = LoadLibrary("lib/vncamt.dll"); // [!] can add to load default(dll is there) or load from path
    if (!hVNCDll)
    {
        PyErr_Format(PyExc_ImportError, "LoadLibrary failed with:0x%08x Check if vncamt.dll is there, or if arh matches", GetLastError());
        return NULL;
    }

    // Locate DLL entry point 
    VncSdkInitialize = (VNCViewerSDKInitializeType*)GetProcAddress(hVNCDll, "VNCViewerSDKInitialize");
    if (!VncSdkInitialize)
    {
        FreeLibrary(hVNCDll);

        PyErr_Format(PyExc_ImportError, "GetProcAddress for VNCViewerSDKInitialize failed with [0x%08x] ", GetLastError());
        return NULL;
    }

    // Initializer
    if (VncSdkInitialize(&gVNCSdk, sizeof(VNCViewerSDK)))
    {
        FreeLibrary(hVNCDll);

        PyErr_Format(PyExc_ImportError, "SDK initialize failed [0x%08x] ", GetLastError());
        return NULL;
    }
    /* <!>init */
    DEBUG("RealVNC Viewer SDK ver %d.%d.%d.%d for Intel AMT",
        gVNCSdk.versionMajor,
        gVNCSdk.versionMinor,
        gVNCSdk.versionPatch,
        gVNCSdk.versionBuild);
    
    return module;
}

///
/// Dodge Linker ERROR
///
void main(){return;}