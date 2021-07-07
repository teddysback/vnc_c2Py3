/* Copyright (C) 2002-2008 RealVNC Ltd.  All Rights Reserved.
*/

#ifndef __VNCTYPES_H__
#define __VNCTYPES_H__

/**
 * \file vnctypes.h
 *
 * This file defines types used by the RealVNC Viewer SDK for Intel AMT.  You
 * should normally include vncamt.h instead of including this file directly.
 *
 * \see VNCColor, VNCPoint, VNCRectangle, VNCPixelFormat
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
  #define VNCCALL __stdcall
  #ifdef VNC_VIEWERSDK_BUILD
    #define VNCDLLIMPORT
  #else
    #define VNCDLLIMPORT __declspec(dllimport)
  #endif
#else
  #define VNCCALL
  #define VNCDLLIMPORT
#endif

#include <stddef.h>

/**
 * \file keysymdef.h
 *
 * This file defines constants representing key symbols as they are transmitted
 * in the RFB protocol.  You should normally include vncamt.h instead of
 * including this file directly.
 *
 * The key symbol constants used with RFB are taken from the X Windows system.
 * You can use VNCSpecialKeyToXKeySymbol(), VNCUnicodeToXKeySymbol(),
 * VNCXKeySymbolToSpecialKey() and  VNCXKeySymbolToUnicode() to translate
 * platform-specific constants and Unicode characters to and from X key
 * symbols.
 *
 * This file is the copyright of The Open Group.  For full copyright
 * information, please see the copyright notice in the file itself.
 *
 * \see VNCSpecialKeyToXKeySymbol, VNCUnicodeToXKeySymbol,
 * VNCXKeySymbolToSpecialKey, VNCXKeySymbolToUnicode, VNCViewerSendKeyEvent,
 * VNCViewerPreKeyEventCallback
 */
#include "keysymdef.h"

/* \cond */
/* Define this if your build environment has the ISO C99 header stdint.h, or if
 * you can author one yourself.
 */
#if defined(VNC_USE_STDINT_H)
    #include <stdint.h>
    typedef int8_t vnc_int8_t;
    typedef int16_t vnc_int16_t;
    typedef int32_t vnc_int32_t;
    typedef int64_t vnc_int64_t;
    typedef uint8_t vnc_uint8_t;
    typedef uint16_t vnc_uint16_t;
    typedef uint32_t vnc_uint32_t;
    typedef uint64_t vnc_uint64_t;
/* Define this if these guesses are correct for your platform. */
#elif defined(VNC_USE_DEFAULT_INT_TYPEDEFS)
    typedef signed char vnc_int8_t;
    typedef signed short vnc_int16_t;
    typedef signed long vnc_int32_t;
    typedef signed long long vnc_int64_t;
    typedef unsigned char vnc_uint8_t;
    typedef unsigned short vnc_uint16_t;
    typedef unsigned long vnc_uint32_t;
    typedef unsigned long long vnc_uint64_t;
#else
    #error you must define either VNC_USE_STDINT_H or VNC_USE_DEFAULT_INT_TYPEDEFS
#endif
/* \endcond */

/**
 * \typedef VNCViewer
 * \brief Represents a single instance of a VNC viewer.
 *
 * This type is opaque; use the VNC viewer SDK APIs to interact with it.
 *
 * \see VNCViewerCreate
 */
typedef struct VNCViewerImplBase VNCViewer;

/**
 * \typedef VNCProtocolExtension
 * \brief Represents a single instance of a protocol extension
 *
 * This type is opaque; use the VNC viewer SDK APIs to interact with it.
 *
 * \see VNCViewerRegisterProtocolExtension
 */
typedef void VNCProtocolExtension;


/**
 * Status codes that may be notified via the VNCStatusCallback.  Any status
 * code that is not VNCViewerStatusNone indicates that the VNC session has
 * ended and that the VNC viewer thread has been or will be stopped.
 *
 * \see VNCViewerStatusCallback
 */
typedef enum
{
    /**
     * Placeholder value for variable initialization.
     */
    VNCViewerStatusNone = 0,

    /**
     * The server hostname could not be resolved.
     */
    VNCViewerStatusDNSFailure = 1,

    /**
     * The server network is unreachable.
     */
    VNCViewerStatusNetworkUnreachable = 2,

    /**
     * The server host is unreachable.
     */
    VNCViewerStatusHostUnreachable = 3,

    /**
     * The connection attempt timed out.
     */
    VNCViewerStatusConnectionTimedOut = 4,

    /**
     * The server host refused the TCP connection.
     */
    VNCViewerStatusConnectionRefused = 5,

    /**
     * The TCP connection to the VNC server was successfully established, but
     * this version of the RealVNC Viewer SDK is not compatible with this VNC
     * server.
     */
    VNCViewerStatusServerUnsupported = 6,

    /**
     * The credentials provided via VNCViewerCredentialsCallback are incorrect
     * (or you did not provide a VNCViewerCredentialsCallback at all).
     */
    VNCViewerStatusAuthenticationFailure = 7,

    /**
     * Your VNCViewerServerSignatureCallback indicated that the received server
     * signature was not correct for this server.
     */
    VNCViewerStatusServerSignatureFailure = 8,

    /**
     * The TCP connection to the server was lost.
     */
    VNCViewerStatusDisconnected = 9,

    /**
     * The user closed the desktop window.
     */
    VNCViewerStatusExited = 10,

    /**
     * VNCViewerStop() was called (either explicitly or from
     * VNCViewerDestroy()).
     */
    VNCViewerStatusStopped = 11,

    /**
     * The server gracefully closed the RFB session.
     */
    VNCViewerStatusConnectionClosed = 12
} VNCViewerStatus;

/**
 * \brief Stores the red, green and blue components of a particular color.
 * 
 * All of the component values range from 0 to 255 inclusive.  So, (0, 0, 0) is
 * black and (255, 255, 255) is white.
 *
 * \see VNCPixelFormat, VNCViewerColorMapChangedCallback
 */
typedef struct
{
    /** The red component of the color. */
    vnc_uint8_t red;
    /** The green component of the color. */
    vnc_uint8_t green;
    /** The blue component of the color. */
    vnc_uint8_t blue;
    /** The padding member is unused. */
    vnc_uint8_t padding;
} VNCColor;

/**
 * \brief Describes a point on the server desktop.
 *
 * The origin, (0, 0), is the point at the top left of the server desktop.
 *
 * \see VNCRectangle
 */
typedef struct
{
    /** The X-coordinate of the point. */
    vnc_int16_t x;
    /** The Y-coordinate of the point. */
    vnc_int16_t y;
} VNCPoint;

/**
 * \brief Describes a rectangular region of the server desktop.  
 *
 * The rectangle contains the pixels from topLeft inclusive through to
 * bottomRight non-inclusive.  That is, there are
 * (bottomRight.x - topLeft.x) * (bottomRight.y - topLeft.y) pixels in the
 * rectangle.
 *
 * \see VNCPoint
 */
typedef struct
{
    /** The point inside the rectangle at its top-left corner. */
    VNCPoint topLeft;
    /** The point just outside the rectangle at its bottom-right corner. */
    VNCPoint bottomRight;
} VNCRectangle;

/**
 * \brief Defines the format of each pixel in a buffer containing pixel data.
 *
 * If the AutoSelect parameter is set to "1", which is the default value, then
 * a VNC viewer will toggle between a number of preset pixel formats in
 * response to variations in network speed.  This behaviour is sufficient for
 * most purposes, but you can disable it by setting AutoSelect to "0", and then
 * setting ColorLevel.
 *
 * You may also select a custom pixel format by calling
 * VNCViewerSetPixelFormat().  You should choose custom pixel formats carefully
 * so that it is easy for the VNC server to generate pixel data in that format,
 * and easy for your application to render.
 *
 * \section byte_order Byte Order
 *
 * Pixel data can be big-endian or little-endian.  If the byte order differs
 * from the VNC viewer system's native byte order, you must swap the byte order
 * of every pixel before further interpreting it.
 *
 * When selecting a custom pixel format, you should choose a pixel format that
 * uses the VNC viewer system's native byte order (unless this would be
 * prohibitively expensive for the VNC server to work with).
 *
 * \section true_color Using True Color
 *
 * If trueColorFlag is non-zero, then the red, green and blue components of
 * each pixel are contained directly in the pixel data.  Each component can be
 * extracted using the maximum and shift values for that component.
 *
 * The maximum value for each component is (2^n - 1), where n is the number of
 * bits used to store that component.  For example, if a pixel's red component
 * is equal to redMax, then its red component is at full intensity.
 *
 * The shift value for each component is the number of right-shifts that must
 * be applied to each pixel value to move the least-significant-bit of that
 * component component into the least-significant-bit of the result.  For
 * example, after dealing with the pixel byte order, the red component can be
 * extracted by calculating ((pixel >> redShift) & redMax).
 *
 * For example, if bitsPerPixel is 8 and each pixel has the format 00rrggbb,
 * then:
 *
 *  - redMax, greenMax and blueMax are all equal to 3
 *  - redShift is 4
 *  - blueShift is 2
 *  - greenShift is 0
 *
 * \section color_map Using a Color Map
 *
 * If trueColorFlag is zero, then each value in the pixel data is an index into
 * a color map chosen by the VNC server.  The color map is updated by calls to
 * your VNCViewerColorMapChangedCallback.
 *
 * \see VNCViewerImageRectangleCallback, VNCViewerFillRectangleCallback,
 * VNCViewerSetPixelFormat, VNCViewerPixelFormatChangedCallback,
 * VNCViewerColorMapChangedCallback
 */
typedef struct
{
    /**
     * The number of bits of pixel data used for each pixel.  Must be either 8,
     * 16, or 32.
     */
    vnc_uint8_t bitsPerPixel;

    /**
     * The number of bits in each pixel that are significant.  Must be less
     * than or equal to bitsPerPixel.  (For example, if 32 bits are used to
     * represent pixels that have 8 bits each for red, green and blue, then
     * bitsPerPixel is 32 and depth is 24.)
     */
    vnc_uint8_t depth;

    /**
     * If non-zero, then the pixels are big-endian.  Otherwise, they are
     * little-endian.
     */
    vnc_uint8_t bigEndianFlag;

    /**
     * If non-zero, then the pixel data contains actual pixel values.
     * Otherwise, the pixel data contains indices into the accompanying color
     * map.
     */
    vnc_uint8_t trueColorFlag;

    /**
     * The maximum value of the red component of each pixel.
     */
    vnc_uint16_t redMax;

    /**
     * The maximum value of the green component of each pixel.
     */
    vnc_uint16_t greenMax;

    /**
     * The maximum value of the blue component of each pixel.
     */
    vnc_uint16_t blueMax;

    /**
     * The shift value of the red component of each pixel.
     */
    vnc_uint8_t redShift;

    /**
     * The shift value of the green component of each pixel.
     */
    vnc_uint8_t greenShift;

    /**
     * The shift value of the blue component of each pixel.
     */
    vnc_uint8_t blueShift;
} VNCPixelFormat;

/**
 * \brief Encoding numbers that may be returned by
 * VNCViewerGetProperty(VNCViewerPropertyLastUsedEncoding).
 *
 * For an explanation of these values, please see the RFB 4.0 Specification.
 */
typedef enum
{
    /** \cond */
    VNCEncodingRaw = 0,
    VNCEncodingCopyRect = 1,
    VNCEncodingRRE = 2,
    VNCEncodingHextile = 5,
    VNCEncodingTRLE = 15,
    VNCEncodingZRLE = 16
    /** \endcond */
} VNCEncoding;

typedef enum
{
  VNCScancodeNotSupported = 0,
  VNCScancodeSuccess = 1,
  VNCScancodeFailed = 2
} VNCScancodeKeyEventResponse;

/**
 * \brief Security type numbers that may be returned by
 * VNCViewerGetProperty(VNCViewerPropertySecurityType).
 *
 * For an explanation of these values, please see the RFB 4.0 Specification.
 */
typedef enum
{
    /** \cond */
    VNCSecurityTypeInvalid = 0,
    VNCSecurityTypeNone = 1,
    VNCSecurityTypeVncAuth = 2,
    VNCSecurityTypeRA2ne = 6
    /** \endcond */
} VNCSecurityType;

/**
 * Possible rotation settings
 */
typedef enum
{
  /** \cond */
  VNCRotation0 = 0,
  VNCRotation90 = 1,
  VNCRotation180 = 2,
  VNCRotation270 = 3,
  /** \endcond */
} VNCRotationSetting;

/**
 * Properties that may be queried via VNCViewerGetProperty().
 */
typedef enum
{
    /**
     * \brief Whether the VNC viewer thread is running.
     *
     * The value is non-zero if the VNC viewer has been started but has not yet
     * exited or been stopped.  Otherwise, the value is zero.
     */
    VNCViewerPropertyIsRunning,

    /**
     * \brief Whether the VNC viewer is fully connected to a VNC server.
     *
     * The value is non-zero if the VNC viewer is connected to a VNC server and
     * has completed the RFB handshake.  Otherwise, the value is zero.
     */
    VNCViewerPropertyIsConnected,

    /**
     * \brief The major component of the version number of the RFB protocol
     * that is being used for the session.
     */
    VNCViewerPropertyRFBVersionMajor,

    /**
     * \brief The minor component of the version number of the RFB protocol
     * that is in use for the session.
     */
    VNCViewerPropertyRFBVersionMinor,

    /**
     * \brief The actual unscaled width of the server desktop.
     */
    VNCViewerPropertyServerDesktopWidth,

    /**
     * \brief The actual unscaled height of the server desktop.
     */
    VNCViewerPropertyServerDesktopHeight,

    /**
     * \brief An approximate estimate of the connection speed, in bits per
     * second.
     */
    VNCViewerPropertyLineSpeedEstimate,

    /**
     * \brief The security type in use for the session.
     *
     * The returned value is a member of the VNCSecurityType enumeration.
     */
    VNCViewerPropertySecurityType,

    /**
     * \brief The RFB encoding that was last used in the session.
     *
     * The returned value is a member of the VNCEncoding enumeration.
     */
    VNCViewerPropertyLastUsedEncoding,    
    
    /**
     * \brief Non-zero if display updates are currently enabled.
     *
     * \see VNCViewerEnableDisplayUpdates
     */
    VNCViewerPropertyDisplayUpdatesEnabled,

    /**
     * \brief The current set rotation of the desktop, or the last requested rotation
     * as supplied to the application via VNCViewerRotationCallback
     *
     * \see VNCRotationSetting, VNCViewerSetRotation and VNCViewerRotationCallback
     */
     VNCViewerPropertyRotation,

} VNCViewerProperty;

/**
 * \brief Contextual information for parameter iteration.
 *
 * This structure is used to hold state in between calls to
 * VNCViewerIterateParameterNames().  The caller allocates and zeroes a
 * VNCParameterIterationContext structure before their first call to that
 * method, and then passes the same VNCParameterIterationContext into
 * subsequent calls to continue iteration.
 *
 * \see VNCViewerIterateParameterNames()
 */
typedef struct
{
    /* \cond */
    void *p1;
    void *p2;
    /* \endcond */
} VNCParameterIterationContext;


/**
* \brief Replacement transport status
*  
* Defines valid status values to be passed as parameters to VNCViewerTransportEvent.
*
* \see VNCViewerTransportEvent
*/
typedef enum
{
    /**
     * Indicates that a read event has been received
     */
    VNCViewerTransportStatusReadReady = 1,
    /**
     * Indicates that a write event has been received
     */
    VNCViewerTransportStatusWriteReady = 2,
    /**
     * Indicates that a close event has been received
     */
    VNCViewerTransportStatusClose = 3,
} VNCViewerTransportStatus;

/** 
 * \brief Replacement transport callback return codes
 *
 * Defines valid return values from the various transport callback 
 * functions to indicate success or various failure types. 
 *
 * \see VNCViewerTransportOpenCallback, VNCViewerTransportCloseCallback,
 * VNCViewerTransportSendCallback, VNCViewerTransportRecvCallback
 */
typedef enum
{    /**
     * Used by transport callback to indicate successful completion.
     */
    VNCViewerTransportResultSuccess = 0,
    /**
     * Used by transport callback to indicate generic failure.
     */
    VNCViewerTransportResultFailure = 1,
    /**
     * Used by transport callback to indicate that it would block if
     * attempted, and that it is returning instead.
     */
    VNCViewerTransportResultWouldBlock = 2,

    /**
     * The transport callback should return this value if the server hostname could 
     * not be resolved.
     */
    VNCViewerTransportResultDNSFailure = 3,
    /**
     * The transport callback should return this value if the server network is 
     * unreachable.
     */
    VNCViewerTransportResultNetworkUnreachable = 4,
    /**
     * The transport callback should return this value if the server host is 
     * unreachable.
     */
    VNCViewerTransportResultHostUnreachable = 5,
    /**
     * The transport callback should return this value if the connection attempt timed out.
     */
    VNCViewerTransportResultConnectionTimedOut = 6,
    /**
     *  The transport callback should return this value if the server host refused the connection.
     */
    VNCViewerTransportResultConnectionRefused = 7,
} VNCViewerTransportResult;

/**
 * \brief Bitmask values indicating different buttons on a pointer device.
 *
 * RFB uses values of the form (2 << (N - 1)) to indicate 'pointer device
 * button N)'.  This enumeration provides symbolic names for these values.
 *
 * On a conventional mouse, buttons 1, 2 and 3 correspond to the left, middle
 * and right buttons on the mouse respectively. On a wheel mouse, each step of
 * the wheel upwards is represented by a press and release of button 4, and
 * each step downwards is represented by a press and release of button 5.
 *
 * \see VNCViewerSendPointerEvent, VNCViewerPrePointerEventCallback
 */
typedef enum
{
    VNCPointerDeviceButton1 = 0x01,
    VNCPointerDeviceButton2 = 0x02,
    VNCPointerDeviceButton3 = 0x04,
    VNCPointerDeviceButton4 = 0x08,
    VNCPointerDeviceButton5 = 0x10,
    VNCPointerDeviceButton6 = 0x20,
    VNCPointerDeviceButton7 = 0x40,
    VNCPointerDeviceButton8 = 0x80,

    VNCPointerDeviceButtonLeft = VNCPointerDeviceButton1,
    VNCPointerDeviceButtonMiddle = VNCPointerDeviceButton2,
    VNCPointerDeviceButtonRight = VNCPointerDeviceButton3,
    VNCPointerDeviceButtonWheelUp = VNCPointerDeviceButton4,
    VNCPointerDeviceButtonWheelDown = VNCPointerDeviceButton5
} VNCPointerDeviceButton;

/**
 * \brief The information contained in an RFB ConnClose message.
 *
 * Prior to closing a connection, a VNC Server may send a ConnClose message to
 * the VNC Viewer, indicating why it has closed the connection.  If the SDK
 * receives a ConnClose from the VNC Server, then it will invoke the
 * VNCViewerStatusCallback with status VNCViewerStatusConnectionClosed, and
 * supply a pointer to a VNCConnCloseReason containing the contents of the
 * ConnClose message.
 *
 * \see VNCViewerStatusCallback
 */
typedef struct
{
    /**
     * \brief The reason that the VNC Server has chosen to close the
     * connection.
     *
     * The VNC Viewer application may choose to log this information or display
     * it to the end user.  Note that most VNC Server implementations are not
     * internationalized, and that if reasonString contains a human-readable
     * error message, it may not be correct for the VNC Viewer application's
     * locale.
     *
     * reasonString may not be NULL but may be zero-length.
     */
    const char *reasonString;

    /**
     * \brief A hint from the VNC Server as to whether the reasonString should
     * be displayed to the end user.
     *
     * The VNC Viewer application may choose to ignore this hint.
     */
    vnc_uint8_t alertUser;
} VNCConnClose;

#ifdef __cplusplus
}
#endif

#endif /* !defined(__VNCTYPES_H__) */
