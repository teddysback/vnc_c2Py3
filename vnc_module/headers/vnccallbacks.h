/* Copyright (C) 2002-2008 RealVNC Ltd.  All Rights Reserved.
*/

#ifndef __VNCCALLBACKS_H__
#define __VNCCALLBACKS_H__

/**
 * \file vnccallbacks.h
 *
 * This file defines types for pointers to callback functions that may be
 * provided to the RealVNC Viewer SDK for Intel AMT.  You should normally
 * include vncamt.h instead of including this file directly.
 *
 * \see VNCViewerCallbacks
 */

#include "vnctypes.h"

/**
 * \brief Called by the SDK to notify of a change in the VNC viewer's status.
 *
 * This callback is associated with the VNC viewer when you call
 * VNCViewerCreate().  When the connection is lost or some other error occurs,
 * the SDK invokes the callback to notify you of the reason.
 *
 * If you explicitly stop a running VNC viewer with VNCViewerStop(), then the
 * VNCViewerStatusCallback will be invoked with status VNCViewerStatusStopped.
 * This allows you to use the same cleanup code for this case as for error
 * cases.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param status The VNC viewer's status.
 * \param pMoreStatus Contains further information specific to the value of the
 * status parameter.  See below.
 *
 * \return The return value of this callback is unused by this version of this
 * SDK.  However, for future compatibility, you should return zero.
 *
 * \note This callback may be invoked by a thread other than the one that
 * created or started the VNC viewer.
 *
 * The value of pMoreStatus is specific to the value of the status parameter,
 * and is as follows:
 *
 * - If status is VNCViewerStatusAuthenticationFailure, pMoreStatus points to a
 *   NUL-terminated string, which will be the string received from the VNC
 *   server in the RFB SecurityResult message.
 * - If status is VNCViewerStatusConnectionClosed, pMoreStatus points to a
 *   VNCConnClose structure, whose contents are the RFB ConnClose message
 *   received from the VNC server.
 * - In all other cases, pMoreStatus is unused and will be NULL.
 *
 * In all cases where pMoreStatus is not NULL, it points to memory that is
 * owned by the SDK and should not be altered or freed.
 *
 * \see VNCViewerStatus, VNCConnClose, VNCViewerCreate, VNCViewerCallbacks,
 * VNCViewerSetContext
 */
typedef vnc_int32_t VNCCALL
VNCViewerStatusCallback(VNCViewer *pViewer,
                        void *pContext,
                        VNCViewerStatus status,
                        void *pMoreStatus);

/**
 * \brief Called by the SDK when the VNC server challenges the VNC viewer for a
 * username and password.
 * 
 * This callback is associated with the VNC viewer when you call
 * VNCViewerCreate(), and is invoked by the SDK when the VNC server challenges
 * the VNC viewer for a username and password.  This takes place after the
 * connection to the server is established but before the VNC protocol
 * 'handshake' is complete (and therefore before the desktop window is
 * displayed).
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param pUsername If pUsername is not NULL, then on successful return,
 * *pUsername must be a buffer allocated by calling VNCAllocString() that
 * contains the username for the session.  If pUsername is NULL, then the
 * security type selected for the session does not require a username.
 * \param pPassword If pPassword is not NULL, then on successful return,
 * *pPassword must be a buffer allocated by calling VNCAllocString() that
 * contains the password for the session.  If pPassword is NULL, then the
 * security type selected for the session does not require a password.
 *
 * \return Return zero if you were able to successfully fetch the username
 * and/or password and supply copies of them.  Otherwise, return non-zero.
 *
 * \note This callback may be invoked by a thread other than the one that
 * called VNCViewerCreate() or VNCViewerStart().
 *
 * The strings returned must be UTF-8 encoded and NUL-terminated string.  The
 * SDK will free them for you.  
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerSetContext
 */
typedef vnc_int32_t VNCCALL
VNCViewerCredentialsCallback(VNCViewer *pViewer,
                             void *pContext,
                             char **pUsername,
                             char **pPassword);

/**
 * \brief Called by the SDK to notify you of the server signature.
 *
 * A VNC server may have a signature, which is a sequence of random bytes that
 * remains constant throughout the server's lifetime.  Depending on the
 * negotiated security type, the server signature may be sent to the VNC viewer
 * during the RFB handshake.  This callback gives you the opportunity to
 * validate that the received server signature has the expected value for this
 * VNC server.
 *
 * If you do not provide a VNCViewerServerSignatureCallback, server signatures
 * will not be validated.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param serverName The name of the server.  This is the canonical version of
 * the name that was provided to VNCServerStart().  If you maintain a database
 * of previously-accepted server signatures, this should be the key.
 * \param fingerprint A prefix of a hash of the received server signature data.
 * This data is displayed in the UI of a standard RealVNC viewer or server.
 * You may prefer to display the fingerprint to the user instead of displaying
 * the entire signature.
 * \param fingerprintLength The length of the fingerprint data.
 * \param signature The received server signature data.
 * \param signatureLength The length of the received server signature data.
 *
 * \return Return non-zero to accept the server signature and proceed with the
 * connection.  Returing zero will cause the SDK to invoke the
 * VNCViewerStatusCallback with status VNCViewerStatusServerSignatureFailure
 * and then stop the VNC viewer thread.
 *
 * \note You may wish to maintain a database mapping server names to received
 * server signatures.  When connecting to a VNC server for the first time, you
 * can prompt the user to accept the signature and then record their decision.
 * Alternatively, if the signature of the VNC server is known at build-time,
 * you could choose to hard-code it.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerSetContext
 */
typedef vnc_int32_t VNCCALL
VNCViewerServerSignatureCallback(VNCViewer *pViewer,
                                 void *pContext,
                                 const char *serverName,
                                 const vnc_uint8_t *fingerprint,
                                 size_t fingerprintLength,
                                 const vnc_uint8_t *signature,
                                 size_t signatureLength);


/**
 * \brief Called by the SDK to inform you that the RFB session with the VNC
 * server has been completely established.
 *
 * This callback is invoked immediately after the VNC viewer displays its
 * default desktop window (if createDesktopWindow was non-zero in the call to
 * VNCViewerStart()).
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param width The initial width of the server desktop.
 * \param height The initial height of the server desktop.
 * \param desktopName The name of the server desktop, as a NUL-terminated UTF-8
 * encoded string.
 * \param pServerNativePixelFormat The native pixel format of the server.
 * Pixel data provided by the VNCViewerImageRectangleCallback and
 * VNCViewerFillRectangleCallback will use this pixel format until the next
 * call to VNCViewerPixelFormatChangedCallback.  You should copy the contents
 * of this structure for future use.
 *
 * \note If the VNC server's native pixel format uses a color map (i.e. 
 * pServerNativePixelFormat->trueColorFlag is zero), then the color map will be
 * provided by a future call to VNCViewerSetColorMapCallback.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerPixelFormat,
 * VNCViewerSetPixelFormat, VNCViewerPixelFormatChangedCallback
 */
typedef void VNCCALL
VNCViewerServerInitCallback(VNCViewer *pViewer,
                            void *pContext,
                            vnc_uint16_t width,
                            vnc_uint16_t height,
                            const char *desktopName,
                            const VNCPixelFormat *pServerNativePixelFormat);

/**
 * \brief Called by the SDK to notify you that future updates will use a new
 * pixel format.
 *
 * This callback is invoked when a pixel format that you selected with a
 * previous call to VNCViewerSetPixelFormat() has taken effect, and that all
 * future calls to VNCViewerImageRectangleCallback and
 * VNCViewerFillRectangleCallback will use the new pixel format.
 *
 * Until this callback is invoked, updates to VNCViewerImageRectangleCallback
 * and VNCViewerFillRectangleCallback will continue to use the existing pixel
 * format.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param pNewPixelFormat The new pixel format, which is one that you
 * previously chose with a call to VNCViewerSetPixelFormat.  You should copy
 * the contents of this structure for future use.
 */
typedef void VNCCALL
VNCViewerPixelFormatChangedCallback(VNCViewer *pViewer,
                                    void *pContext,
                                    const VNCPixelFormat *pNewPixelFormat);

/**
 * \brief Called by the SDK to notify you that the VNC server has chosen a new
 * color map.
 *
 * If the pixel format for the session changes to one that uses a color map
 * (i.e. trueColorFlag is zero), then the VNC server will choose a color map
 * for use with the session.  This callback notifies you of the color map
 * chosen by the server.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param colorMap The new color map that has been chosen by the server.  You
 * should copy whatever information you need from this array.
 * \param colorMapSize The number of elements in the colorMap array.  This will
 * be at most the depth of the pixel format.
 */
typedef void VNCCALL
VNCViewerColorMapChangedCallback(VNCViewer *pViewer,
                                 void *pContext,
                                 const VNCColor *colorMap,
                                 size_t colorMapSize);

/**
 * \brief Called by the SDK to notify you that a rectangle on the VNC server's
 * desktop has been updated with the given pixel data.
 *
 * The SDK calls a mixture of this function and VNCViewerFillRectangleCallback
 * for each rectangle received as part of an RFB FramebufferUpdate message,
 * provided that the encoding of the rectangle is not CopyRect.  For that
 * encoding, VNCViewerCopyRectangleCallback is called instead.
 *
 * The pixel format and optional color map used with the pixel data are the
 * last ones provided by VNCViewerServerInitCallback,
 * VNCViewerPixelFormatChangedCallback and VNCViewerColorMapChangedCallback.
 * For more information on the format of pixel data, see the documentation for
 * VNCPixelFormat.
 *
 * You will typically implement either all three of
 * VNCViewerImageRectangleCallback, VNCViewerCopyRectangleCallback and
 * VNCViewerFillRectangleCallback, or none of them.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param pRectangle The rectangle on the server desktop that has been updated.
 * \param pixelData The pixel data that has been received from the VNC server.
 * \param pixelDataLength The length of the pixel data that has been received
 * from the VNC server.  This is equal to
 * (area of *pRectangle * bitsPerPixel of the current pixel format / 8),
 * but it is provided here as a convenience.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCRectangle, VNCPixelFormat,
 * VNCViewerCopyRectangleCallback, VNCViewerFillRectangleCallback,
 * VNCViewerServerInitCallback, VNCViewerPixelFormatChangedCallback,
 * VNCViewerColorMapChangedCallback, VNCViewerSetPixelFormat
 */
typedef void VNCCALL
VNCViewerImageRectangleCallback(VNCViewer *pViewer,
                                void *pContext,
                                const VNCRectangle *pRectangle,
                                const vnc_uint8_t *pixelData,
                                size_t pixelDataLength);

/**
 * \brief Called by the SDK to notify you that a rectangle on the VNC server's
 * desktop has been copied from somewhere else.
 *
 * The SDK calls this function for each rectangle received as part of an RFB
 * FramebufferUpdate message that uses the CopyRect encoding.  For other
 * encodings, a mixture of VNCViewerImageRectangleCallback and
 * VNCViewerFillRectangleCallback is called instead.
 *
 * Note that the source and destination rectangles often overlap.  This is
 * because the RFB CopyRect encoding is usually used by a VNC server when it
 * detects that a window is being dragged around the display.  Your application
 * must be prepared to deal with this correctly.
 *
 * You will typically implement either all three of
 * VNCViewerImageRectangleCallback, VNCViewerCopyRectangleCallback and
 * VNCViewerFillRectangleCallback, or none of them.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param pDestination The rectangle on the VNC server desktop that is being
 * updated - that is, the destination of the copy.
 * \param pSource The top-left corner of the source of the copy.  The area to
 * be copied is the same as the area of *pDestination.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCRectangle, VNCPixelFormat,
 * VNCViewerImageRectangleCallback, VNCViewerFillRectangleCallback
 */
typedef void VNCCALL
VNCViewerCopyRectangleCallback(VNCViewer *pViewer,
                               void *pContext,
                               const VNCRectangle *pDestination,
                               const VNCPoint *pSource);

/**
 * \brief Called by the SDK to notify you that a rectangle on the VNC server's
 * desktop has been filled with the given pixel.
 *
 * The SDK calls this function for each rectangle that it determines must be
 * filled with a uniform color.  This can happen as a result of receiving a
 * Rectangle message in one of several RFB encodings.
 *
 * The pixel format and optional color map used with the pixel data are the
 * last ones provided by VNCViewerServerInitCallback,
 * VNCViewerPixelFormatChangedCallback and VNCViewerColorMapChangedCallback.
 * For more information on the format of pixel data, see the documentation for
 * VNCPixelFormat.
 *
 * You will typically implement either all three of
 * VNCViewerImageRectangleCallback, VNCViewerCopyRectangleCallback and
 * VNCViewerFillRectangleCallback, or none of them.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param pRectangle The rectangle on the VNC server desktop that must be
 * filled.
 * \param pixelData The pixel that must be copied to the entire rectangle.  If
 * the current pixel format uses a color map (i.e. its trueColorFlag is zero),
 * then this pixel value is an index into the color map.
 * \param pixelDataLength The length of the pixel data that has been received
 * from the VNC server.  There is only one pixel, so this is equal to
 * (area of *pRectangle * bitsPerPixel of the current pixel format / 8),
 * but it is provided here as a convenience.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCRectangle, VNCPixelFormat,
 * VNCViewerImageRectangleCallback, VNCViewerCopyRectangleCallback
 */
typedef void VNCCALL
VNCViewerFillRectangleCallback(VNCViewer *pViewer,
                               void *pContext,
                               const VNCRectangle *pRectangle,
                               const vnc_uint8_t *pixelData,
                               size_t pixelDataLength);

/**
 * \brief Called by the SDK to notify you that the size of the VNC server's
 * desktop has changed.
 *
 * When you receive this notification, you should allocate a new buffer to
 * store your local copy of the server desktop.  Before destroying the old
 * buffer, you must copy its contents into the top-left of the new buffer.
 * This is because the VNC server will not refresh the entire server desktop
 * following a resize, but will instead only refresh the parts that have
 * changed.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param width The new width of the server desktop.
 * \param height The new height of the server desktop.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks
 */
typedef void VNCCALL
VNCViewerDesktopResizeCallback(VNCViewer *pViewer,
                               void *pContext,
                               vnc_uint16_t width,
                               vnc_uint16_t height);

/**
 * \brief Called by the SDK to notify you that the default desktop window has
 * generated a key event.
 *
 * You can use this callback to prevent a key event from being sent to the
 * server, or as a trigger to do something else.  For example, you can use it
 * to implement an application-specific hot-key.
 *
 * You are permitted to call VNCViewerSendKeyEvent() from within your
 * VNCViewerPreKeyEventCallback.  However, be aware that such calls are queued
 * until after your VNCViewerPreKeyEventCallback returns and the current
 * KeyEvent has been fully processed.  Therefore, if the callback returns
 * non-zero, the current KeyEvent will arrive at the VNC server before the
 * events you queue with VNCViewerSendKeyEvent().  If this is not what you
 * want, re-queue the original event with a final call to
 * VNCViewerSendKeyEvent() and then return zero from the callback to prevent
 * the original event from being sent.
 *
 * VNCViewerPreKeyEventCallback is not invoked for KeyEvents that you generate
 * yourself by calling VNCViewerSendKeyEvent().
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param keyDown If non-zero, the event is a key-down event.  If zero, the
 * event is a key-up event.
 * \param keySymbol The X key symbol (e.g. XK_Tab) representing the key in the
 * event.  You may wish further to interpret this by first calling 
 * VNCXKeySymbolToSpecialKey() and then, if that fails, calling
 * VNCXKeySymbolToUnicode().
 *
 * \return Return non-zero to allow the KeyEvent to be sent to the VNC server.
 * Return zero to prevent it from being sent.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerSendKeyEvent,
 * VNCXKeySymbolToUnicode, VNCXKeySymbolToSpecialKey, keysymdef.h
 */
typedef vnc_int32_t VNCCALL
VNCViewerPreKeyEventCallback(VNCViewer *pViewer,
                             void *pContext,
                             vnc_uint8_t keyDown,
                             vnc_uint32_t keySymbol);

/**
 * \brief Called by the SDK to notify you that the SDK has generated a pointer
 * event.
 *
 * You can use this callback to prevent a pointer event from being sent to the
 * server, or as a trigger to do something else.  For example, you might decide
 * that the application will display a menu in response to a simultaneous left-
 * and right-click.
 *
 * VNCViewerPrePointerEventCallback is not invoked for PointerEvents that you
 * generate yourself by calling VNCViewerSendPointerEvent().
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param buttonMask A bit-mask representing the pointer device buttons that
 * are currently depressed.  Use the VNCPointerDeviceButton enumeration to set
 * or inspect the bits in buttonMask.
 * \param x The x co-ordinate of the pointer on the server desktop.
 * \param y The y co-ordinate of the pointer on the server desktop.
 *
 * \return Return non-zero to allow the PointerEvent to be sent to the VNC
 * server.  Return zero to prevent it from being sent.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerSendPointerEvent,
 * VNCPointerDeviceButton
 */
typedef vnc_int32_t VNCCALL
VNCViewerPrePointerEventCallback(VNCViewer *pViewer,
                                 void *pContext,
                                 VNCPointerDeviceButton buttonMask,
                                 vnc_uint16_t x,
                                 vnc_uint16_t y);

/**
 * \brief Called by the SDK to notify you that the VNC server's clipboard
 * contents have been updated.
 *
 * If you wish, you can update the local clipboard contents so that they match
 * the server's.  This allows the user to copy text from the server desktop to
 * a local application.
 *
 * If you are using the default desktop window, the local clipboard contents
 * are updated automatically, so there is no need to implement this callback.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param clipboardText The text that has been copied to the server desktop's
 * clipboard.  For compatibility with existing VNC servers, you should assume
 * that the encoding of the text is ISO-8859-1 (Latin-1).  clipboardText is not
 * NUL-terminated.
 * \param clipboardTextLength The length of clipboardText.
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerSendClientCutText
 */
typedef void VNCCALL
VNCViewerServerCutTextCallback(VNCViewer *pViewer,
                               void *pContext,
                               const char *clipboardText,
                               size_t clipboardTextLength);


/**
 * \brief Called by the SDK to notify you that the VNC server has requested 
 * a desktop rotation or VNCViewerSetDesktop has been called.
 *
 * If you are using the default desktop window, the rotation will be handled
 * automatically, so there is no need to implement this callback.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param rotation The clockwise rotation requested
 *
 * \see VNCViewerCreate, VNCViewerCallbacks, VNCViewerRotateDesktop, VNCRotationSetting
 */
typedef vnc_int32_t VNCCALL
VNCViewerRotationCallback(VNCViewer *pViewer,
                          void *pContext,
                          VNCRotationSetting rotation); 

/**
 * \brief Called by the SDK when a VNCProtocolExtension message is received.
 *
 * Extension messages are sent to and received from VNC servers as RFB 4
 * ExtensionMsg messages.  The ExtensionMsg message body contains the length of
 * the extension message data in the first byte, followed by the message data
 * itself.
 *
 * The length of an extension message may be at most 255 bytes.  The SDK will
 * discard any extension message longer than 255 bytes that is received.
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNC viewer by
 * VNCViewerSetContext().
 * \param data The message data.  This data is is treated as opaque by the SDK
 * and may take any any format.
 * \param length The length of the message in bytes.
 * \param extension The VNCProtocolExtension the message is coming from.
 * \param name The name of the VNCProtocolExtension.
 *
 * \see VNCViewerCallbacks, VNCViewerRegisterProtocolExtension,
 * VNCViewerGetProtocolExtensionByName, VNCViewerSendExtensionMessage
 */
typedef void VNCCALL
VNCViewerProtocolExtensionMessageCallback(VNCViewer *pViewer,
                                          void *pContext,
                                          const void *data,
                                          size_t length,
                                          const char *name);

/**
 * \brief Called by the SDK when an RFB 4.0 Rectangle message or an RFB 3.8
 * FrameBufferUpdate message is received, and the data is encoded using an
 * encoding registered with VNCViewerRegisterRectangleDecoder().
 *
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNCViewer.
 * \param encoding The rectangle encoding used for this data.  This will be an
 * encoding number that you previously registered with
 * VNCViewerRegisterRectangleDecoder().
 * \param width The rectangle width in pixels.
 * \param height The rectangle height in pixels.
 * \param bitsPerPixel Equal to the bitsPerPixel field of the current pixel
 * format.
 * \param encodedData The encoded rectangle data.
 * \param encodedDataLength The length in bytes of data pointed to by
 * encodedData (in bytes).  This is is _not_ guaranteed to be sufficient to
 * contain the entire rectangle (see below).
 * \param decodeBuffer The buffer into which the rectangle should be decoded.
 * The size of this buffer is (width * height * bitsPerPixel / 8) bytes.
 *
 * \return There are three return cases:
 *
 *  - > 0 - A rectangle has been successfully decoded, and its encoded length
 *    is given by the return value.  The SDK will consume this much data from
 *    its input stream and then process the decodeBuffer.
 *  - < 0 - The callback failed to decode a rectangle because
 *    encodedDataLength is too small.  However, the length of the complete
 *    encoded rectangle is known, and is given by (0 - the return value).  The
 *    SDK will read from the network until it has at least this much data and
 *    then then invoke the callback again.
 *  - == 0 - The callback failed to decode a rectangle because
 *    encodedDataLength is too small, and the length of the complete encoded
 *    rectangle is not known.  The SDK will read from the network again and
 *    then invoke the callback again.
 *
 * \see VNCViewerCallbacks, VNCViewerRegisterRectangleDecoder
 */
typedef vnc_int32_t VNCCALL
VNCViewerRectangleDecodeCallback(VNCViewer *pViewer,
                                 void *pContext,
                                 vnc_uint32_t encoding,
                                 vnc_uint16_t width,
                                 vnc_uint16_t height,
                                 vnc_uint8_t bitsPerPixel,
                                 const vnc_uint8_t *encodedData,
                                 size_t encodedDataLength,
                                 vnc_uint8_t *decodeBuffer);

/**
 * \brief Allows replacement of underlying transport, called when the 
 * SDK would normally open a TCP socket
 * 
 * VNCViewerTransportOpenCallback will be called shortly after VNCViewerStart, if
 * implementations for all transport replacement callbacks are registered with 
 * the SDK. 
 * pViewer and pContext pointers are passed to each of the transport callbacks and 
 * if desired, can be used to identify the desired connection endpoint to the open
 * callback, and can also be used to subsequently identify the specific connection
 * to Send/Recv/Close callbacks where necessary, for instance by storing a connection 
 * reference in the pContext data.
 * 
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNCViewer.
 *
 * \return VNCViewerTransportResult values as appropriate for the transport, but a
 * minimum of VNCViewerTransportResultSuccess or VNCViewerTransportResultFailure
 * 
 * \see VNCViewerCallbacks, VNCViewerTransportEvent, VNCViewerStart, VNCViewerTransportResult
 */
typedef VNCViewerTransportResult VNCCALL VNCViewerTransportOpenCallback(VNCViewer *pViewer,
                                                                        void *pContext);

/**
 * \brief Allows replacement of underlying transport, called when the 
 * SDK would normally close a TCP socket
 * 
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNCViewer.
 *
 * \return VNCViewerTransportResultSuccess or VNCViewerTransportResultFailure
 * 
 * \see VNCViewerCallbacks, VNCViewerTransportEvent, VNCViewerTransportResult
 */
typedef VNCViewerTransportResult VNCCALL VNCViewerTransportCloseCallback(VNCViewer *pViewer,
                                                                         void *pContext);

/**
 * \brief Allows replacement of underlying transport, called when the 
 * SDK would normally send data over TCP.
 * 
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNCViewer.
 *
 * \param buffer The data to send.
 *
 * \param length The length in bytes of the data pointed to by buffer.
 *
 * \return VNCViewerTransportResultSuccess, VNCViewerTransportResultFailure,
 * or VNCViewerTransportResultWouldBlock
 * 
 * \see VNCViewerCallbacks, VNCViewerTransportEvent, VNCViewerTransportResult
 */
typedef VNCViewerTransportResult VNCCALL VNCViewerTransportSendCallback(VNCViewer *pViewer,
                                                                        void *pContext, 
                                                                        const char* buffer, int length);

/**
 * \brief Allows replacement of underlying transport, called when the 
 * SDK would normally receive data over TCP
 * 
 * \param pViewer The VNC viewer instance to which this notification applies.
 * \param pContext The context pointer associated with the VNCViewer.
 *
 * \param buffer The buffer into which the requested data should be copied.
 *
 * \param length As an input, this is the length of data requested, as an output
 * this should be populated by the actual length of data received.
 *
 * \return VNCViewerTransportResultSuccess, VNCViewerTransportResultFailure,
 * or VNCViewerTransportResultWouldBlock
 * 
 * \see VNCViewerCallbacks, VNCViewerTransportEvent, VNCViewerTransportResult
 */
typedef VNCViewerTransportResult VNCCALL VNCViewerTransportRecvCallback(VNCViewer *pViewer,
                                                                        void *pContext,
                                                                        char* buffer, int* length);

/**
 * \brief Structure that holds pointers to callbacks invoked by the SDK.
 *
 * Any of the pointers in this structure may be NULL if you choose not to
 * implement a particular callback.  However, you will almost certainly want to
 * provide a VNCViewerStatusCallback, because otherwise it will be difficult to
 * terminate your application safely.
 *
 * For a description of each of the callbacks, please see the documentation for
 * the corresponding typedef.
 *
 * \see VNCViewerCreate
 */
typedef struct
{
    /**
     * Called by the SDK to notify of a change in the VNC viewer's status.
     */
    VNCViewerStatusCallback *pStatusCallback;

    /**
     * Called by the SDK when the VNC server challenges the VNC viewer for a
     * username and password.
     */
    VNCViewerCredentialsCallback *pCredentialsCallback;

    /**
     * Called by the SDK to notify you of the server signature.
     */
    VNCViewerServerSignatureCallback *pServerSignatureCallback;

    /**
     * Called by the SDK to inform you that the connection to the VNC server
     * has been completely established.
     */
    VNCViewerServerInitCallback *pServerInitCallback;

    /**
     * Called by the SDK to notify you that future updates will use a new pixel
     * format.
     */
    VNCViewerPixelFormatChangedCallback *pPixelFormatChangedCallback;

    /**
     * Called by the SDK to notify you that the VNC server has chosen a new
     * color map.
     */
    VNCViewerColorMapChangedCallback *pColorMapChangedCallback;

    /**
     * Called by the SDK to notify you that a rectangle on the VNC server's
     * desktop has been updated with the given pixel data.
     */
    VNCViewerImageRectangleCallback *pImageRectangleCallback;

    /**
     * Called by the SDK to notify you that a rectangle on the VNC server's
     * desktop has been copied from somewhere else.
     */
    VNCViewerCopyRectangleCallback *pCopyRectangleCallback;

    /**
     * Called by the SDK to notify you that a rectangle on the VNC server's
     * desktop has been filled with the given pixel.
     */
    VNCViewerFillRectangleCallback *pFillRectangleCallback;

    /**
     * Called by the SDK to notify you that the size of the VNC server's
     * desktop has changed.
     */
    VNCViewerDesktopResizeCallback *pDesktopResizeCallback;

    /**
     * Called by the SDK to notify you that the SDK has generated a key event.
     */
    VNCViewerPreKeyEventCallback *pPreKeyEventCallback;

    /**
     * Called by the SDK to notify you that the SDK has generated a pointer
     * event.
     */
    VNCViewerPrePointerEventCallback *pPrePointerEventCallback;

    /**
     * Called by the SDK to notify you that the SDK has received a message
     * from a protocol extension.
     */
    VNCViewerProtocolExtensionMessageCallback *pExtensionMessageCallback;

    /**
     * Called by the SDK to notify you that the SDK has received an encoded
     * rectangle for a registered decoder
     */
    VNCViewerRectangleDecodeCallback *pRectangleDecodeCallback;

    /**
     * Called by the SDK to notify you that the VNC server's clipboard contents
     * have been updated.
     */
    VNCViewerServerCutTextCallback *pServerCutTextCallback;

    /**
     * Called by the SDK to notify you that the server has requested a desktop
     * rotation
     */
    VNCViewerRotationCallback *pRotationCallback;

    /**
     * Called by the SDK to open the replacement transport
     */
    VNCViewerTransportOpenCallback *pTransportOpenCallback;

    /**
     * Called by the SDK to close the replacement transport
     */
    VNCViewerTransportCloseCallback *pTransportCloseCallback;

    /**
     * Called by the SDK to open the replacement transport
     */
    VNCViewerTransportSendCallback *pTransportSendCallback;

    /**
     * Called by the SDK to open the replacement transport
     */
    VNCViewerTransportRecvCallback *pTransportRecvCallback;

} VNCViewerCallbacks;

#endif /* !defined(__VNCCALLBACKS_H__) */
