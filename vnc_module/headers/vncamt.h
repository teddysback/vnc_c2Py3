/* Copyright (C) 2002-2008 RealVNC Ltd.  All Rights Reserved.
 */

#ifndef __VNCAMT_H__
#define __VNCAMT_H__

#include "vnctypes.h"
#include "vnccallbacks.h"
#include "vncparameters.h"

/**
 * \file vncamt.h
 *
 * This main header file for the RealVNC Viewer SDK for Intel AMT.  To use the
 * SDK, you need only include this file.
 *
 * \see VNCViewerSDKInitialize, VNCViewerSDK
 */

/**
 * \mainpage RealVNC Viewer SDK for Intel AMT
 *
 * \section section_overview Overview
 *
 * The RealVNC Viewer SDK allows to you to create and manipulate VNC viewer
 * objects.  A VNC viewer can connect to a VNC server and display the contents
 * of the VNC server's own desktop, either by a creating 'desktop window' that
 * renders the server desktop for you, or by notifying you of desktop updates
 * so that you can render the server desktop yourself.
 *
 * \section section_usage Using the SDK
 *
 * There are two ways to use the SDK.  The simplest way is to allow the VNC
 * viewer to create its default desktop window.  This window displays the
 * contents of the VNC server's server desktop and also captures the desktop
 * window's keyboard and mouse input events for transmision to the server.
 * Using the default desktop window requires you to write very little code, but
 * provides almost no capabilities for embellishing the desktop window's user
 * interface.  It is not possible, for example, to add a toolbar or a menu to
 * the default desktop window.
 *
 * If you do not wish to use the default desktop window, you can register
 * callbacks so that the SDK can notify you when changes have been made to the
 * server desktop.  You will then be responsible for drawing the contents of
 * the server desktop yourself.  Also, because there is no desktop window, the
 * SDK cannot gather keyboard and mouse input events - so you will have to
 * provide these yourself, too.  However, you will have the freedom to 'embed'
 * the VNC viewer in almost any application that can render the server desktop
 * and provide input events, and to add whatever user interface features you
 * want.
 *
 * The following sections summarize typical usage of the SDK in each of these
 * modes.
 *
 * \subsection subsection_usage_startup Loading and initializing the SDK
 *
 * This section applies whether or not you are using the default desktop
 * window.
 *
 * -# Load the SDK.
 *    - on Windows, call LoadLibrary()
 *    - on UNIX, call dlopen()
 * -# Locate the SDK entry point, VNCViewerSDKInitialize().
 *    - on Windows, call GetProcAddress()
 *    - on UNIX, call dlsym()
 * -# Call VNCViewerSDKInitialize().  This populates a VNCViewerSDK structure
 *    with pointers to the rest of the SDK's 'methods'.
 *
 * \subsection subsection_usage_default Using the default desktop window
 *
 * -# Call VNCViewerCreate() to create a VNC viewer, setting the
 *    createDesktopWindow flag to TRUE.  Because you are using the default
 *    desktop window, you do not need to implement the majority of the
 *    callbacks in the VNCViewerCallbacks structure, but you will almost always
 *    want to implement:
 *    - VNCViewerServerInitCallback, so that you are notified when a connection
 *      is successfully established.
 *    - VNCViewerStatusCallback, so that you are notified when a connection
 *      attempt is unsuccessful or is lost.
 *    - VNCViewerCredentialsCallback, so that the user can provide a username
 *      and password if the server requires them.
 * -# Use VNCViewerSetParameter() to set the desired parameters on the VNC
 *    viewer, possibly determining their values by displaying UI to the end
 *    user.
 *    - If you intend to support RA2 or RA2ne security, you must set the
 *      RSA_Private_Key parameter.  See VNCGenerateRSAKeys() for more
 *      information.
 * -# Call VNCViewerStart() to start the VNC viewer.  This initiates the VNC
 *    viewer's connection to the VNC server.  If the connection is successful,
 *    the VNC viewer will begin displaying its desktop window.
 * -# Continue executing until either:
 *    - your VNCViewerStatusCallback informs you that the VNC viewer has
 *      stopped, or
 *    - you close the connection yourself by calling VNCViewerStop().  In this
 *      case, you will still receive a call to your VNCViewerStatusCallback.
 * -# Use VNCViewerDestroy() to destroy the VNC viewer.
 *
 * By default, the default desktop window will forward all keyboard and
 * pointer-device events to the VNC server.  You can override this behaviour by
 * providing a VNCViewerPreKeyEventCallback or VNCViewerPrePointerEventCallback
 * and having it block the event.  For example, you could add an application
 * hotkey by checking in VNCViewerPreKeyEventCallback to see whether that key
 * had been released.  In this case the callback should probably also prevent
 * key down or key up events for that key from being sent to the server.
 *
 * \subsection subsection_usage_callbacks Using the desktop update callbacks
 *
 * -# Call VNCViewerCreate() to create a VNC viewer, setting the
 *    createDesktopWindow flag to FALSE.  In addition to the three callbacks
 *    mentioned above, you should implement:
 *    - VNCViewerImageRectangleCallback, VNCViewerCopyRectangleCallback and
 *      VNCViewerFillRectangleCallback.  These are the 'desktop update'
 *      callbacks.
 *    - VNCViewerPixelFormatChangedCallback, so that you know the format of the
 *      data provided to the desktop update callbacks.  You absolutely must
 *      implement this callback if you make any calls to
 *      VNCViewerSetPixelFormat().
 *    - VNCViewerDesktopResizeCallback, so that you are told if the server
 *      desktop is resized.  Not all platforms on which VNC servers run can
 *      support this; if you know that your application will not be interacting
 *      with VNC servers on platforms that that support desktop resize, then
 *      you can omit this step.
 * -# Use VNCViewerSetParameter() to set the desired parameters on the VNC
 *    viewer, possibly determining their values by displaying UI to the end
 *    user.
 *    - If you intend to support RA2 or RA2ne security, you must set the
 *      RSA_Private_Key parameter.  See VNCGenerateRSAKeys() for more
 *      information.
 * -# Call VNCViewerStart() to start the VNC viewer.  This initiates the VNC
 *    viewer's connection to the VNC server.  If the connection is successful,
 *    the VNC viewer will begin displaying its desktop window.
 * -# When a call is made to your VNCViewerServerInitCallback,
 *    VNCViewerPixelFormatChangedCallback or VNCViewerDesktopResizeCallback,
 *    you should create a 'pixel buffer' into which you can copy desktop
 *    updates.  (If you previously created a pixel buffer, you should discard
 *    it.  The new pixel buffer will be refreshed shortly.)  Typically, you
 *    will use a platform-specific bitmap object as the pixel buffer, and use
 *    the platform's standard mechanisms for painting the bitmap onto the
 *    screen.
 * -# When a call is made to any of the three desktop update callbacks, you
 *    will copy the update into the pixel buffer and then instruct the platform
 *    to refresh the corresponding part of the screen (i.e. 'invalidate' it).
 * -# Call VNCViewerSendKeyEvent() and VNCViewerSendPointerEvent() to provide
 *    input events to the VNC viewer.  Usually, you will call these in response
 *    to keyboard and mouse events generated by the user of the application.
 *    Note that if using the Scancode extension, VNCViewerSendUSBKeyEvent() 
 *    should be called instead for key events. If using the Relative Pointer 
 *    extension, then the co-ordinate arguemnts to VNCViewerSendPointerEvent()
 *    are no longer positions on the screen, but relative movement values.
 * -# Optionally, call VNCViewerSendClientCutText() when the local clipboard
 *    contents change and the new contents are text.  This will cause the VNC
 *    server's clipboard to be updated to reflect the new local clipboard
 *    contents.  If you provide a VNCViewerServerCutTextCallback, your
 *    application will be notified when the VNC server's clipboard contents
 *    change, so that you can update the local clipboard to reflect this.
 * -# Continue executing until either:
 *    - your VNCViewerStatusCallback informs you that the VNC viewer has
 *      stopped, or
 *    - you close the connection yourself by calling VNCViewerStop().  In this
 *      case, you will still receive a call to your VNCViewerStatusCallback.
 * -# Use VNCViewerDestroy() to destroy the VNC viewer.
 *
 * \subsection subsection_usage_shutdown Uninitializing and unloading the SDK
 *
 * This section applies whether or not you are using the default desktop
 * window.
 *
 * -# Call VNCViewerSDKUninitialize().
 * -# Unload the SDK.
 *    - on Windows, call FreeLibrary()
 *    - on UNIX, call dlclose()
 *
 * \section section_usage_notes Usage notes
 *
 * - Calling VNCViewerStart() starts a new thread of execution.  All of the
 *   SDK callbacks are invoked from that thread.  You must take care to ensure
 *   that your application responds to the callbacks in a thread-safe manner.
 *
 * - If you wish, you may start and stop the same VNC viewer object multiple
 *   times.  This allows you to make a succession of VNC connection attempts
 *   with the same configuration.  For example, you can use this pattern to
 *   handle the case where the VNC server challenges the VNC viewer for a
 *   username and password, but the user enters them incorrectly.
 *
 * - There are two ways to send the special key sequence Ctrl-Alt-Del to the
 *   server:
 *   -# Write code to do it explicitly:
 *      \code
 *      VNCViewerSendKeyEvent(pViewer, XK_Control_L, TRUE);
 *      VNCViewerSendKeyEvent(pViewer, XK_Alt_L, TRUE);
 *      VNCViewerSendKeyEvent(pViewer, XK_Delete, TRUE);
 *      VNCViewerSendKeyEvent(pViewer, XK_Delete, FALSE);
 *      VNCViewerSendKeyEvent(pViewer, XK_Alt_L, FALSE);
 *      VNCViewerSendKeyEvent(pViewer, XK_Control_L, FALSE);
 *      \endcode
 *      and then hook it to an application menu or hotkey (possibly by using
 *      VNCViewerPreKeyEventCallback).
 *   -# Document that the user should press Shift-Ctrl-Alt-Del when they want
 *      to do this.  Windows will not interpret this as the Secure Attention
 *      Sequence, but will pass the keys to the application as it would
 *      normally.  When the keys are then sent to the VNC server, however, the
 *      server will simulate a server-side Secure Attention Sequence, as if the
 *      user had pressed only Ctrl-Alt-Del locally.
 *
 * \section section_advanced Advanced features
 *
 * \subsection subsection_usage_messages Protocol extensions
 *
 * The SDK provides the ability for applications to define their own protocol 
 * extensions.  These allow the application to exchange short messages with the
 * VNC server to which it is connected.  
 *
 * Protocol extensions are supported only for VNC sessions using RFB 4.0 or
 * later.
 *
 * To configure the SDK to use a protocol extension, you must provide a
 * VNCViewerProtocolExtensionMessageCallback when you call VNCViewerCreate(),
 * and then call VNCViewerRegisterProtocolExtension() in or after the
 * VNCViewerServerInitCallback.
 *
 * To send a protocol extension message, call VNCViewerSendExtensionMessage().
 *
 * Your VNCViewerProtocolExtensionMessageCallback will be invoked whenever a
 * protocol extension message is received for an extension that you have
 * registered.
 *
 * \subsection subsection_usage_decoders Custom rectangle decoders
 *
 * A VNC server send sends screen updates to the VNC viewer in Rectangle (RFB
 * 4) or FrameBufferUpdate (RFB 3) protocol messages.  The payloads of these
 * messages contain pixel data that has been encoded in a variety of ways that
 * are designed to reduce the required bandwidth.  (You can think of an RFB
 * encoding as being a compression algorithm for video data.)
 *
 * The SDK provides the ability for applications to replace or extend the set
 * of encodings that it uses.  This allows for an application to compensate for
 * a bugged server-side encoder, or to add support for entirely new encodings.
 *
 * To configure the SDK to use a custom decoder, you must provide a
 * VNCViewerRectangleDecodeCallback when you call VNCViewerCreate(), and then,
 * before calling VNCViewerStart(), call VNCViewerRegisterRectangleDecoder()
 * for each encoding that the callback will handle.
 *
 * For for information on the built-in RFB encodings, see the RFB 4.0
 * Specification or the RFB 3.8 Specification.
 *
 * \subsection subsection_usage_transports Custom Transports
 * 
 * Internally, the SDK uses TCP socket connections to transport RFB data. 
 * A set of callbacks allow the replacement of the internal default transport
 * layer with any other. 
 * 
 * To configure the SDK to use a custom transport, you must provide all of the 
 * following callbacks when you call VNCViewerCreate():
 *    VNCViewerTransportOpenCallback, VNCViewerTransportCloseCallback, 
 *    VNCViewerTransportSendCallback, and VNCViewerTransportRecvCallback.
 * The open callback will be invoked at the beginning of a thread of execution,
 * i.e. shortly after VNCViewerStart() is called. The close callback will
 * be invoked upon closure of the connection, either as a result of 
 * a call to VNCViewerStop() or due to an external disconnection event.
 *
 * VNCViewerTransportOpenCallback() will be passed a pointer to the Context data
 * set by calls to VNCViewerSetContext; this can be used to identify 
 * the server endpoint, (for example, a name and port number could be stored
 * in the Context data for retrieval by the open callback).
 * If desired, the same Context data can be used to identify the connection to 
 * all subsequent Send, Recv and Close callbacks in a similar manner.
 * VNCViewerTransportOpenCallback implementations should return appropriate 
 * VNCViewerTransportResult values for the type of transport implementated, but 
 * at a minimum the open callback must support returning 
 * VNCViewerTransportResultSuccess and VNCViewerTransportResultFailure.
 * 
 *
 * VNCViewerTransportSendCallback and VNCViewerTransportRecvCallback implemenations
 * should return VNCViewerTransportResultFailure or VNCViewerTransportResultSuccess 
 * as appropriate, aside from cases where the read or write attempt would block; the 
 * function should then return VNCViewerTransportResultWouldBlock, with a length 
 * value of 0 in the case of a blocked read. In order to indicate to the SDK when an 
 * external read, write or close event has occurred/is ready, the application should 
 * call the method VNCViewerTransportEvent with one of the VNCViewerTransportStatus 
 * values as appropriate.
 * 
 * \section section_notes Notes
 * 
 * When debugging the any SDK-based applications in a development environment such
 * as Visual Studio, the output window may report first-chance exceptions of the form:
 *
 *   First-chance exception at 0xXXXXXXX in vnccommand.exe: Microsoft C++ exception: 
 *   rdr::BlockException at memory location 0xXXXXXXX..
 *
 * This is a normal part of debugging, and indicates that an exceptional circumstance 
 * has been encountered. Only if it is not handled correctly by the SDK or application
 * will an exception become a second-chance exception and the application will stop 
 * or crash.
 *
 * Should you wish to switch off the recording of first-chance exceptions to the output 
 * panel you can right-click on the panel and de-select Exception Messages.
 * 
 * \section acknowledgements Acknowledgements
 *
 * Details of and copyright notices for third-party software that is used by
 * the RealVNC Viewer SDK can be found in the file vncacknowledgements.h.
 *
 * \see VNCViewerSDK, VNCViewerCallbacks, VNCViewerSDKInitialize,
 * VNCViewerSDKUninitialize, VNCViewerCreate, VNCViewerDestroy, VNCViewerStart,
 * VNCViewerStop
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief Create a new VNC viewer instance.
 *
 * This method creates and returns a new, uninitialized VNC viewer instance.
 * All of the viewer's parameters have their default values, and the viewer
 * will not attempt to connect to a server or create its desktop window until
 * you call VNCViewerStart().
 *
 * \param pCallbacks Pointer to a VNCViewerCallbacks structure holding pointers
 * to callbacks that may be invoked by the SDK.  The SDK copies the contents of
 * *pCallbacks, so it is safe to free this structure once VNCViewerCreate()
 * returns.  Passing pCallbacks equal to NULL is equivalent to passing
 * pCallbacks non-NULL with every pointer in *pCallbacks set to NULL.
 * \param callbacksSize The size of *pCallbacks.  If pCallbacks is non-NULL,
 * pass sizeof(VNCViewerCallbacks); if pCallbacks is NULL, pass 0.
 * \param createDesktopWindow If non-zero, the VNC viewer will create a desktop
 * window.  The VNC viewer renders the contents of the server desktop into this
 * window, and gathers keyboard and mouse input events from it so that they can
 * be sent to the VNC server.  If you prefer to render the server desktop and
 * provide input events yourself, pass zero.
 * \return The new VNC viewer.  When you are finished with it, call
 * VNCViewerDestroy() to destroy it.  If a VNC viewer could not be created,
 * this method returns NULL.
 *
 * \note IMPORTANT.  This version of the SDK is limited in that only one VNC
 * viewer instance may safely exist in each process at any time.  Do not
 * attempt to create a second VNC viewer in the same process without previously
 * having destroyed the first.  (You may, however, start and stop a single VNC
 * viewer multiple times, for example to attempt to reconnect to the same
 * server.)
 *
 * \see VNCViewerDestroy, VNCViewerCallbacks
 */
typedef VNCViewer *VNCCALL
VNCViewerCreate(VNCViewerCallbacks *pCallbacks,
                size_t callbacksSize,
                vnc_int32_t createDesktopWindow);

/**
 * \brief Destroy an instance of the VNC viewer.
 *
 * This method destroys a VNC viewer instance.  If the VNC viewer is still
 * running, it will be stopped first.  After calling this method, you should
 * not attempt to interact further with the VNC viewer.
 *
 * \param pViewer The VNC viewer that should be destroyed.
 *
 * \see VNCViewerCreate
 */ 
typedef void VNCCALL
VNCViewerDestroy(VNCViewer *pViewer);

/**
 * \brief Begin running a VNC viewer.
 *
 * This method creates and start a new thread in which to run the VNC viewer.
 * In that thread, the VNC viewer will attempt to connect to the VNC server,
 * and, if successful, start displaying its desktop window.  You will be
 * notified of the outcome of the connection attempt via the
 * VNCViewerStatusCallback.
 *
 * \param pViewer The VNC viewer that should be started.
 * \param serverName The name of the VNC server to which the connection should
 * be made.  A VNC server name has one of the following formats:
 *  - hostname:port - where port is a TCP port number greater that or equal to
 *    100.
 *  - hostname - equivalent to hostname:5900.
 *  - hostname::port - equivalent to hostname:port.
 *  - hostname:display - where display is a number between 0 and 99 inclusive.
 *    This is equivalent to hostname:(display + 5900).
 * Instead of a hostname, you may use an IPv4 address, or an IPv6 address in
 * square brackets (in the same way as specified in RFC 2732).
 * If/when using a replacement transport, the serverName argument is used only
 * for informational purposes and can be ommitted (i.e.: set as the null string "")
 * if desired. Replacement transports should use the user-defined context data to 
 * identify connection endpoints if necessary.
 *
 * \note Prior to calling VNCViewerStart(), you should call
 * VNCViewerSetParameter() as necessary to provide the VNC viewer with its
 * initial configuration.
 * \note Calling this method when the VNC viewer is already running has no
 * effect.
 *
 * \see VNCViewerStatusCallback, VNCViewerStop
 */
typedef void VNCCALL
VNCViewerStart(VNCViewer *pViewer, const char *serverName);

/**
 * \brief Stop a running VNC viewer.
 *
 * If the VNC viewer is running, this method will instruct the VNC viewer to
 * close its connection to the close its desktop window (if there is one), and
 * then stop and destroy the thread in which is was running.
 *
 * Immediately before the viewer thread terminates, it will invoke your
 * VNCViewerStatusCallback with VNCViewerStatusStopped.
 *
 * Calling VNCViewerStop() does not destroy the VNC viewer object itself.
 * You may either call VNCViewerDestroy() to destroy it, or, once it has
 * finished stopping, call VNCViewerStart() to start it again.
 *
 * If the VNC viewer is not running, this method has no effect.
 *
 * \param pViewer The VNC viewer that should be stopped.
 *
 * \see VNCViewerStart
 */
typedef void VNCCALL
VNCViewerStop(VNCViewer *pViewer);

/**
 * \brief Queries a VNC viewer for the value of a runtime property.
 *
 * You can use the method to find out information about a running VNC viewer.
 * For example, if you have disabled the VNC viewer's desktop window and are
 * drawing the server desktop yourself, you can use this method to obtain the
 * server desktop's width and height.
 *
 * \param pViewer The VNC viewer that should be queried.
 * \param property The property to query, such as server desktop width or
 * height.
 *
 * \return Returns the value of the property.
 *
 * \see VNCViewerProperty
 */
typedef vnc_int32_t VNCCALL
VNCViewerGetProperty(VNCViewer *pViewer, VNCViewerProperty property);

/**
 * \brief Queries the value of a VNC viewer parameter.
 *
 * The configuration of a VNC viewer is stored internally as a list of
 * name-value pairs.  This method queries the value of a VNC viewer parameter
 * and returns it.  The parameter names are case-insensitive.
 *
 * \param pViewer The VNC viewer whose parameter to query.
 * \param parameter The name of the VNC viewer parameter to set as a UTF-8
 * encoded, NUL-terminated string.  See vncparameters.h for an explanation of
 * the supported parameters.
 *
 * \return Returns a copy of the parameter's value as a UTF-8 encoded,
 * NUL-terminated string.  When you are finished with the copied value, you
 * must free it by calling VNCFreeString().  Returns NULL if the parameter name
 * is not valid or if memory could not be allocated.
 *
 * \note The values of parameters whose values are integers are formatted into
 * strings of decimal digits.
 *
 * \see VNCViewerSetParameter, vncparameters.h
 */
typedef char *VNCCALL
VNCViewerGetParameter(VNCViewer *pViewer, const char *parameter);

/**
 * Sets the value of a VNC viewer parameter.
 *
 * If the viewer is running, then the new value will take effect
 * immediately, provided that this makes sense.  For example, changes to a
 * running viewer's "Scaling" and "FullColor" parameters will take effect
 * immediately, because they affect the desktop window.  However, it is
 * meaningless to change a running viewer's "ProxyServer" parameter, because
 * the connection to the VNC server has already been made.
 *
 * \param pViewer The VNC viewer whose parameter to set.
 * \param parameter The name of the VNC viewer parameter to set as a UTF-8
 * encoded, NUL-terminated string.  See vncparameters.h for an explanation of
 * the supported parameters.
 * \param value The new parameter value as a UTF-8 encoded, NUL-terminated
 * string.  You may free this string after VNCViewerSetParameter() has
 * returned.
 *
 * \return Returns zero if the parameter was set successfully.  Returns
 * non-zero otherwise (this usually indicates that the name of the parameter is 
 * not recognized).
 *
 * \note VNC viewer parameter names are case-insensitive.
 * \note Parameters whose values are integers are formatted in decimal.
 * \note VNC viewer parameters may never have NULL values.  To 'clear' the
 * value of a string parameter, pass an empty string as the value.
 *
 * \see VNCViewerGetParameter, vncparameters.h
 */
typedef vnc_int32_t VNCCALL
VNCViewerSetParameter(VNCViewer *pViewer,
                      const char *parameter,
                      const char *value);

/**
 * \brief Iterates the names of the supported VNC viewer parameters.
 *
 * \param pViewer The VNC viewer whose parameter names to iterate.
 * \param pContext A structure that holds state between calls to 
 * VNCViewerIterateParameterNames().  Before the first call to 
 * VNCViewerIterateParameterNames(), allocate and zero the structure.  To
 * retrieve subsequent parameter names, call VNCViewerIterateParameterNames()
 * again without modifying the structure.
 * \param contextSize The size of the VNCParameterIterationContext structure.
 *
 * \return Returns the name of next parameter.  This string is owned by the
 * SDK; do not free it.  Return NULL if there are no more parameters to
 * iterate.
 *
 * \see VNCParameterIterationContext, VNCViewerGetParameter
 */
typedef const char *VNCCALL
VNCViewerIterateParameterNames(VNCViewer *pViewer,
                               VNCParameterIterationContext *pContext,
                               size_t contextSize);

/**
 * \brief Associates a context pointer with a VNC viewer.
 *
 * Use this method to associate data with a VNC viewer instance so that you can
 * recover it at a later time, e.g. in one of the SDK callback functions.  The
 * context pointer can be retrieved by calling VNCViewerGetContext().
 *
 * \note A second call to VNCViewerSetContext() replaces the context pointer
 * assigned by the first call.
 * \note The SDK will not interact in any way with the memory to which the
 * context pointer points.  In particular, the SDK will not free the memory
 * when the VNC viewer is destroyed.
 *
 * \see VNCViewerGetContext
 */
typedef void VNCCALL
VNCViewerSetContext(VNCViewer *pViewer, void *pContext);

/**
 * \brief Retrieves the VNC viewer's context pointer.
 *
 * This method returns the context pointer associated with the VNC viewer by a
 * call to VNCViewerSetContext(). 
 *
 * \return Returns the VNC viewer's context pointer, or NULL if there is none.
 *
 * \see VNCViewerGetContext
 */
typedef void *VNCCALL
VNCViewerGetContext(VNCViewer *pViewer);

/**
 * \brief Enables or disables display updates.
 *
 * This method can be used to prevent the VNC server from sending further
 * display updates to the given VNC viewer object.  This is useful when it is
 * temporarily unnecessary to continue to update the VNC viewer's display in
 * real time (for example, because it is obscured) because it prevents the VNC
 * Server from continuing to do unnecessary work.
 *
 * Re-enabling display updates will cause the VNC server to refresh the entire
 * display.  The entire display will be refreshed whenever
 * VNCViewerEnableDisplayUpdates(TRUE) is called, even if display updates were
 * never disabled.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param enable If non-zero, (re-)enable updates and refresh the entire
 * display; if zero, disable updates.
 *
 * \note Calls to the display update callbacks may continue for a few moments
 * after VNCViewerEnableDisplayUpdates(FALSE) is called.  This is because the
 * server was in the middle of an update.  Further updates will be disabled
 * once the current update has finished.
 * 
 * \see VNCViewerPropertyDisplayUpdatesEnabled
 */
typedef void VNCCALL
VNCViewerEnableDisplayUpdates(VNCViewer *pViewer,
                              vnc_uint8_t enable);

/**
 * \brief Requests that a VNC session uses a particular pixel format.
 *
 * This method causes a SetPixelFormat RFB message to be sent to a VNC server,
 * followed by a FramebufferUpdateRequest for the entire server desktop.  The
 * VNC server will use the given pixel format for all future
 * FramebufferUpdates, until the next call to VNCViewerSetPixelFormat().
 *
 * Unless told otherwise, a VNC server will send FramebufferUpdates in its
 * native pixel format.
 *
 * VNCViewerSetPixelFormat() overrides the AutoSelect parameter.  If you wish
 * to re-enable AutoSelect, you must first call VNCViewerSetPixelFormat() with
 * the pPixelFormat equal to NULL and then set the AutoSelect parameter to
 * true.
 *
 * \param pViewer The VNC viewer whose pixel format to set.
 * \param pPixelFormat The pixel format to request.  This data is copied; it is
 * safe to free this structure after VNCViewerSetPixelFormat() returns.
 *
 * \see VNCPixelFormat, VNCParameterAutoSelect
 */
typedef void VNCCALL
VNCViewerSetPixelFormat(VNCViewer *pViewer,
                        const VNCPixelFormat *pPixelFormat);

/**
 * \brief Causes a KeyEvent RFB message to be sent to the VNC server.
 *
 * If you are not using the default desktop window, you should call this method
 * whenever the user presses or releases a key, so that the key event can be
 * reflected by the VNC server.
 *
 * \param pViewer The VNC viewer that should send the KeyEvent.
 * \param keyDown If non-zero, send a key-down event.  If zero, send a key-up
 * event.
 * \param keySymbol The X key symbol (e.g. XK_Tab) representing the key that
 * has been pressed or released.  These constants are defined in keysymdef.h.
 * To translate from a platform-native key symbol constant or a Unicode
 * character, call VNCSpecialKeyToXKeySymbol() or VNCUnicodeToXKeySymbol().
 * You should use VNCSpecialKeyToXKeySymbol() for keys that do no generate
 * characters (e.g. Shift) and VNCUnicodeToXKeySymbol() for keys that have
 * generated characters (your application must be able to differentiate between
 * such keypresses).
 *
 * \note Remember to send key up events that exactly match the key down events
 * that you have sent.  Failing to do so will cause the VNC server to act as
 * though the key is being held down.
 *
 * \note Not all platforms generate key up events for keys that were pressed
 * when the application loses the input focus.  However, your application
 * should still send RFB key up events for these keys.
 *
 * \see VNCViewerPreKeyEventCallback, VNCSpecialKeyToXKeySymbol,
 * VNCUnicodeToXKeySymbol, keysymdef.h
 */
typedef void VNCCALL
VNCViewerSendKeyEvent(VNCViewer *pViewer,
                      vnc_uint8_t keyDown,
                      vnc_int32_t keySymbol);

/**
 * \brief Causes a USB Scancode to be sent to the VNC server, if the server supports
 * the scancode extension.
 * 
 * \param keydown
 * \param usbScancode
 *
 * \return Returns true if the scancode has been sent via the extension, false if not.
 */
typedef vnc_uint8_t VNCCALL
VNCViewerSendUSBKeyEvent(VNCViewer *pViewer,
                         vnc_uint8_t keydown,
                         vnc_uint32_t usbScancode);

/**
 * \brief Causes a PointerEvent RFB message to be sent to the VNC server.
 *
 * If you are not using the default desktop window, you should call this method
 * whenever the user moves the mouse pointer or changes its button state, so
 * that the pointer event can be reflected by the VNC server.
 *
 * \param pViewer The VNC viewer that should send the PointerEvent.
 * \param buttonMask A bit-mask representing the pointer device buttons that
 * are currently depressed.  Use the VNCPointerDeviceButton enumeration to set
 * the bits in buttonMask.
 * \param x The x co-ordinate of the pointer on the server desktop. 
 * \param y The y co-ordinate of the pointer on the server desktop.
 *
 * \note If RelativePtr is enabled both x and y co-ordinates will be treated as 
 * relative motion values.
 *
 * \see VNCViewerPrePointerEventCallback, VNCPointerDeviceButton
 */
typedef void VNCCALL
VNCViewerSendPointerEvent(VNCViewer *pViewer,
                          VNCPointerDeviceButton buttonMask,
                          vnc_uint16_t x,
                          vnc_uint16_t y);

/**
 * \brief Causes a ClientCutText RFB message to be sent to the VNC server.
 *
 * If your application is running on a platform that supports a clipboard, and
 * you are able to observe the clipboard's contents, then you may wish to call
 * this method when the clipboard has new text copied to it.  This will cause
 * the server desktop's clipboard contents to be updated to match the new text,
 * allowing the user to copy text from a local application to the server
 * desktop.
 *
 * If you are using the default desktop window, the VNC server's clipboard
 * contents are updated automatically, so there is no need to implement this
 * callback.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param clipboardText The text to be copied to the server desktop's
 * clipboard.  For compatibility with existing VNC servers, the encoding of the
 * the text should be ISO-8859-1 (Latin-1).  clipboardText need not be
 * NUL-terminated.
 * \param clipboardTextLength The length of clipboardText.
 *
 * \see VNCViewerServerCutTextCallback
 */
typedef void VNCCALL
VNCViewerSendClientCutText(VNCViewer *pViewer,
                           const char *clipboardText,
                           size_t clipboardTextLength);


/**
 * \brief Causes the default desktop to rotate to a new orientation, and if supplied
 * triggers the application's VNCViewerRotationCallback.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param rotation The angle to which the desktop should be rotated
 *
 * \see VNCViewerRotationCallback, VNCRotationSetting
 */
typedef vnc_uint8_t VNCCALL
VNCViewerRotateDesktop(VNCViewer *pViewer,
                       VNCRotationSetting rotation); 

/**
 * \brief Registers a new protocol extension in the viewer and notifies the
 * server of the new extension.
 *
 * Calling this method allows you to call VNCViewerSendExtensionMessage() for
 * the registered extension, and also instructs the SDK to call your
 * VNCViewerProtocolExtensionMessageCallback for any extension messages that it
 * receives with this extension name.
 *
 * This method may be called at any time after or during the
 * VNCViewerServerInitCallback.
 *
 * Extension messages are supported only if the session is using RFB 4 or
 * later.  You can inspect the protocol version for a session by calling
 * VNCViewerGetProperty(VNCViewerPropertyRFBVersionMajor).
 *
 * \param pViewer The VNC viewer to register the extension with.
 * \param name The extension name, which should be a NUL-terminated ASCII
 * string.  This name must not clash with any existing RFB 4 extension names.
 *
 * \return A pointer that may be used in future calls to extension-related
 * APIs, or NULL if registration failed.
 *
 * \see VNCViewerUnregisterProtocolExtension,
 * VNCViewerGetProtocolExtensionByName, VNCViewerSendExtensionMessage
 */
typedef VNCProtocolExtension *VNCCALL
VNCViewerRegisterProtocolExtension(VNCViewer *pViewer,
                                   const char *name);

/**
 * \brief Unregisters a previously registered protocol extension.
 *
 * Unregistering an extension prevents the SDK from calling your
 * VNCViewerProtocolExtensionMessageCallback for any extension messages that it
 * receives for this extension.
 *
 * This method may be called at any time after or during the
 * VNCViewerServerInitCallback.
 *
 * \param pViewer The VNC viewer with which the extension was registered.
 * \param extension The extension.  This must be a pointer returned by
 * VNCViewerRegisterProtocolExtension() or
 * VNCViewerGetProtocolExtensionByName().
 *
 * \see VNCViewerRegisterProtocolExtension,
 * VNCViewerGetProtocolExtensionByName, VNCViewerSendExtensionMessage
 */
typedef void VNCCALL
VNCViewerUnregisterProtocolExtension(VNCViewer *pViewer,
                                     VNCProtocolExtension *extension);

/**
 * \brief Returns a VNCProtocolExtension pointer for a previously-registered
 * extension.
 *
 * \param pViewer The VNC viewer with which the extension was registered.
 * \param name The extension name, which should be a NUL-terminated ASCII
 * string.
 *
 * \return The VNCProtocolExtension pointer, or NULL if no extension with this
 * name was registered.
 *
 * \see VNCViewerRegisterProtocolExtension,
 * VNCViewerUnregisterProtocolExtension, VNCViewerSendExtensionMessage
 */
typedef VNCProtocolExtension *VNCCALL
VNCViewerGetProtocolExtensionByName(VNCViewer *pViewer,
                                    const char *name);

/**
 * \brief Sends a protocol extension message.
 *
 * Extension messages are sent to and received from VNC servers as RFB 4
 * ExtensionMsg messages.  The ExtensionMsg message body contains the length of
 * the extension message data in the first byte, followed by the message data
 * itself.
 *
 * \param pViewer The VNC viewer with which to send the message.
 * \param extension The extension on which to to send the message.
 * \param data The message data.  This data is is treated as opaque by the SDK
 * and may take any any format.
 * \param length The length of the message in bytes.  This may not be greater
 * than 255 (the SDK will not send any part of the message if length is
 * greater than 255).
 *
 * \see VNCViewerRegisterProtocolExtension,
 * VNCViewerGetProtocolExtensionByName,
 * VNCViewerProtocolExtensionMessageCallback
 */
typedef void VNCCALL
VNCViewerSendExtensionMessage(VNCViewer *pViewer,
                              VNCProtocolExtension *extension,
                              const void *data,
                              size_t length);

/**
 * \brief Notifies that the SDK that the VNCViewerRectangleDecodeCallback
 * should be invoked whenever rectangles are received with a specific RFB
 * encoding.
 *
 * You must make all calls to VNCViewerRegisterRectangleDecoder() before
 * starting the viewer with VNCViewerStart().  Calling
 * VNCViewerRegisterRectangleDecoder() after calling VNCViewerStart() will
 * result in undefined behavior.
 *
 * \param pViewer The VNC viewer for which the callback should be called.
 * \param encoding The encoding for which the callback should be called.  If
 * this is one of the standard RFB encoding numbers (i.e. a member of the
 * VNCEncoding enumeration), then the callback will be used in preference to
 * the built-in decoder for that encoding.  Otherwise, encoding should be a
 * number within the range 1024 to 2047 inclusive.  This range is reserved for
 * SDK customers.  The sub-range 1024 to 1039 inclusive (16 encoding numbers)
 * is reserved for Intel.  Using a non-standard encoding with the SDK will
 * require you also to implement a VNC server that is capable of sending
 * updates in that encoding.
 *
 * \see VNCViewerRectangleDecodeCallback, VNCEncoding
 */
typedef void VNCCALL
VNCViewerRegisterRectangleDecoder(VNCViewer *pViewer,
                                  vnc_uint32_t encoding);

/**
 * \brief Generates an RSA public and private key pair for use by a VNC viewer.
 *
 * Generating an RSA key pair and then assigning it to a VNC viewer's
 * RSA_Private_Key parameter allows it to use VNC authentication schemes that
 * require RSA keys.  (If the VNC server is known not to require
 * authentication, there is no need to perform this step.)
 *
 * Generating an RSA key pair is a CPU-intensive operation and may take a
 * minute or two.  You may wish to perform this operation in a background
 * thread to prevent your UI from becoming unresponsive, and to warn the user
 * that it may take a while.
 *
 * However, it is not necessary to generate a new RSA key pair for every
 * session.  You can perform this step during initial application setup or at
 * first use, and then securely store the key pair for future use.
 *
 * \return Returns the new RSA key pair, formatted as a hexadecimal string.
 * When you are finished with this string, call VNCFreeString(), which will
 * zero the memory for you.  Returns NULL if an RSA key pair cannot be
 * generated.
 *
 * \note This version of the SDK does not support session encryption.  RSA key
 * pairs are used solely for session authentication.
 *
 * \note The private key component of the returned string is not encrypted.  If
 * you store it persistently (e.g. on disk or in the Windows registry), take
 * care to encrypt it first.
 *
 * \see VNCParameterRSA_Private_Key
 */
typedef char *VNCCALL
VNCGenerateRSAKeys(void);

/**
 * \brief Allocates a string for use by the SDK.
 *
 * Use this method to allocate strings whose ownership must be passed to the
 * SDK.
 *
 * \param string A string to copy.  See the note below.
 * \param length The length of the string to allocate.  See the note below.
 *
 * \return Returns a new string.  The returned string must be freed by calling
 * VNCFreeString().
 *
 * \note There are three ways to use this function:
 *
 *  - string != NULL, length == (size_t) -1 - copies an existing NUL-terminated
 *    string.
 *  - string != NULL, length >= 0 - copies an existing string of that length
 *    and places a NUL-terminator afterwards.
 *  - string == NULL, length >= 0 - allocates a buffer that can hold a string
 *    of that length and also a NUL-terminator (i.e. it allocates (length + 1)
 *    bytes).  The entire buffer is initialized with NULs.
 */
typedef char *VNCCALL
VNCAllocString(const char *string, size_t length);

/**
 * \brief Frees a string that has been allocated by the SDK or by calling
 * VNCAllocString().
 *
 * Use this method to free any strings that allocated in this way but are no
 * longer required.  The memory is zeroed prior to being freed.
 *
 * \param string The string to free.
 */
typedef void VNCCALL
VNCFreeString(char *string);

/**
 * \brief Translates a platform-specific special key symbol into an X key
 * symbol.
 *
 * Special keys are keys that do not produce characters and are not dead keys.
 * These include Shift, Control, IME mode change keys, and so on.
 *
 * On Windows, the platform-specific key symbols are the virtual key (VK_*)
 * constants defined in winuser.h (or, equivalently, members of the .Net
 * Framework's System.Windows.Forms.Keys enumeration).  On Linux,
 * the platform-specific key symbols are the X key symbols, and this function
 * is a no-op.
 *
 * \param platformSpecificKeySymbol The platform-specific key symbol.
 *
 * \return The X key symbol corresponding to platformSpecificKeySymbol, or
 * (vnc_uint32_t) -1 if this special key is not supported by VNC.
 *
 * \see VNCViewerSendKeyEvent, VNCViewerPreKeyEventCallback, keysymdef.h
 */
typedef vnc_uint32_t VNCCALL
VNCSpecialKeyToXKeySymbol(vnc_uint32_t platformSpecificKeySymbol);

/**
 * \brief Translates an X key symbol into a platform-specific special key
 * symbol.
 *
 * Special keys are keys that do not produce characters and are not dead keys.
 * These include Shift, Control, IME mode change keys, and so on.
 *
 * On Windows, the platform-specific key symbols are the virtual key (VK_*)
 * constants defined in winuser.h (or, equivalently, members of the .Net
 * Framework's System.Windows.Forms.Keys enumeration).  On Linux,
 * the platform-specific key symbols are the X key symbols, and this function
 * is a no-op.
 *
 * \param keySymbol An X key symbol.
 *
 * \return The platform-specific special key symbol corresponding to keySymbol,
 * or (vnc_uint32_t) -1 if this X key symbol is not supported by VNC on this
 * platform.
 *
 * \see VNCViewerSendKeyEvent, VNCViewerPreKeyEventCallback, keysymdef.h
 */
typedef vnc_uint32_t VNCCALL
VNCXKeySymbolToSpecialKey(vnc_uint32_t keySymbol);

/**
 * \brief Translates a Unicode character into an X key symbol.
 *
 * \param unicodeCharacter A Unicode character in UCS-2 encoding.  (Characters
 * that cannot be encoded by UCS-2 are not supported by this API.)
 *
 * \return The X key symbol corresponding to unicodeCharacter, or
 * (vnc_uint32_t) -1 if there is none.
 *
 * \see VNCViewerSendKeyEvent, VNCViewerPreKeyEventCallback, keysymdef.h
 */
typedef vnc_uint32_t VNCCALL
VNCUnicodeToXKeySymbol(vnc_uint32_t unicodeCharacter);

/**
 * \brief Translates an X key symbol into a Unicode character.
 *
 * \param keySymbol An X key symbol.
 *
 * \return The Unicode character, in UCS-2 encoding, corresponding to
 * keySymbol, or (vnc_uint32_t) -1 if there is none or if the character cannot
 * be represented in UCS-2.
 *
 * \see VNCViewerSendKeyEvent, VNCViewerPreKeyEventCallback, keysymdef.h
 */
typedef vnc_uint32_t VNCCALL
VNCXKeySymbolToUnicode(vnc_uint32_t keySymbol);

/**
 * \brief Translates a WM Event keyboard input into a USB scancode.
 * 
 * \param keySymbol The WM Key event lParam associated with an input event.
 *
 * \return The USB scancode corresponding to keySymbol or -1 if there is none.
 * 
 * \see VNCViewerSendUSBKeyEvent
 */
typedef vnc_uint32_t VNCCALL
VNCWMKeyToUSBScancode(vnc_uint32_t keySymbol);

/**
 * \brief Notifies the SDK of a status change in the replacement transport
 *
 * \param pViewer The VNC viewer for which the transport status event is 
 * relevant.
 * \param status A VNCViewerTransportStatus value
 *
 * \see VNCViewerTransportStatus
 */
typedef void VNCCALL
VNCViewerTransportEvent(VNCViewer *pViewer, VNCViewerTransportStatus status);

/**
 * \brief Sets the license key string for this session
 *
 * \param pViewer The VNC viewer for which the license string is valid.
 * \param licenseString A null terminated license string
 */
typedef void VNCCALL 
VNCViewerSetLicense(VNCViewer *pViewer, const char *licenseString, size_t length);

/**
 * \brief Uninitializes the SDK.
 *
 * You must call this function after destroying all VNC viewers but before
 * unloading the SDK.
 *
 * \see VNCViewerSDKInitialize, VNCViewerDestroy
 */
typedef void VNCCALL
VNCViewerSDKUninitialize(void);

/**
 * \brief Contains the SDK's version and the addresses of the SDK's methods.
 *
 * An instance of this structure should be passed to VNCViewerSDKInitialize().
 * A successful call to VNCViewerSDKInitialize() fills in the structure with
 * the SDK's major and minor version numbers and the addresses of the SDK's VNC
 * viewer methods.
 *
 * After the structure is initialized, you may create a VNC viewer and interact
 * with it by calling the SDK's methods through the function pointers in the
 * structure.  Begin by calling VNCViewerCreate().
 *
 * You may also wish to check the version numbers reported by the SDK.  These
 * numbers are the version of the SDK itself, not the version of the RealVNC
 * software on which it was based or the version of the RFB protocol.
 *
 * For full documentation on each of the function pointers in the VNCViewerSDK
 * structure, see the documentation for the corresponding typedef.
 */
typedef struct
{
    /** Major component of the SDK's version number. */
    vnc_int32_t versionMajor;
    /** Minor component of the SDK's version number. */
    vnc_int32_t versionMinor;
    /** Patch component of the SDK's version number. */
    vnc_int32_t versionPatch;
    /** Build number component of the SDK's version number. */
    vnc_int32_t versionBuild;

    /** Uninitializes the SDK. */
    VNCViewerSDKUninitialize *vncViewerSDKUninitialize;
    /** Allocates a string for use by the SDK. */
    VNCAllocString *vncAllocString;
    /** Frees a string that was allocated with VNCAllocString. */
    VNCFreeString *vncFreeString;
    /** Generates an RSA public and private key pair for use by a VNC viewer. */
    VNCGenerateRSAKeys *vncGenerateRSAKeys;
    /** Creates a VNC viewer. */
    VNCViewerCreate *vncViewerCreate;
    /** Destroys a VNC viewer. */
    VNCViewerDestroy *vncViewerDestroy;
    /** Starts a VNC viewer. */
    VNCViewerStart *vncViewerStart;
    /** Stops a VNC viewer. */
    VNCViewerStop *vncViewerStop;
    /** Queries a VNC viewer for the value of a runtime property. */
    VNCViewerGetProperty *vncViewerGetProperty; 
    /** Queries a VNC viewer for the value of parameter. */
    VNCViewerGetParameter *vncViewerGetParameter; 
    /** Sets the value of a VNC viewer parameter. */
    VNCViewerSetParameter *vncViewerSetParameter; 
    /** Iterates the names of the supported VNC viewer parameters. */
    VNCViewerIterateParameterNames *vncViewerIterateParameterNames;
    /** Assigns a context data pointer to a VNC viewer. */
    VNCViewerSetContext *vncViewerSetContext; 
    /** Retrieves a context data pointer from a VNC viewer. */
    VNCViewerGetContext *vncViewerGetContext; 
    /** Requests that a VNC session uses a particular pixel format. */
    VNCViewerSetPixelFormat *vncViewerSetPixelFormat;
    /** Translates a platform-specific special key symbol into an X key symbol. */
    VNCSpecialKeyToXKeySymbol *vncSpecialKeyToXKeySymbol;
    /** Translates an X key symbol into a platform-specific special key symbol. */
    VNCXKeySymbolToSpecialKey *vncXKeySymbolToSpecialKey;
    /** Translates a Unicode character into an X key symbol. */
    VNCUnicodeToXKeySymbol *vncUnicodeToXKeySymbol;
    /** Translates an X key symbol into a Unicode character. */
    VNCXKeySymbolToUnicode *vncXKeySymbolToUnicode;
    /** Translates a Windows WM_KEYXXX message scancode into a USBScancode. */
    VNCWMKeyToUSBScancode *vncWMKeyToUSBScancode;
    /** Causes a KeyEvent RFB message to be sent to the VNC server. */
    VNCViewerSendKeyEvent *vncViewerSendKeyEvent;
    /** Causes a USB scancode to be sent to the VNC server via the scancode extension. */
    VNCViewerSendUSBKeyEvent *vncViewerSendUSBKeyEvent;
    /** Causes a PointerEvent RFB message to be sent to the VNC server. */
    VNCViewerSendPointerEvent *vncViewerSendPointerEvent;
    /** Enables or disables display updates. */
    VNCViewerEnableDisplayUpdates *vncViewerEnableDisplayUpdates;
    /** Register a new protocol extension with viewer and notify server. */
    VNCViewerRegisterProtocolExtension *vncViewerRegisterProtocolExtension;
    /** Unregister a a protocol extension from the viewer */
    VNCViewerUnregisterProtocolExtension *vncViewerUnregisterProtocolExtension;
    /** Get a VNCProtocolExtension by name. */
    VNCViewerGetProtocolExtensionByName *vncViewerGetProtocolExtensionByName;
    /** Send an extension message. */
    VNCViewerSendExtensionMessage *vncViewerSendExtensionMessage;
    /** Register a rectangle decoder. */
    VNCViewerRegisterRectangleDecoder *vncViewerRegisterRectangleDecoder;
    /** Causes a ClientCutText RFB message to be sent to the VNC server. */
    VNCViewerSendClientCutText *vncViewerSendClientCutText;
    /** Causes the desktop window to rotate the desktop, and invokes VNCViewerRotationCallback. */
    VNCViewerRotateDesktop *vncViewerRotateDesktop;
    /** Notifies the Viewer SDK of any changes in the replacement transport status. */
    VNCViewerTransportEvent *vncViewerTransportEvent;
    /** Allows the user to provide a license key to the SDK. */
    VNCViewerSetLicense *vncViewerSetLicense;
} VNCViewerSDK;

/**
 * \brief The type of VNCViewerSDKInitialize().
 *
 * If you are dynamically loading the SDK at runtime, you should use this
 * typedef to declare your function pointer.
 *
 * \see VNCViewerSDKInitialize
 */
typedef vnc_int32_t VNCCALL
VNCViewerSDKInitializeType(VNCViewerSDK *pSDK, size_t sdkSize);

/**
 * \brief The entry point to the SDK.
 *
 * This function is the sole entry point exported by the SDK.  Before you can
 * call any other of the SDK's functions, you must call this function to obtain
 * pointers to their implementations.
 *
 * Before unloading the SDK, you should call VNCViewerSDKUninitialize().
 *
 * \param pSDK Pointer to the VNCViewerSDK structure to be initialized.
 * \param sdkSize Size of the VNCViewerSDK structure to be initialized.
 *
 * \return Returns zero if *pSDK was successfully initialized, and non-zero
 * otherwise (e.g. because sdkSize is unacceptable).
 *
 * \see VNCViewerSDK, VNCViewerSDKUninitialize, VNCViewerSDKInitializeType
 */
VNCDLLIMPORT vnc_int32_t VNCCALL
VNCViewerSDKInitialize(VNCViewerSDK *pSDK, size_t sdkSize);

#ifdef __cplusplus
}
#endif

#endif /* !defined(__VNCAMT_H__) */
