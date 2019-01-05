/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2016/07/hal-library-35-gps-parser-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   GPS NMEA standard data parser for STM32Fxxx devices
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_GPS_H
#define TM_GPS_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_GPS
 * @brief    GPS NMEA standard data parser for STM32Fxxx devices - http://stm32f4-discovery.net/2016/07/hal-library-35-gps-parser-stm32fxxx/
 * @{
 *
 * This library parses response from GPS module, in NMEA statements format.
 * 
 * For more, how GPS returns data, look at link below:
 *
 *	http://aprs.gids.nl/nmea/
 *
 * There is a lot of possible sentences, which can be returned from GPS.
 * This library uses only 4. But some GPS don't returns all of this 4 sentences,
 * so I made a possibility to disable them.
 *
 * By default, these statements are in use and supported:
 *  - GPGGA: Global Positioning System Fix Data
 *     - Latitude
 *     - Longitude
 *     - Altitude
 *     - Fix
 *     - Satellites in use
 *     - Time
 *  - GPRMC: Recommended minimum specific GPS/Transit data
 *     - Date
 *     - Speed
 *     - Validity
 *     - Direction
 *  - GPGSA: GPS DOP and Active Satellites
 *     - HDOP
 *     - VDOP
 *     - PDOP
 *     - FixMode
 *     - Id's of satellites in use
 *  - GPGSV: GPS Satellites in view
 *     - Satellites in view
 *     - Description of all satellites in view
 *  - Custom statements defined by user
 *
 * By default, each of this data has to be detected in order to get "VALID" data.
 * If your GPS does not return any of this statement, you can disable option.
 * If you disable any of statements, then you will loose data, corresponding to statement.
 *
 * Add lines below in your defines.h file and uncomment your settings
\code
//Disable GPGGA statement
#define GPS_DISABLE_GPGGA
//Disable GPRMC statement
#define GPS_DISABLE_GPRMC 
//Disable GPGSA statement
#define GPS_DISABLE_GPGSA
//Disable GPGSV statement
#define GPS_DISABLE_GPGSV
\endcode
 *
 * \par Pinout
 *
 * To communicate with GPS, USART is commonly used. By default, my library uses USART1,
 * but if you want your own USARTx, add lines below in your defines.h file:
 *
\code
#define GPS_USART				USART1
#define GPS_USART_PINSPACK		TM_USART_PinsPack_2
\endcode
 *
 * With default pinout:
 *
 *  STM32F4xx TX = PB6\n
 *  STM32F4xx RX = PB7
 *
 * @note Connect GPS's TX to STM32F4xx's RX and vice versa
 *
 * \par Increase USART internal buffer
 *
 * GPS library is based on USART library, which uses internal buffers to store data which are received but not read.
 * 
 * GPS receiver will send a lot of data at a time and if you are doing other stuff that time, it might happen that you
 * will not so quickly update GPS data and buffer will overflow. Default buffer size is for my USART lib 32 bytes.
 *
 * I recommend that you increase that memory. 
 * For further instructions how to do that, look at @ref TM_USART module.
 *
 * \par Distance and bearing
 *
 * Distance between 2 points can not be provided from GPS receiver, so we have to calculate it. 
 * This is useful, if you have let’s say quadcopter and you want to know, how far you are from quad. 
 * First coordinate will be quad’s current position from GPS and second point will be set before you start with your flight.
 * 
 * If you want to make let’s say “Return home” function on quadcopter, bearing comes to be very useful.
 * GPS module tells you your direction (included in library) where you are currently moving. 
 * Still, first coordinate is from quad’s GPS receiver, and second is where quad started with flight.
 * When you activate “Return home” function, quad will know how far he is from “home” and 
 * in which direction (bearing) he has to move according to the north. 
 * Then, you just have to compare your calculated bearing with actual direction provided from GPS.
 * And you will know, if he needs to go more left, right, etc. You can tune PID then according to values.
 *
 * \par Custom GPS statements
 *
 * Library supports by default 4 statements. 
 * It assumes that every GPS module should output these 4 statements.
 * It may happen, that your module outputs data that you need, but are not available with my library by default.
 *
 * Here is why custom GPS statements come handy. 
 * It is possible to define custom statements which will be parsed as strings from GPS receiver directly to user.
 * 
 * For example, let’s say that your module outputs $GPRMB sentence (navigation).
 * From that statement, you want to get “Steering value”, which can be “L” or “R” for Left or Right. 
 * Steering value is part 3 inside GPRMB sentence. If you want to include this into your output, you can do this:
\code
//Set variable
TM_GPS_Custom_t* GPRMB_3_Pointer;

//$GPRMB statement, term number 3 = Steering value
GPRMB_3_Pointer = TM_GPS_AddCustom(&GPS_Data, "$GPRMB", 3);
\endcode
 * This statement is now added into main GPS_Data structure and also, pointer to small GPRMB.3 structure is returned.
 * @note You have to make sure, that in this case, GPRMB statement is really returned from module.
 *       If module will not return this statement, you will not be able to read anything from library,
 *       because it will never happen that everything will be updated before new data are available.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM USART
 - TM GPIO
 - defines.h
 - math.h
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_gpio.h"
#include "defines.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/**
 * @defgroup TM_GPS_Macros
 * @brief    Library private defines without any sense for USER
 * @{
 */

/* Default GPS USART used */
#ifndef GPS_USART
#define GPS_USART                   USART1
#define GPS_USART_PINSPACK          TM_USART_PinsPack_2
#endif

/* Checks if USART buffer for GPS is empty */
#ifndef GPS_USART_BUFFER_EMPTY
#define GPS_USART_BUFFER_EMPTY      TM_USART_BufferEmpty(GPS_USART)
#endif

/* Get character from USART buffer for GPS */
#ifndef GPS_USART_BUFFER_GET_CHAR
#define GPS_USART_BUFFER_GET_CHAR   TM_USART_Getc(GPS_USART)
#endif

/* Init function for GPS */
#ifndef GPS_USART_INIT
#define GPS_USART_INIT(baudrate)    TM_USART_Init(GPS_USART, GPS_USART_PINSPACK, baudrate)
#endif

/* Maximum number of custom GPGxx values */
#ifndef GPS_CUSTOM_NUMBER
#define GPS_CUSTOM_NUMBER       10
#endif

 /**
 * @}
 */

/**
 * @defgroup TM_GPS_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  GPS Result enumeration
 */
typedef enum {
	TM_GPS_Result_NewData,          /*!< New data are available to operate with */
	TM_GPS_Result_OldData,          /*!< We don't have new data available yet */
	TM_GPS_Result_FirstDataWaiting, /*!< We are waiting for first data from GPS module */
} TM_GPS_Result_t;


/**
 * @brief  Speed conversion enumeration
 * @note   Speed from GPS is in knots, use TM_GPS_ConvertSpeed() to convert to useable value
 */
typedef enum {
	/* Metric */
	TM_GPS_Speed_KilometerPerSecond,  /*!< Convert speed to kilo meters per second */
	TM_GPS_Speed_MeterPerSecond,      /*!< Convert speed to meters per second */
	TM_GPS_Speed_KilometerPerHour,    /*!< Convert speed to kilo meters per hour */
	TM_GPS_Speed_MeterPerMinute,      /*!< Convert speed to meter per minute */
	/* Imperial */
	TM_GPS_Speed_MilePerSecond,       /*!< Convert speed to miles per second */
	TM_GPS_Speed_MilePerHour,         /*!< Convert speed to miles per hour */
	TM_GPS_Speed_FootPerSecond,       /*!< Convert speed to foots per second */
	TM_GPS_Speed_FootPerMinute,       /*!< Convert speed to foots per minute */
	/* For Runners and Joggers */
	TM_GPS_Speed_MinutePerKilometer,  /*!< Convert speed to minutes per kilo meter */
	TM_GPS_Speed_SecondPerKilometer,  /*!< Convert speed to seconds per kilo meter */
	TM_GPS_Speed_SecondPer100Meters,  /*!< Convert speed to seconds per 100 meters */
	TM_GPS_Speed_MinutePerMile,       /*!< Convert speed to minutes per mile */
	TM_GPS_Speed_SecondPerMile,       /*!< Convert speed to seconds per mile */
	TM_GPS_Speed_SecondPer100Yards,   /*!< Convert speed to seconds per 100 yards */
	/* Nautical */
	TM_GPS_Speed_SeaMilePerHour,      /*!< Convert speed to sea miles per hour */
} TM_GPS_Speed_t;


/**
 * @brief  GPS float structure
 * @note   GPS has longitude, latitude, altitude, etc.. in float format.
 *         
 *         But, if you want to make a tracker and save data to SD card, there is a problem, because FATFS library breaks %f with sprintf() function.
 *         
 *         For that purpose I made a new function TM_GPS_ConvertFloat() which converts float number to an integer and decimal part.
 */
typedef struct {
	int32_t Integer;   /*!< Integer part of float number. */
	uint32_t Decimal;  /*!< Decimal part of float number, in integer format. */
} TM_GPS_Float_t;

/**
 * @brief  Date struct for GPS date 
 */
typedef struct {
	uint8_t Date;  /*!< Date in month from GPS. */
	uint8_t Month; /*!< Month from GPS. */
	uint8_t Year;  /*!< Year from GPS. */
} TM_GPS_Date_t;

/**
 * @brief  Time structure for GPS
 */
typedef struct {
	uint8_t Hours;       /*!< Hours from GPS time. */
	uint8_t Minutes;     /*!< Minutes from GPS time. */			
	uint8_t Seconds;     /*!< Seconds from GPS time. */
	uint16_t Hundredths; /*!< Hundredths from GPS time. */
} TM_GPS_Time_t;

/**
 * @brief  Satellite in view description structure
 */
typedef struct {
	uint8_t ID;        /*!< SV PRN number */
	uint8_t Elevation; /*!< Elevation in degrees, 90 maximum */
	uint16_t Azimuth;  /*!< Azimuth, degrees from true north, 000 to 359 */
	uint8_t SNR;       /*!< SNR, 00-99 dB (0 when not tracking) */
} TM_GPS_Satellite_t;

/**
 * @brief  Custom NMEA statement and term, selected by user 
 */
typedef struct {
	char Statement[7];  /*!< Statement value, including "$" at beginning. For example, "$GPRMC" */
	uint8_t TermNumber; /*!< Term number position inside statement */
	char Value[15];     /*!< Value from GPS receiver at given statement and term number will be stored here.
	                            @note Value will not be converted to number if needed, but will stay as a character */
	uint8_t Updated;    /*!< Updated flag. If this parameter is set to 1, then new update has been made. Meant for private use */
} TM_GPS_Custom_t;

/**
 * @brief  Main GPS data structure 
 */
typedef struct {
#ifndef GPS_DISABLE_GPGGA
	float Latitude;                                       /*!< Latitude position from GPS, -90 to 90 degrees response. */
	float Longitude;                                      /*!< Longitude position from GPS, -180 to 180 degrees response. */
	float Altitude;                                       /*!< Altitude above the seain units of meters */
	uint8_t Satellites;                                   /*!< Number of satellites in use for GPS position. */
	uint8_t Fix;                                          /*!< GPS fix; 0: Invalid; 1: GPS Fix; 2: DGPS Fix. */
	TM_GPS_Time_t Time;                                   /*!< Current time from GPS. @ref TM_GPS_Time_t. */
#endif
#ifndef GPS_DISABLE_GPRMC
	TM_GPS_Date_t Date;                                   /*!< Current date from GPS. @ref TM_GPS_Date_t. */
	float Speed;                                          /*!< Speed in knots from GPS. */
	uint8_t Validity;                                     /*!< GPS validation; 1: valid; 0: invalid. */
	float Direction;                                      /*!< Course on the ground in relation to North. */
#endif
#ifndef GPS_DISABLE_GPGSA
	float HDOP;                                           /*!< Horizontal dilution of precision. */
	float PDOP;                                           /*!< Position dilution od precision. */
	float VDOP;                                           /*!< Vertical dilution of precision. */
	uint8_t FixMode;                                      /*!< Current fix mode in use:; 1: Fix not available; 2: 2D; 3: 3D. */
	uint8_t SatelliteIDs[12];                             /*!< Array with IDs of satellites in use. 
	                                                           Only first data are valid, so if you have 5 satellites in use, only SatelliteIDs[4:0] are valid */
#endif
#ifndef GPS_DISABLE_GPGSV	
	uint8_t SatellitesInView;                             /*!< Number of satellites in view */
	TM_GPS_Satellite_t SatDesc[30];                       /*!< Description of each satellite in view */ 
#endif
	TM_GPS_Result_t Status;                               /*!< GPS result. This parameter is value of @ref TM_GPS_Result_t */
	TM_GPS_Custom_t* CustomStatements[GPS_CUSTOM_NUMBER]; /*!< Array of pointers for custom GPS NMEA statements, selected by user.
	                                                              You can use @ref GPS_CUSTOM_NUMBER number of custom statements */
	uint8_t CustomStatementsCount;                        /*!< Number of custom GPS statements selected by user */
} TM_GPS_t;

/* Backward compatibility */
typedef TM_GPS_t TM_GPS_Data_t;

/**
 * @brief  GPS Distance and bearing struct
 */
typedef struct {
	float Latitude1;  /*!< Latitude of starting point. */
	float Longitude1; /*!< Longitude of starting point. */
	float Latitude2;  /*!< Latitude of ending point. */
	float Longitude2; /*!< Longitude of ending point. */
	float Distance;   /*!< Distance between 2 points which will be calculated. */
	float Bearing;    /*!< Bearing from start to stop point according to North. */
} TM_GPS_Distance_t;

/**
 * @}
 */

/**
 * @defgroup TM_GPS_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes GPS and USART peripheral
 * @param  *GPS_Data: Pointer to @ref TM_GPS_t structure to set default values
 * @param  baudrate: Specify GPS baudrate for USART. Most common are 9600 or 115200 bauds
 * @note   GPS baudrate can have other values. Check GPS datasheet for proper info.
 * @retval None
 */
void TM_GPS_Init(TM_GPS_t* GPS_Data, uint32_t baudrate);

/**
 * @brief  Update GPS data.
 * @note   This function must be called periodically, as fast as possible. 
 *         It basically checks if data is available on GPS USART and parse it to useful data for user.
 * @note   - When you first call this function and there is not available data from GPS, this function will return @ref TM_GPS_Result_FirstTimeWaiting.
 *         - This will be returning all the time we don't have any useful data.
 *         - When first time useful data is received from GPS (everything parsed), @ref TM_GPS_Result_NewData will be returned.
 *         - When we have already new data, next time we call this function, @ref TM_GPS_Result_OldData will be returning until we don't receive new packet of useful data.
 * @note   If you are making GPS logger, then when you receive @ref TM_GPS_Result_NewData it is time to save your data.
 * @param  *GPS_Data: Pointer to working @ref TM_GPS_t structure
 * @retval Returns value of @ref TM_GPS_Result_t structure
 */
TM_GPS_Result_t TM_GPS_Update(TM_GPS_t* GPS_Data);

/**
 * @brief  Converts speed in knots (from GPS) to user selectable speed
 * @param  speedInKnots: float value from GPS module
 * @param  toSpeed: Select to which speed you want conversion from knot. This parameter ca be a value of TM_GPS_Speed_t enumeration.
 * @retval Calculated speed from knots to user selectable format
 */
float TM_GPS_ConvertSpeed(float SpeedInKnots, TM_GPS_Speed_t toSpeed);

/**
 * @brief  Converts float number into integer and decimal part
 * @param  num: Float number to split into 2 parts
 * @param  *Float_Data: Pointer to TM_GPS_Float_t structure where to save result
 * @param  decimals: Number of decimal places for conversion
 * @note   Example: You have number 15.002 in float format.
 *            - You want to split this to integer and decimal part with 6 decimal places.
 *            - Call @ref TM_GPS_ConvertFloat(15.002, &Float_Struct, 6);
 *            - Result will be: Integer: 15; Decimal: 2000 (0.002 * 10^6)
 * @retval None
 */
void TM_GPS_ConvertFloat(float num, TM_GPS_Float_t* Float_Data, uint8_t decimals);

/**
 * @brief  Calculates distance between 2 coordinates on earth and bearing from start to end point in relation to the north
 * @param  *Distance_Data: Pointer to @ref TM_GPS_Distance_t structure with latitude and longitude set values
 * @note   Calculation results will be saved in *Distance_Data @ref TM_GPS_Distance_t structure
 * @retval None
 */
void TM_GPS_DistanceBetween(TM_GPS_Distance_t* Distance_Data);

/**
 * @brief  Adds custom GPG statement to array of user selectable statements.
 *            Array is available to user using @ref TM_GPS_t workign structure
 * @note   Functions uses @ref malloc() function to allocate memory, so make sure you have enough heap memory available.
 * @note   Also note, that your GPS receiver HAVE TO send statement type you use in this function, or 
 *            @ref TM_GPS_Update() function will always return that there is not data available to read.
 * @param  *GPS_Data: Pointer to working @ref TM_GPS_t structure
 * @param  *GPG_Statement: String of NMEA starting line address, including "$" at beginning
 * @param  TermNumber: Position in NMEA statement
 * @retval Success status:
 *            - NULL: Malloc() failed or you reached limit of user selectable custom statements:
 *            - > NULL: Function succeded, pointer to @ref TM_GPS_Custom_t structure
 */
TM_GPS_Custom_t * TM_GPS_AddCustom(TM_GPS_t* GPS_Data, char* GPG_Statement, uint8_t TermNumber);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */ 

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

