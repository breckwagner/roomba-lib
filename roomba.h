/**
 * @file roomba.h
 * @defgroup roomba-lib Roomba Library
 * @code #include <roomba.h> @endcode
 *
 * @brief Roomba library that enumurates the entire roomba open interface and
 * defines useful commands and macros
 *
 * @note Based on the iRobot® Create® 2 Open Interface (OI) written by the
 * iRobot®. Specification based on the iRobot® Roomba® 600 but may be
 * compatible with other models.
 *
 * @author Richard B. Wagner
 * @date 2016-04-04
 */

#ifndef ROOMBA_H_
#define ROOMBA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/**@{*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * ROOMBA_INTERFACE_VERSION can asume several values and must be definied for
 * this library to work properly. The number corresponds to the version of the
 * interface. Also, features that have lost forward support are marked in this
 * doc as deprecated but this is only done for convinience.
 *
 * | # | Name                                                                  |
 * |---|-----------------------------------------------------------------------|
 * | 0 | Roomba® Serial Command Interface (SCI)                                |
 * | 1 | Create® Open Interface (OI)                                           |
 * | 2 | Create® 2 Open Interface (OI)                                         |
 *
 * The easiest way to define this value is to include it in a makefile as a
 * compiler flag:
 *
 * '''bash
 * -D ROOMBA_INTERFACE_VERSION=*#*
 * '''
 *
 * @note If it is note defined, the version will default to 2.
 */
#ifndef ROOMBA_INTERFACE_VERSION
  #define ROOMBA_INTERFACE_VERSION 2
#endif

#if ROOMBA_INTERFACE_VERSION==2
  #define ROOMBA_DEFAULT_BAUD_RATE 115200
#elif ROOMBA_INTERFACE_VERSION==1
  #define ROOMBA_DEFAULT_BAUD_RATE 57600
#else
  #warning Interface version not specified
#endif

#define LOW_BYTE(v)   ((unsigned char) (v))
#define HIGH_BYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))
#define ROOMBA_RADIUS_STRAIGHT_NEGATIVE 0x8000
#define ROOMBA_RADIUS_STRAIGHT_POSITIVE 0x7FFF
#define ROOMBA_RADIUS_STRAIGHT ROOMBA_RADIUS_STRAIGHT_POSITIVE
#define ROOMBA_RADIUS_CLOCKWISE 0xFFFF
#define ROOMBA_RADIUS_COUNTER_CLOCKWISE 0x0001

/**
 * @brief OP_CODE enum
 *
 * Operation codes for roomba
 */
typedef enum _roomba_op_code {
  /**
   * Reset
   * Opcode: 7
   * Data Bytes: 0
   *
   * This command resets the robot, as if you had removed and reinserted the
   * battery.
   *   - Serial sequence: [7].
   *   - Available in modes: Always available.
   *   - Changes mode to: Off. You will have to send [128] again to re-enter
   *     Open Interface mode.
   *
   * @since 0 (SCI)
   */
  ROOMBA_RESET = 7,

  /**
   * Start
   * Opcode: 128
   * Data Bytes: 0
   *
   * This command starts the OI. You must always send the Start command before
   * sending any other commands to the OI.
   *   - Serial sequence: [128].
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive. Roomba beeps once to acknowledge it is
   *     starting * from “off” mode.
   *
   * @since 0 (SCI)
   */
  ROOMBA_START = 128,

  /**
   * Baud
   * Opcode: 129
   * Data Bytes: 1
   *
   * This command sets the baud rate in bits per second (bps) at which OI
   * commands and data are sent according to the baud code sent in the data
   * byte. The default baud rate at power up is 115200 bps, but the starting
   * baud rate can be changed to 19200 by following the method outlined on page
   * 4. Once the baud rate is changed, it persists until Roomba is power cycled
   * by pressing the power button or removing the battery, or when the battery
   * voltage falls below the minimum required for processor operation. You
   * must wait 100ms after sending this command before sending additional
   * commands at the new baud rate.
   *   - Serial sequence: [129][Baud Code]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Baud data byte 1: Baud Code (0 - 11)
   *
   * | Baud Code | Baud Rate in BPS |
   * |-----------|------------------|
   * | 0         | 300              |
   * | 1         | 600              |
   * | 2         | 1200             |
   * | 3         | 2400             |
   * | 4         | 4800             |
   * | 5         | 9600             |
   * | 6         | 14400            |
   * | 7         | 19200            |
   * | 8         | 28800            |
   * | 9         | 38400            |
   * | 10        | 57600            |
   * | 11        | 115200           |
   *
   * @since 0 (SCI)
   */
  ROOMBA_BAUD = 129,

  /**
   * Control Command
   * opcode: 130
   * Data bytes: 0
   *
   * Enables user control of Roomba. This command must be sent after the start
   * command and before any control commands are sent to the SCI. The SCI must
   * be in passive mode to accept this command. This command puts the SCI in
   * safe mode.
   * Serial sequence: [130]
   *
   * @since 0 (SCI)
   *
   * @deprecated
   */
  ROOMBA_CONTROL = 130,

  /**
   * Safe
   * Opcode: 131
   * Data Bytes: 0
   *
   * This command puts the OI into Safe mode, enabling user control of Roomba.  * It turns off all LEDs. The OI can be in Passive, Safe, or Full mode to
   * accept this command. If a safety condition occurs (see above)
   * Roomba reverts automatically to Passive mode.
   *   - Serial sequence: [131]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Safe
   *
   * @note The effect and usage of the Control command (130) are identical to
   * the Safe command (131).
   *
   * @since 0 (SCI)
   */
  ROOMBA_SAFE = 131,

  /**
   * Full
   * Opcode: 132
   * Data Bytes: 0
   *
   * This command gives you complete control over Roomba by putting the OI into
   * Full mode, and turning off the cliff, wheel-drop and internal charger
   * safety features. That is, in Full mode, Roomba executes any command that
   * you send it, even if the internal charger is plugged in, or command
   * triggers a cliff or wheel drop condition.
   *   - Serial sequence: [132]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Full
   *
   * @note Use the Start command (128) to change the mode to Passive
   *
   * @since 0 (SCI)
   */
  ROOMBA_FULL = 132,

  /**
   * Power
   * Opcode: 133
   * Data Bytes: 0
   *
   * This command powers down Roomba. The OI can be in Passive, Safe, or Full
   * mode to accept this command.
   *   - Serial sequence: [133]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   *
   * @since 0 (SCI)
   */
  ROOMBA_POWER = 133,

  /**
   * Spot
   * Opcode: 134
   * Data Bytes: 0
   *
   * This command starts the Spot cleaning mode. This is the same as pressing
   * Roomba’s Spot button, and will pause a cleaning cycle if one is already in
   * progress.
   *   - Serial sequence: [134]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   *
   * @since 0
   */
  ROOMBA_SPOT = 134,

  #if ROOMBA_INTERFACE_VERSION==1

  /**
   * Cover
   * Opcode: 135
   * Data Bytes: 0
   *
   * This command starts the Cover demo.
   *   - Serial sequence: [135]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   */
  ROOMBA_COVER = 135,

  #elif ROOMBA_INTERFACE_VERSION==2

  /**
   * Clean
   * Opcode: 135
   * Data Bytes: 0
   *
   * This command starts the default cleaning mode. This is the same as
   * pressing Roomba’s Clean button, and will pause a cleaning cycle if one is
   * already in progress.
   *   - Serial sequence: [135]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   */
  ROOMBA_CLEAN = 135,

  #endif

  #if ROOMBA_INTERFACE_VERSION==1

  /**
   * Demo
   * Opcode: 136
   * Data Bytes: 1
   *
   * This command starts the requested built-in demo.
   *   - Serial sequence: [136][Which-demo]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   *   - Demo data byte 1: Demo number (-1 - 9)
   *

   Demo Names, Descriptions and Numbers

   * | Number  | Demo           | Description |
   * |---------|----------------|-------------|
   * |-1 (255) | Abort          | current demo Stops the demo that Create is currently performing. |

   * | 0       | Cover          | Create attempts to cover an entire room using a combination of behaviors, such as random bounce, wall following, and spiraling. |

   * | 1       | Cover and Dock | Identical to the Cover demo, with one exception. If Create sees an infrared signal from an iRobot Home Base, it uses that signal to dock with the Home Base and recharge itself. |

   * | 2       | Spot Cover     | Create covers an area around its starting position by spiraling outward, then inward. |

   * | 3       | Mouse          | Create drives in search of a wall. Once a wall is found, Create drives along the wall, traveling around circumference of the room. |

   * | 4 Drive Figure Eight Create continuously drives in a figure 8
   pattern.

   * | 5 Wimp Create drives forward when pushed from
   behind. If Create hits an obstacle while
   driving, it drives away from the obstacle.

   * | 6 Home Create drives toward an iRobot Virtual
   @todo formating
   Wall as long as the back and sides of
   the virtual wall receiver are blinded by
   black electrical tape.
   A Virtual Wall emits infrared signals
   that Create sees with its Omnidirectional
   Infrared Receiver, located on top of the
   bumper.
   If you want Create to home in on a
   Virtual Wall, cover all but a small
   opening in the front of the infrared
   receiver with black electrical tape.
   Create spins to locate a virtual wall,
   then drives toward it. Once Create hits
   the wall or another obstacle, it stops.
   Number Demo Description
   7 Tag Identical to the Home demo, except
   Create drives into multiple virtual walls
   by bumping into one, turning around,
   driving to the next virtual wall, bumping
   into it and turning around to bump into
   the next virtual wall.
   8 Pachelbel Create plays the notes of Pachelbel’s
   Canon in sequence when cliff sensors
   are activated.
   9 Banjo Create plays a note of a chord for each
   of its four cliff sensors. Select the
   chord using the bumper, as follows:
   • No bumper: G major.
   • Right/left bumper: D major 7
   • Both bumpers (center): C major

   * @deprecated
   */
  ROOMBA_DEMO = 136,

  #elif ROOMBA_INTERFACE_VERSION==2

  /**
   * Max
   * Opcode: 136
   * Data Bytes: 0
   *
   * This command starts the Max cleaning mode, which will clean until the
   * battery is dead. This command will pause a cleaning cycle if one is
   * already in progress.
   *   - Serial sequence: [136]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   */
  ROOMBA_MAX = 136,

  #endif

  /**
   * Drive
   * Opcode: 137
   * Data Bytes: 4
   *
   * This command controls Roomba’s drive wheels. It takes four data bytes,
   * interpreted as two 16-bit signed values using two’s complement.
   * (http://en.wikipedia.org/wiki/Two%27s_complement) The first two bytes
   * specify the average velocity of the drive wheels in millimeters per second
   * (mm/s), with the high byte being sent first. The next two bytes specify
   * the radius in millimeters at which Roomba will turn. The longer radii make
   * Roomba drive straighter, while the shorter radii make Roomba turn more.
   * The radius is measured from the center of the turning circle to the center
   * of Roomba. A Drive command with a positive velocity and a positive radius
   * makes Roomba drive forward while turning toward the left. A negative
   * radius makes Roomba turn toward the right. Special cases for the radius
   * make Roomba turn in place or drive straight, as specified below. A
   * negative velocity makes Roomba drive backward.
   * @note Internal and environmental restrictions may prevent Roomba from
   * accurately carrying out some drive commands. For example, it may not be
   * possible for Roomba to drive at full speed in an arc with a large radius
   * of curvature.
   *   - Serial sequence: [137] [Velocity high byte] [Velocity low byte]
   *     [Radius high byte] [Radius low byte]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Velocity (-500 – 500 mm/s)
   *   - Radius (-2000 – 2000 mm)
   * Special cases:
   * Straight = 32768 or 32767 = 0x8000 or 0x7FFF
   * Turn in place clockwise = -1 = 0xFFFF
   * Turn in place counter-clockwise = 1 = 0x0001
   * Example:
   * To drive in reverse at a velocity of -200 mm/s while turning at a radius
   * of 500mm, send the following serial byte sequence:
   * [137] [255] [56] [1] [244]
   * Explanation:
   * Desired value  two’s complement and convert to hex  split into 2 bytes  convert to decimal
   * Velocity = -200 = 0xFF38 = [0xFF] [0x38] = [255] [56]
   * Radius = 500 = 0x01F4 = [0x01] [0xF4] = [1] [244]
   */
  ROOMBA_DRIVE = 137,

  /**
   * Motors
   * Opcode: 138
   * Data Bytes: 1
   *
   * This command lets you control the forward and backward motion of Roomba’s
   * main brush, side brush, and vacuum independently. Motor velocity cannot be
   * controlled with this command, all motors will run at maximum speed when
   * enabled. The main brush and side brush can be run in either direction. The
   * vacuum only runs forward.
   * Serial sequence: [138] [Motors]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Bits 0-2: 0 = off, 1 = on at 100% pwm duty cycle
   *   - Bits 3 & 4: 0 = motor’s default direction, 1 = motor’s opposite
   *     direction. Default direction for the side brush is counterclockwise.
   * Default direction for the main brush/flapper is inward.
   * | Bit   |  7  |  6  |  5 | 4 | 3 | 2 | 1 | 0 |
   * |-------|-----|-----|----|---|---|---|---|---|
   * | Value | Reserved |  |  | Main Brush Direction | Side Brush Clockwise? | Main Brush | Vacuum | Side Brush |
   *
   * Example:
   * To turn on the main brush inward and the side brush clockwise, send: [138]
   * [13]
   */
  ROOMBA_MOTORS = 138,

  /**
   * LEDs
   * Opcode: 139
   * Data Bytes: 3
   *
   * This command controls the LEDs common to all models of Roomba 600. The
   * power LED is specified by two data bytes: one for the color and the other
   * for the intensity.
   *   - Serial sequence: [139] [LED Bits] [Power Color] [Power Intensity]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - LED Bits (0 – 255)
   *     - Home and Spot use green LEDs: 0 = off, 1 = on
   *     - Check Robot uses an orange LED.
   *     - Debris uses a blue LED.
   *     - Power uses a bicolor (red/green) LED.
   * The intensity and color of this LED can be controlled with 8-bit
   * resolution.
   *
   * LED Bits (0-255)
   *
   * | Bit   | 7        | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   * |-------|----------|---|---|---|---|---|---|---|
   * | Value | Reserved |   |   |   | Check Robot | Dock | Spot | Debris |
   *
   * Power LED Color (0 – 255)
   * 0 = green, 255 = red. Intermediate values are intermediate colors (orange,
   * yellow, etc).
   *
   * Power LED Intensity (0 – 255)
   * 0 = off, 255 = full intensity. Intermediate values are intermediate
   * intensities.
   *
   * Example:
   * To turn on the Home LED and light the Power LED green at half intensity,
   * send the serial byte sequence [139] [4] [0] [128].
   */
  ROOMBA_LEDS = 139,

  /**
   * Song
   * Opcode: 140
   * Data Bytes: 2N+2, where N is the number of notes in the song
   *
   * This command lets you specify up to four songs to the OI that you can play
   * at a later time. Each song is associated with a song number. The Play
   * command uses the song number to identify your song selection.
   * Each song can contain up to sixteen notes. Each note is associated with a
   * note number that uses MIDI note definitions and a duration that is
   * specified in fractions of a second. The number of data bytes varies,
   * depending on the length of the song specified. A one note song is
   * specified by four data bytes. For each additional note within a song, add
   * two data bytes.
   *   - Serial sequence: [140] [Song Number] [Song Length] [Note Number 1]
   *     [Note Duration 1] [Note Number 2] [Note Duration 2], etc.
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Song Number (0 – 4)
   *     The song number associated with the specific song. If you send a
   *     second Song command, using the same song number, the old song is
   *     overwritten.
   *   - Song Length (1 – 16)
   *     The length of the song, according to the number of musical notes
   *     within the song.
   *   - Song data bytes 3, 5, 7, etc.: Note Number (31 – 127)
   *     The pitch of the musical note Roomba will play, according to the MIDI
   *     note numbering scheme. The lowest musical note that Roomba will play
   *     is Note #31. Roomba considers all musical notes outside the range of
   *     31 – 127 as rest notes, and will make no sound during the duration of
   *     those notes.
   *   - Song data bytes 4, 6, 8, etc.: Note Duration (0 – 255)
   *     The duration of a musical note, in increments of 1/64th of a second.
   *     Example: a half-second long musical note has a duration value of 32.
   *
   * | Number | Note | Freq.  | Number | Note | Freq.  | Number| Note | Freq.  |
   * |-------:|:----:|:------:|-------:|:----:|:------:|------:|:----:|:------:|
   * | 31     | G    | 49.0   | 58     | A#   | 233.1  | 85    | C#   | 1108.8 |
   * | 32     | G#   | 51.9   | 59     | B    | 246.9  | 86    | D    | 1174.7 |
   * | 33     | A    | 55.0   | 60     | C    | 261.6  | 87    | D#   | 1244.5 |
   * | 34     | A#   | 58.3   | 61     | C#   | 277.2  | 88    | E    | 1318.5 |
   * | 35     | B    | 61.7   | 62     | D    | 293.7  | 89    | F    | 1396.9 |
   * | 36     | C    | 65.4   | 63     | D#   | 311.1  | 90    | F#   | 1480.0 |
   * | 37     | C#   | 69.3   | 64     | E    | 329.6  | 91    | G    | 1568.0 |
   * | 38     | D    | 73.4   | 65     | F    | 349.2  | 92    | G#   | 1661.3 |
   * | 39     | D#   | 77.8   | 66     | F#   | 370.0  | 93    | A    | 1760.0 |
   * | 40     | E    | 82.4   | 67     | G    | 392.0  | 94    | A#   | 1864.7 |
   * | 41     | F    | 87.3   | 68     | G#   | 415.3  | 95    | B    | 1975.6 |
   * | 42     | F#   | 92.5   | 69     | A    | 440.0  | 96    | C    | 2093.1 |
   * | 43     | G    | 98.0   | 70     | A#   | 466.2  | 97    | C#   | 2217.5 |
   * | 44     | G#   | 103.8  | 71     | B    | 493.9  | 98    | D    | 2349.4 |
   * | 45     | A    | 110.0  | 72     | C    | 523.3  | 99    | D#   | 2489.1 |
   * | 46     | A#   | 116.5  | 73     | C#   | 554.4  | 100   | E    | 2637.1 |
   * | 47     | B    | 123.5  | 74     | D    | 587.3  | 101   | F    | 2793.9 |
   * | 48     | C    | 130.8  | 75     | D#   | 622.3  | 102   | F#   | 2960.0 |
   * | 49     | C#   | 138.6  | 76     | E    | 659.3  | 103   | G    | 3136.0 |
   * | 50     | D    | 146.8  | 77     | F    | 698.5  | 104   | G#   | 3322.5 |
   * | 51     | D#   | 155.6  | 78     | F#   | 740.0  | 105   | A    | 3520.1 |
   * | 52     | E    | 164.8  | 79     | G    | 784.0  | 106   | A#   | 3729.4 |
   * | 53     | F    | 174.6  | 80     | G#   | 830.6  | 107   | B    | 3951.2 |
   * | 54     | F#   | 185.0  | 81     | A    | 880.0  |       |      |        |
   * | 55     | G    | 196.0  | 82     | A#   | 932.4  |       |      |        |
   * | 56     | G#   | 207.7  | 83     | B    | 987.8  |       |      |        |
   * | 57     | A    | 220.0  | 84     | C    | 1046.5 |       |      |        |
   * | 58     | A#   | 233.1  | 85     | C#   | 1108.8 |       |      |        |
   */
  ROOMBA_SONG = 140,

  /**
   * Play
   * Opcode: 141
   * Data Bytes: 1
   *
   * This command lets you select a song to play from the songs added to Roomba
   * using the Song command.
   * You must add one or more songs to Roomba using the Song command in order
   * for the Play command to work.
   *   - Serial sequence: [141] [Song Number]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Song Number (0 – 4)
   * The number of the song Roomba is to play.
   */
  ROOMBA_PLAY = 141,

  /**
   * Sensors
   * Opcode: 142
   * Data Bytes: 1
   *
   * This command requests the OI to send a packet of sensor data bytes. There
   * are 58 different sensor data packets. Each provides a value of a specific
   * sensor or group of sensors.
   * For more information on sensor packets, refer to the next section, “Roomba
   * Open Interface Sensors Packets”.
   *   - Serial sequence: [142] [Packet ID]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   * Identifies which of the 58 sensor data packets should be sent back by the
   * OI. A value of 6 indicates a packet with all of the sensor data. Values of
   * 0 through 5 indicate specific subgroups of the sensor data.
   */
  ROOMBA_SENSORS = 142,

  /**
   * Seek Dock
   * Opcode: 143
   * Data Bytes: 0
   *
   * This command directs Roomba to drive onto the dock the next time it
   * encounters the docking beams. This is the same as pressing Roomba’s Dock
   * button, and will pause a cleaning cycle if one is already in progress.
   *   - Serial sequence: [143]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Passive
   */
  ROOMBA_SEEK_DOCK = 143,

  /**
   * PWM Motors
   * Opcode: 144
   * Data Bytes: 3
   *
   * This command lets you control the speed of Roomba’s main brush, side
   * brush, and vacuum independently. With each data byte, you specify the duty
   * cycle for the low side driver (max 128). For example, if you want to
   * control a motor with 25% of battery voltage, choose a duty cycle of
   * 128 * 25% = 32. The main brush and side brush can be run in either
   * direction. The vacuum only runs forward.
   * Positive speeds turn the motor in its default (cleaning) direction.
   * Default direction for the side brush is counterclockwise. Default
   * direction for the main brush/flapper is inward.
   * Serial sequence: [144] [Main Brush PWM] [Side Brush PWM] [Vacuum PWM]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Main Brush and Side Brush duty cycle (-127 – 127)
   *   - Vacuum duty cycle (0 – 127)
   */
  ROOMBA_PWM_MOTORS = 144,

  /**
   * Drive Direct
   * Opcode: 145
   * Data Bytes: 4
   *
   * This command lets you control the forward and backward motion of Roomba’s
   * drive wheels independently. It takes four data bytes, which are
   * interpreted as two 16-bit signed values using two’s complement. The first
   * two bytes specify the velocity of the right wheel in millimeters per
   * second (mm/s), with the high byte sent first. The next two bytes specify
   * the velocity of the left wheel, in the same format. A positive velocity
   * makes that wheel drive forward, while a negative velocity makes it drive
   * backward.
   *   - Serial sequence: [145] [Right velocity high byte]
   *     [Right velocity low byte] [Left velocity high byte]
   *     [Left velocity low byte]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Right wheel velocity (-500 – 500 mm/s)
   *   - Left wheel velocity (-500 – 500 mm/s)
   */
  ROOMBA_DRIVE_DIRECT = 145,

  /**
   * Drive PWM
   * Opcode: 146
   * Data Bytes: 4
   *
   * This command lets you control the raw forward and backward motion of
   * Roomba’s drive wheels independently. It takes four data bytes, which are
   * interpreted as two 16-bit signed values using two’s complement. The first
   * two bytes specify the PWM of the right wheel, with the high byte sent
   * first. The next two bytes specify the PWM of the left wheel, in the same
   * format. A positive PWM makes that wheel drive forward, while a negative
   * PWM makes it drive backward.
   *   - Serial sequence: [146] [Right PWM high byte] [Right PWM low byte]
   *     [Left PWM high byte] [Left PWM low byte]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Right wheel PWM (-255 – 255)
   *   - Left wheel PWM (-255 – 255)
   */
  ROOMBA_DRIVE_PWM = 146,

  #if ROOMBA_INTERFACE_VERSION==1

  /**
   *
   */
  DIGITAL_OUTPUTS = 147,

  #endif

  /**
   * Stream
   * Opcode: 148
   * Data Bytes: N + 1, where N is the number of packets
   * requested.
   *
   * This command starts a stream of data packets. The list of packets
   * requested is sent every 15 ms, which is the rate Roomba uses to update
   * data. This method of requesting sensor data is best if you are controlling
   * Roomba over a wireless network (which has poor real-time characteristics)
   * with software running on a desktop computer.
   *   - Serial sequence: [148] [Number of packets] [Packet ID 1] [Packet ID 2] *     [Packet ID 3] etc.
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   * The format of the data returned is:
   * [19][N-bytes][Packet ID 1][Packet 1 data…][Packet ID 2][Packet 2 data…]
   * [Checksum]
   * N-bytes is the number of bytes between the n-bytes byte and the checksum.
   * The checksum is a 1-byte value. It is the 8-bit complement of all of the
   * bytes in the packet, excluding the checksum itself. That is, if you add
   * all of the bytes in the packet, including the checksum, the low byte of
   * the result will be 0.
   * Example:
   * To get data from Roomba's left cliff signal (packet 29) and virtual wall
   * sensor (packet 13), send the following command string to Roomba:
   * [148] [2] [29] [13]
   * @note
   * The left cliff signal is a 2-byte packet and the virtual wall is a 1-byte
   * packet.
   * Roomba starts streaming data that looks like this:
   * 19 5 29 2 25 13 0 163 header n-bytes packet ID 1 Packet data 1 (2 bytes)
   * packet ID 2 packet data 2 (1 byte) Checksum
   * @note
   * Checksum computation: (19 + 5 + 29 + 2 + 25 + 13 + 0 + 163) = 256 and
   * (256 & 0xFF) = 0.
   * In the above stream segment, Roomba’s left cliff signal value was 549
   * (0x0225) and there was no virtual wall signal.
   * It is up to you not to request more data than can be sent at the current
   * baud rate in the 15 ms time slot.
   * For example, at 115200 baud, a maximum of 172 bytes can be sent in 15 ms:
   * 15 ms / 10 bits (8 data + start + stop) * 115200 = 172.8
   *
   * If more data is requested, the data stream will eventually become
   * corrupted. This can be confirmed by checking the checksum.
   * The header byte and checksum can be used to align your receiving program
   * with the data. All data chunks start with 19 and end with the 1-byte
   * checksum.
   */
  ROOMBA_STREAM = 148,

  /**
   * Query List
   * Opcode: 149
   * Data Bytes: N + 1, where N is the number of packets requested.
   *
   * This command lets you ask for a list of sensor packets. The result is
   * returned once, as in the Sensors command. The robot returns the packets in
   * the order you specify.
   *   - Serial sequence: [149][Number of Packets][Packet ID 1]
   *     [Packet ID 2]...[Packet ID N]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes modes to: No Change
   * Example:
   * To get the state of the bumpers and the virtual wall sensor, send the
   * following sequence: [149] [2] [7] [13]
   */
  ROOMBA_QUERY_LIST = 149,

  /**
   * Pause/Resume Stream
   * Opcode: 150
   * Data Bytes: 1
   *
   * This command lets you stop and restart the steam without clearing the list
   * of requested packets.
   *   - Serial sequence: [150][Stream State]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes modes to: No Change
   *   - Range: 0-1
   * An argument of 0 stops the stream without clearing the list of requested
   * packets. An argument of 1 starts the stream using the list of packets last
   * requested.
   */
  ROOMBA_PAUSE_RESUME_STREAM = 150,



  #if ROOMBA_INTERFACE_VERSION==1

  /**
   * Send IR
   * Opcode: 151
   * Data Bytes: 1
   *
   * This command sends the requested byte out of low side driver 1 (pin 23 on
   * the Cargo Bay Connector), using the format expected by iRobot Create’s IR
   * receiver. You must use a preload resistor (suggested value: 100 ohms) in
   * parallel with the IR LED and its resistor in order turn it on.
   *   - Serial sequence: [151][Byte Value]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Send IR data byte 1: Byte value to send (0 - 255)
   *
   * @deprecated
   */
  SEND_IR = 151,

  /**
   * Script
   * Opcode: 152
   * Data Bytes: N + 1 where N is the number of bytes in the script.
   *
   * This command specifies a script to be played later. A script consists of
   * OI commands and can be up to 100 bytes long. There is no flow control, but
   * “wait” commands (see below) cause Create to hold its current state until
   * the specified event is detected.
   *   - Serial sequence: [152] [Script Length] [Opcode 1] [Opcode 2]
   *     [Opcode 3] etc.
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Script data byte 1: Script Length (0 – 100) Specifies the length of
   *     the script in terms of the number of commands. Specify a length of 0
   *     to clear the current script.
   *   - Script data bytes 2 and above: Open Interface commands and data bytes
   *
   * Tip: To make a script loop forever, use Play Script (153) as the last
   * command.
   * Example Scripts:
   * Drive 40 cm and stop:
   * 152 13 137 1 44 128 0 156 1 144 137 0 0 0 0
   * Toggle led on bump:
   * 152 17 158 5 158 251 139 2 0 0 158 5 158 251 139 0
   * 0 0 153
   * Drive in a square:
   * 152 17 137 1 44 128 0 156 1 144 137 1 44 0 1 157
   * 0 90 153
   *
   * @deprecated
   */
  SCRIPT = 152,

  /**
   * Play Script
   * Opcode: 153
   * Data Bytes: 0
   *
   * This command loads a previously defined OI script into the serial input
   * queue for playback.
   *   - Serial sequence: [153]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *
   * @deprecated
   */
  PLAY_SCRIPT = 153,

  /**
   * Show Script
   * Opcode: 154
   * Data Bytes: 0
   *
   * This command returns the values of a previously stored script, starting
   * with the number of bytes in the script and followed by the script’s
   * commands and data bytes. It first halts the sensor stream, if one has been
   * started with a Stream or Pause/Resume Stream command. To restart the
   * stream, send Pause/Resume Stream (opcode 150).
   * • Serial sequence: [154]
   * • Available in modes: Passive, Safe, or Full
   * • Changes mode to: No Change
   *
   * ### Wait Commands
   * The following commands cause Create to wait for a specific time, distance,
   * angle of rotation, or event to occur. While it is waiting, Create does not
   * change its state, nor does it react to any inputs, serial or otherwise.
   * These commands are intended for use in scripting only.
   *
   * @deprecated
   */
  SHOW_SCRIPT = 154,

  /**
   * Wait Time
   * Opcode: 155
   * Data Bytes: 1
   *
   * This command causes Create to wait for the specified time. During this
   * time, Create’s state does not change, nor does it react to any inputs,
   * serial or otherwise.
   *   - Serial sequence: [155] [time]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Wait Time data byte 1: Time (0 - 255)
   * Specifies time to wait in tenths of a second with a resolution of 15 ms.
   *
   * @deprecated
   */
  WAIT_TIME = 155,

  /**
   * Wait Distance
   * Opcode: 156
   * Data Bytes: 2
   *
   * This command causes iRobot Create to wait until it has traveled the
   * specified distance in mm. When Create travels forward, the distance is
   * incremented. When Create travels backward, the distance is decremented. If
   * the wheels are passively rotated in either direction, the distance is
   * incremented. Until Create travels the specified distance, its state does
   * not change, nor does it react to any inputs, serial or otherwise.
   * @note This command resets the distance variable that is returned in
   * Sensors packets 19, 2 and 6.
   *   - Serial sequence: [156] [Distance high byte] [Distance low byte]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Wait Distance data bytes 1-2: 16-bit signed distance in mm, high byte
   *     first (-32767 -32768)
   *
   * @deprecated
   */
  WAIT_DISTANCE = 156,

  /**
   * Wait Angle
   * Opcode: 157
   * Data Bytes: 2
   *
   * This command causes Create to wait until it has rotated through specified
   * angle in degrees. When Create turns counterclockwise, the angle is
   * incremented. When Create turns clockwise, the angle is decremented. Until
   * Create turns through the specified angle, its state does not change, nor
   * does it react to any inputs, serial or otherwise.
   * @note This command resets the angle variable that is returned in Sensors
   * packets 20, 2 and 6.
   *   - Serial sequence: [157] [Angle high byte] [Angle low byte]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Wait Angle data bytes 1-2: 16-bit signed angle in degrees, high byte
   *     first (-32767 -32768)
   *
   * @deprecated
   */
  WAIT_ANGLE = 157,

  /**
   * Wait Event
   * Opcode: 158
   * Data Bytes: 1 signed
   *
   * This command causes Create to wait until it detects the specified event.
   * Until the specified event is detected, Create’s state does not change, nor
   * does it react to any inputs, serial or otherwise.
   *   - Serial sequence: [158] [Event number]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Wait Event data byte 1: Signed event number (1 to 20 and -1 to -20)
   *
   * To wait for the inverse of an event, send the negative of its number using
   * two’s complement notation. For example, to wait for no bumps, send the
   * serial byte sequence [158] [-5], which is equivalent to [158] [251].
   *
   * Wait Event: Unsigned Equivalent of Inverse
   * | Event             | Number | Unsigned Equivalent of Inverse |
   * |-------------------|--------|--------------------------------|
   * | Wheel Drop        | 1      | 255                            |
   * | Front Wheel Drop  | 2      | 254                            |
   * | Left Wheel Drop   | 3      | 253                            |
   * | Right Wheel Drop  | 4      | 252                            |
   * | Bump              | 5      | 251                            |
   * | Left Bump         | 6      | 250                            |
   * | Right Bump        | 7      | 249                            |
   * | Virtual Wall      | 8      | 248                            |
   * | Wall              | 9      | 247                            |
   * | Cliff             | 10     | 246                            |
   * | Left Cliff        | 11     | 245                            |
   * | Front Left Cliff  | 12     | 244                            |
   * | Front Right Cliff | 13     | 243                            |
   * | Right Cliff       | 14     | 242                            |
   * | Home Base         | 15     | 241                            |
   * | Advance Button    | 16     | 240                            |
   * | Play Button       | 17     | 239                            |
   * | Digital Input 0   | 18     | 238                            |
   * | Digital Input 1   | 19     | 237                            |
   * | Digital Input 2   | 20     | 236                            |
   * | Digital Input 3   | 21     | 235                            |
   * | OI Mode = Passive | 22     | 234                            |
   *
   * @deprecated
   */
  WAIT_EVENT = 158,

  #endif
  #if ROOMBA_INTERFACE_VERSION==2
  /**
   * Scheduling LEDS
   * Opcode: 162
   * Data Bytes: 2
   *
   * This command controls the state of the scheduling LEDs present on the
   * Roomba 560 and 570.
   *   - Serial sequence: [162] [Weekday LED Bits][Scheduling LED Bits]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Weekday LED Bits (0 – 255)
   *   - Scheduling LED Bits (0 – 255)
   *   - All use red LEDs: 0 = off, 1 = on
   *
   * ### Weekday LED Bits
   * | Bit   | 7        | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
   * |-------|----------|-----|-----|-----|-----|-----|-----|-----|
   * | Value | Reserved | Sat | Fri | Thu | Wed | Tue | Mon | Sun |
   *
   * ### Scheduling LED Bits
   * | Bit   | 7        | 6        | 5     | 4  | 3  | 2         | 1 | 0 |
   * |-------|----------|----------|-------|----|----|-----------|---|---|
   * | Value | Reserved | Schedule | Clock | AM | PM | Colon (:) |   |   |
   */
  ROOMBA_SCHEDULING_LEDS = 162,

  /**
   * Digit LEDs Raw
   * Opcode: 163
   * Data Bytes: 4
   *
   * This command controls the four 7 segment displays on the Roomba 560 and
   * 570.
   *   - Serial sequence: [163] [Digit 3 Bits] [Digit 2 Bits] [Digit 1 Bits]
   *     [Digit 0 Bits]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Digit N Bits (0 – 255)
   *   - All use red LEDs: 0 = off, 1 = on. Digits are ordered from left to
   *     right on the robot 3,2,1,0.
   *
   * Digit N Bits
   * | Bit   | 7        | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   * |-------|----------|---|---|---|---|---|---|---|
   * | Value | Reserved | G | F | E | D | C | B | A |
   */
  ROOMBA_DIGIT_LEDS_RAW = 163,

  /**
   * Digit LEDs ASCII
   * Opcode: 164
   * Data Bytes: 4
   *
   * This command controls the four 7 segment displays on the Roomba 560 and
   * 570 using ASCII character codes. Because a 7 segment display is not
   * sufficient to display alphabetic characters properly, all characters are
   * an approximation, and not all ASCII codes are implemented.
   *   - Serial sequence: [164] [Digit 3 ASCII] [Digit 2 ASCII] [Digit 1 ASCII]
   *     [Digit 0 ASCII]
   *   - Available in modes: Safe or Full
   *   - Changes mode to: No Change
   *   - Digit N ASCII (32 – 126)
   *   - All use red LEDs. Digits are ordered from left to right on the robot
   *     3,2,1,0.
   * Example:
   * To write ABCD to the display, send the serial byte sequence: [164] [65]
   * [66] [67] [68]
   *
   * Table of ASCII codes:
   * | Code | Display | Code   | Display | Code   | Display | Code   | Display |
   * |------|---------|--------|---------|--------|---------|--------|---------|
   * | 32   |         | 53     | 5       | 70,102 | F       | 86,118 | V       |
   * | 33   | !       | 54     | 6       | 71,103 | G       | 87,119 | W       |
   * | 34   | ”       | 55     | 7       | 72,104 | H       | 88,120 | X       |
   * | 35   | #       | 56     | 8       | 73,105 | I       | 89,121 | Y       |
   * | 37   | %       | 57     | 9       | 74,106 | J       | 90,122 | Z       |
   * | 38   | &       | 58     | :       | 75,107 | K       | 91,40  | [       |
   * | 39   | ’       | 59     | ;       | 76,108 | L       | 92     | \       |
   * | 44   | ,       | 60     | ¡       | 77,109 | M       | 93,41  | ]       |
   * | 45   | -       | 61     | =       | 78,110 | N       | 94     | ∧       |
   * | 46   | .       | 62     | ¿       | 79,111 | O       | 95     |         |
   * | 47   | /       | 63     | ?       | 80,112 | P       | 96     | ‘       |
   * | 48   | 0       | 65,97  | A       | 81,113 | Q       | 123    | {       |
   * | 49   | 1       | 66,98  | B       | 82,114 | R       | 124    | —       |
   * | 50   | 2       | 67,99  | C       | 83,36,115 | S    | 125    | }       |
   * | 51   | 3       | 68,100 | D       | 84,116 | T       | 126    | ∼       |
   * | 52   | 4       | 69,101 | E       | 85,117 | U       |        |         |
   */
  ROOMBA_DIGIT_LEDS_ASCII = 164,

  /**
   * Buttons
   * Opcode: 165
   * Data Bytes: 1
   *
   * This command lets you push Roomba’s buttons. The buttons will
   * automatically release after 1/6th of a second.
   *   - Serial sequence: [165] [Buttons]
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: No Change
   *   - Buttons (0-255) 1 = Push Button, 0 = Release Button
   * Buttons
   * | Bit   | 7     | 6        | 5   | 4    | 3      | 2    | 1    | 0     |
   * |-------|-------|----------|-----|------|--------|------|------|-------|
   * | Value | Clock | Schedule | Day | Hour | Minute | Dock | Spot | Clean |
   */
  ROOMBA_BUTTONS_CMD = 165,

  /**
   * This command sends Roomba a new schedule. To disable scheduled cleaning,
   * send all 0s.
   *   - Serial sequence: [167] [Days] [Sun Hour] [Sun Minute] [Mon Hour] [Mon Minute] [Tue Hour] [Tue
  Minute] [Wed Hour] [Wed Minute] [Thu Hour] [Thu Minute] [Fri Hour] [Fri Minute] [Sat Hour] [Sat
  Minute]
   *   - Available in modes: Passive, Safe, or Full.
   *   - If Roomba’s schedule or clock button is pressed, this command will be ignored.
   *   - Changes mode to: No change
   *   - Times are sent in 24 hour format. Hour (0-23) Minute (0-59)
  Days
  Bit 7 6 5 4 3 2 1 0
  Value Reserved Sat Fri Thu Wed Tue Mon Sun
  Example:
  To schedule the robot to clean at 3:00 PM on Wednesdays and 10:36 AM on Fridays, send: [167] [40] [0]
  [0] [0] [0] [0] [0] [15] [0] [0] [0] [10] [36] [0] [0]
  To disable scheduled cleaning, send: [167] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0]
  */
  ROOMBA_SCHEDULE = 167,

  /**
   * Set Day/Time
   * Opcode: 168
   * Data Bytes: 3
   *
   * This command sets Roomba’s clock.
   *   - Serial sequence: [168] [Day] [Hour] [Minute]
   *   - Available in modes: Passive, Safe, or Full.
   *   - If Roomba’s schedule or clock button is pressed, this command will be
   *     ignored.
   *   - Changes mode to: No change
   *   - Time is sent in 24 hour format. Hour (0-23) Minute (0-59)
   * | Code | Day       |
   * |------|-----------|
   * | 0    | Sunday    |
   * | 1    | Monday    |
   * | 2    | Tuesday   |
   * | 3    | Wednesday |
   * | 4    | Thursday  |
   * | 5    | Friday    |
   * | 6    | Saturday  |
   */
  ROOMBA_SET_DAY_TIME = 168,

  /**
   * Stop
   * Opcode: 173
   * Data Bytes: 0
   *
   * This command stops the OI. All streams will stop and the robot will no
   * longer respond to commands.
   * Use this command when you are finished working with the robot.
   *   - Serial sequence: [173].
   *   - Available in modes: Passive, Safe, or Full
   *   - Changes mode to: Off. Roomba plays a song to acknowledge it is exiting
   *     the OI.
   */
  ROOMBA_STOP = 173,
  #endif

} ROOMBA_OP_CODE;


typedef enum {
  ROOMBA_IR_CHARACTER_LEFT = 129,
  ROOMBA_IR_CHARACTER_FORWARD = 130,
  ROOMBA_IR_CHARACTER_RIGHT = 131,
  ROOMBA_IR_CHARACTER_SPOT = 132,
  ROOMBA_IR_CHARACTER_MAX = 133,
  ROOMBA_IR_CHARACTER_SMALL = 134,
  ROOMBA_IR_CHARACTER_MEDIUM = 135,
  ROOMBA_IR_CHARACTER_LARGE_CLEAN = 136,
  ROOMBA_IR_CHARACTER_STOP_1 = 137,
  ROOMBA_IR_CHARACTER_POWER = 138,
  ROOMBA_IR_CHARACTER_ARC_LEFT = 139,
  ROOMBA_IR_CHARACTER_ARC_RIGHT = 140,
  ROOMBA_IR_CHARACTER_STOP_2 = 141,
  ROOMBA_IR_CHARACTER_DOWNLOAD = 142,
  ROOMBA_IR_CHARACTER_SEEK_DOCK = 143,
  ROOMBA_IR_CHARACTER_RESERVED_1 = 240,
  ROOMBA_IR_CHARACTER_RED_BUOY = 248,
  ROOMBA_IR_CHARACTER_GREEN_BUOY = 244,
  ROOMBA_IR_CHARACTER_FORCE_FIELD = 242,
  ROOMBA_IR_CHARACTER_RED_BUOY_AND_GREEN_BUOY = 252,
  ROOMBA_IR_CHARACTER_RED_BUOY_AND_FORCE_FIELD = 250,
  ROOMBA_IR_CHARACTER_GREEN_BUOY_AND_FORCE_FIELD = 246,
  ROOMBA_IR_CHARACTER_RED_BUOY_GREEN_BUOY_AND_FORCE_FIELD = 254,
  ROOMBA_IR_CHARACTER_RESERVED_2 = 160,
  ROOMBA_IR_CHARACTER_VIRTUAL_WALL = 162,
  /*
  Roomba 600 Auto-on Virtual Wall
  0LLLL0BB LLLL = Auto-on virtual
  wall ID (assigned
  automatically by
  Roomba 660 robots)
  1-10: Valid ID
  11: Unbound
  12-15: Reserved
  BB = Beam set
00 = Fence
01 = Force Field
10 = Green Buoy
11 = Red Buoy
  */
} ROOMBA_IR_CHARACTER_CODE;



/**
 * Roomba Open Interface Sensor Packets
 * Roomba sends back one of 58 different sensor data packets, depending on the
 * value of the packet data byte, when responding to a Sensors command, Query
 * List command, or Stream command’s request for a packet of sensor data bytes.
 * Some packets contain groups of other packets. Some of the sensor data values
 * are 16 bit values.
 * Most of the packets (numbers 7 – 58) contain the value of a single sensor or
 * variable, which can be either 1 byte or 2 bytes. Two byte packets correspond
 * to 16-bit values, sent high byte first.
 * Some of the packets (0-6, 100-107) contain groups of the single-value
 * packets.
 *
 * | Group Packet ID | Packet Size | Contains Packets |
 * |-----------------|-------------|------------------|
 * | 0               | 26          | 7 - 26           |
 * | 1               | 10          | 7 - 16           |
 * | 2               | 6           | 17 - 20          |
 * | 3               | 10          | 21 - 26          |
 * | 4               | 14          | 27 - 34          |
 * | 5               | 12          | 35 - 42          |
 * | 6               | 52          | 7 - 42           |
 * | 100             | 80          | 7 - 58           |
 * | 101             | 28          | 43 - 58          |
 * | 106             | 12          | 46 - 51          |
 * | 107             | 9           | 54 - 58          |
 */
typedef enum _pv {
  /**
   * Bumps and Wheel Drops
   * Packet ID: 7
   * Data Bytes: 1, unsigned
   *
   * The state of the bumper (0 = no bump, 1 = bump) and wheel drop sensors
   * (0 = wheel raised, 1 = wheel dropped) are sent as individual bits.
   * Range: 0 – 15
   * | Bit   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   * |-------|---|---|---|---|---|---|---|---|
   * | Value | Reserved | Wheel Drop Left? | Wheel Drop Right? | Bump Left? | Bump Right? |
   */
  ROOMBA_BUMPS_WHEELDROPS  = 7,

  /**
   * Wall
   * Packet ID: 8
   * Data Bytes: 1, unsigned
   *
   * The state of the wall sensor is sent as a 1 bit value (0 = no wall,
   * 1 = wall seen).
   *
   * Range: 0 – 1
   */
  ROOMBA_WALL = 8,

  /**
   * Cliff Left
   * Packet ID: 9
   * Data Bytes: 1, unsigned
   *
   * The state of the cliff sensor on the left side of Roomba is sent as a 1
   * bit value (0 = no cliff, 1 = cliff).
   *
   * Range: 0 – 1
   */
  ROOMBA_CLIFF_LEFT = 9,

  /**
   * Cliff Front Left
   * Packet ID: 10
   * Data Bytes: 1, unsigned
   *
   * The state of the cliff sensor on the front left of Roomba is sent as a 1
   * bit value (0 = no cliff, 1 = cliff).
   *
   * Range: 0 – 1
   */
  ROOMBA_CLIFF_FRONT_LEFT = 10,

  /**
   * Cliff Front Right
   * Packet ID: 11
   * Data Bytes: 1, unsigned
   *
   * The state of the cliff sensor on the front right of Roomba is sent as a 1
   * bit value (0 = no cliff, 1 = cliff)
   *
   * Range: 0 – 1
   */
  ROOMBA_CLIFF_FRONT_RIGHT = 11,

  /**
   * Cliff Right
   * Packet ID: 12
   * Data Bytes: 1, unsigned
   *
   * The state of the cliff sensor on the right side of Roomba is sent as a 1
   * bit value (0 = no cliff, 1 = cliff)
   *
   * Range: 0 – 1
   */
  ROOMBA_CLIFF_RIGHT = 12,

  /**
   * Virtual Wall
   * Packet ID: 13
   * Data Bytes: 1, unsigned
   *
   * The state of the virtual wall detector is sent as a 1 bit value (0 = no
   * virtual wall detected, 1 = virtual wall detected).
   *
   * Range: 0 – 1
   */
  ROOMBA_VIRTUAL_WALL = 13,

  /**
   * Wheel Overcurrents
   * Packet ID: 14
   * Data Bytes: 1, unsigned
   *
   * The state of the four wheel overcurrent sensors are sent as individual
   * bits (0 = no overcurrent, 1 = overcurrent). There is no overcurrent sensor
   * for the vacuum on Roomba 600.
   *
   * Range: 0 – 31
   *
   * | Bit   | 7        | 6          | 5           | 4    | 3     | 2 | 1 | 0 |
   * |-------|----------|------------|-------------|------|-------|---|---|---|
   * | Value | Reserved | Left Wheel | Right Wheel | Main | Brush | Reserved | Side Brush |
   */
  ROOMBA_OVERCURRENTS = 14,

  /**
   * Dirt Detect
   * Packet IDs: 15
   * Data Bytes: 1
   *
   * The level of the dirt detect sensor.
   *
   * Range: 0-255
   */
  ROOMBA_DIRT_DETECT = 15,

  /**
   * Unused Byte
   * Packet IDs: 16
   * Data Bytes: 1
   *
   * Unused bytes: One unused byte is sent after the dirt detect byte when the
   * requested packet is 0, 1, or 6. The value of the unused byte is always 0.
   *
   * Range: 0
   */
  ROOMBA_UNUSED_1 = 16,

  /**
   * Infrared Character Omni
   * Packet ID: 17
   * Data Bytes: 1, unsigned
   *
   * This value identifies the 8-bit IR character currently being received by
   * Roomba’s omnidirectional receiver. A value of 0 indicates that no
   * character is being received. These characters include those sent by the
   * Roomba Remote, Dock, Virtual Walls, Create robots using the Send-IR
   * command, and user-created devices.
   *
   * Range: 0 – 255
   */
  ROOMBA_IR_OPCODE = 17,

  /**
   * Buttons
   * Packet ID: 18
   * Data Bytes: 1, unsigned
   *
   * The state of the Roomba buttons are sent as individual bits (0 = button
   * not pressed, 1 = button pressed). The day, hour, minute, clock, and
   * scheduling buttons that exist only on Roomba 560 and 570 will always
   * return 0 on a Roomba 510 or 530 robot.
   *
   * Range: 0 – 255
   *
   * | Bit   | 7     | 6        | 5   | 4    | 3      | 2    | 1    | 0     |
   * |-------|-------|----------|-----|------|--------|------|------|-------|
   * | Value | Clock | Schedule | Day | Hour | Minute | Dock | Spot | Clean |
   */
  ROOMBA_BUTTONS_PKT = 18,

  /**
   * Distance
   * Packet ID: 19
   * Data Bytes: 2, signed
   *
   * The distance that Roomba has traveled in millimeters since the distance it
   * was last requested is sent as a signed 16-bit value, high byte first. This
   * is the same as the sum of the distance traveled by both wheels divided by
   * two. Positive values indicate travel in the forward direction; negative
   * values indicate travel in the reverse direction. If the value is not
   * polled frequently enough, it is capped at its minimum or maximum.
   *
   * Range: -32768 – 32767
   *
   * @note Create 2 and Roomba 500/600 firmware versions prior to 3.3.0 return
   * an incorrect value for sensors measured in millimeters. To determine the
   * firmware version on your robot, send a 7 via the serial port to reset it.
   * The robot will print a long welcome message which will include the
   * firmware version, for example: r3_robot/tags/release-3.3.0.
   */
  ROOMBA_DISTANCE = 19,

  /**
   * Angle
   * Packet ID: 20
   * Data Bytes: 2, signed
   *
   * The angle in degrees that Roomba has turned since the angle was last
   * requested is sent as a signed 16-bit value, high byte first.
   * Counter-clockwise angles are positive and clockwise angles are negative.
   * If the value is not polled frequently enough, it is capped at its minimum
   * or maximum.
   *
   * Range: -32768 – 32767
   */
  ROOMBA_ANGLE = 20,

  /**
   * Charging State
   * Packet ID: 21
   * Data Bytes: 1, unsigned
   *
   * This code indicates Roomba’s current charging state.
   *
   * Range: 0 – 5
   *
   * | Code | Charging State           |
   * |------|--------------------------|
   * | 0    | Not charging             |
   * | 1    | Reconditioning Charging  |
   * | 2    | Full Charging            |
   * | 3    | Trickle Charging         |
   * | 4    | Waiting                  |
   * | 5    | Charging Fault Condition |
   */
  ROOMBA_CHARGING_STATE = 21,

  /**
   * Voltage
   * Packet ID: 22
   * Data Bytes: 2, unsigned
   *
   * This code indicates the voltage of Roomba’s battery in millivolts (mV).
   *
   * Range: 0 – 65535 mV
   */
  ROOMBA_VOLTAGE = 22,

  /**
   * Current
   * Packet ID: 23
   * Data Bytes: 2, signed
   *
   * The current in milliamps (mA) flowing into or out of Roomba’s battery.
   * Negative currents indicate that the current is flowing out of the battery,
   * as during normal running. Positive currents indicate that the current is
   * flowing into the battery, as during charging.
   *
   * Range: -32768 – 32767 mA
   */
  ROOMBA_CURRENT = 23,

  /**
   * Temperature
   * Packet ID: 24
   * Data Bytes: 1, signed
   *
   * The temperature of Roomba’s battery in degrees Celsius.
   *
   * Range: -128 – 127
   */
  ROOMBA_TEMPERATURE = 24,

  /**
   * Battery Charge
   * Packet ID: 25
   * Data Bytes: 2, unsigned
   *
   * The current charge of Roomba’s battery in milliamp-hours (mAh). The charge
   * value decreases as the battery is depleted during running and increases
   * when the battery is charged.
   *
   * Range: 0 – 65535 mAh
   */
  ROOMBA_BATTERY_CHARGE = 25,

  /**
   * Battery Capacity
   * Packet ID: 26
   * Data Bytes: 2, unsigned
   *
   * The estimated charge capacity of Roomba’s battery in milliamp-hours (mAh).
   *
   * Range: 0 – 65535 mAh
   */
  ROOMBA_BATTERY_CAPACITY = 26,

  /**
   * Wall Signal
   * Packet ID: 27
   * Data Bytes: 2, unsigned
   *
   * The strength of the wall signal is returned as an unsigned 16-bit value,
   * high byte first.
   *
   * Range: 0-1023.
   */
  ROOMBA_WALL_SIGNAL = 27,

  /**
   * Cliff Left Signal
   * Package ID: 28
   * Data Bytes: 2, unsigned
   *
   * The strength of the cliff left signal is returned as an unsigned 16-bit
   * value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_CLIFF_LEFT_SIGNAL = 28,

  /**
   * Cliff Front Left Signal
   * Package ID: 29
   * Data Bytes 2, unsigned
   *
   * The strength of the cliff front left signal is returned as an unsigned
   * 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_CLIFF_FRONT_LEFT_SIGNAL = 29,

  /**
   * Cliff Front Right Signal
   * Package ID: 30
   * Data Bytes 2, unsigned
   *
   * The strength of the cliff front right signal is returned as an unsigned
   * 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_CLIFF_FRONT_RIGHT_SIGNAL = 30,

  /**
   * Cliff Right Signal
   * Package ID: 31
   * Data Bytes 2, unsigned
   *
   * The strength of the cliff right signal is returned as an unsigned 16-bit
   * value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_CLIFF_RIGHT_SIGNAL = 31,

  /**
   * Unused
   * Packet ID: 32-33
   * Data Bytes, 3
   */
  ROOMBA_UNUSED_2 = 32,

  ROOMBA_UNUSED_3 = 33,

  /**
   * Charging Sources Available
   * Packet ID: 34
   * Data Bytes 1, unsigned
   *
   * Roomba’s connection to the Home Base and Internal Charger are returned as
   * individual bits, as below.
   *
   * Range: 0-3
   *
   * 1 = charging source present and powered;
   * 0 = charging source not present or not powered.
   *
   * | Bit   | 7        | 6 | 5 | 4 | 3    | 2    | 1        | 0       |
   * |-------|----------|---|---|---|------|------|----------|---------|
   * | Value | Reserved |   |   |   | Home | Base | Internal | Charger |
   */
  ROOMBA_CHARGER_AVAILABLE = 34,

  /**
   * OI Mode
   * Packet ID: 35
   * Data Bytes 1, unsigned
   *
   * The current OI mode is returned. See table below.
   *
   * Range: 0-3
   *
   * | Number | Mode    |
   * |--------|---------|
   * | 0      | Off     |
   * | 1      | Passive |
   * | 2      | Safe    |
   * | 3      | Full    |
   */
  ROOMBA_OPEN_INTERFACE_MODE = 35,

  /**
   * Song Number
   * Packet ID: 36
   * Data Bytes 1, unsigned
   *
   * The currently selected OI song is returned.
   *
   * Range: 0-15
   */
  ROOMBA_SONG_NUMBER = 36,

  /**
   * Song Playing
   * Packet ID: 37
   * Data Bytes 1, unsigned
   *
   * The state of the OI song player is returned. 1 = OI song currently
   * playing; 0 = OI song not playing.
   *
   * Range: 0-1
   */
  ROOMBA_SONG_PLAYING = 37,

  /**
   * Number of Stream
   * Packets Packet ID: 38
   * Data Bytes 1, unsigned
   *
   * The number of data stream packets is returned.
   *
   * Range: 0-108
   */
  ROOMBA_OI_STREAM_NUM_PACKETS = 38,

  /**
   * Requested Velocity
   * Packet ID: 39
   * Data Bytes 2, signed
   *
   * The velocity most recently requested with a Drive command is returned as a
   * signed 16-bit number, high byte first.
   *
   * Range: -500 - 500 mm/s
   */
  ROOMBA_VELOCITY = 39,

  /**
   * Requested Radius
   * Packet ID: 40
   * Data Bytes 2, signed
   *
   * The radius most recently requested with a Drive command is returned as a
   * signed 16-bit number, high byte first.
   *
   * Range: -32768 - 32767 mm
   *
   * @note Create 2 and Roomba 500/600 firmware versions prior to 3.3.0 return
   * an incorrect value for sensors measured in millimeters. To determine the
   * firmware version on your robot, send a 7 via the serial port to reset it.
   * The robot will print a long welcome message which will include the
   * firmware version, for example: r3_robot/tags/release-3.3.0.
   */
  ROOMBA_RADIUS = 40,

  /**
   * Requested Right Velocity
   * Packet ID: 41
   * Data Bytes 2, signed
   *
   * The right wheel velocity most recently requested with a Drive Direct
   * command is returned as a signed 16-bit number, high byte first.
   *
   * Range: -500 - 500 mm/s
   */
  ROOMBA_VELOCITY_RIGHT = 41,

  /**
   * Requested Left Velocity
   * Packet ID: 42
   * Data Bytes 2, signed
   * The left wheel velocity most recently requested with a Drive Direct
   * command is returned as a signed 16-bit number, high byte first.
   *
   * Range: -500 - 500 mm/s
   */
  ROOMBA_VELOCITY_LEFT = 42,

  /**
   * Left Encoder Counts
   * Packet ID: 43
   * Data Bytes 2, unsigned
   *
   * The cumulative number of raw left encoder counts is returned as an
   * unsigned 16-bit number, high byte first. This number will roll over to 0
   * after it passes 65535.
   *
   * Range: 0 - 65535
   */
  ROOMBA_ENCODER_COUNTS_LEFT = 43,

  /**
   * Right Encoder Counts
   * Packet ID: 44
   * Data Bytes 2, unsigned
   * The cumulative number of raw right encoder counts is returned as an
   * unsigned 16-bit number, high byte first. This number will roll over to 0
   * after it passes 65535.
   *
   * Range: 0 - 65535
   */
  ROOMBA_ENCODER_COUNTS_RIGHT = 44,

  /**
   * Light Bumper
   * Package ID: 45
   * Data Bytes 1, unsigned
   *
   * The light bumper detections are returned as individual bits.
   *
   * | Bit   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   * |-------|---|---|---|---|---|---|---|---|
   * | Value | Reserved | Lt Bumper Right? | Lt Bumper Front Right? | Lt Bumper Center Right? | Lt Bumper Center Left? | Lt Bumper Front Left? | Lt Bumper Left? |
   *
   * Range: 0-127
   */
  ROOMBA_LIGHT_BUMPER = 45,

  /**
   * Light Bump Left Signal
   * Package ID: 46
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump left signal is returned as an unsigned
   * 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_LEFT = 46,

  /**
   * Light Bump Front Left Signal
   * Package ID: 47
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump front left signal is returned as an
   * unsigned 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_FRONT_LEFT = 47,

  /**
   * Light Bump Center Left Signal
   * Package ID: 48
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump center left signal is returned as an
   * unsigned 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_CENTER_LEFT = 48,

  /**
   * Light Bump Center Right Signal
   * Package ID: 49
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump center right signal is returned as an
   * unsigned 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_CENTER_RIGHT = 49,

  /**
   * Light Bump Front Right Signal
   * Package ID: 50
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump front right signal is returned as an
   * unsigned 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_FRONT_RIGHT = 50,

  /**
   * Light Bump Right Signal
   * Package ID: 51
   * Data Bytes 2, unsigned
   *
   * The strength of the light bump right signal is returned as an
   * unsigned 16-bit value, high byte first.
   *
   * Range: 0-4095
   */
  ROOMBA_LIGHT_BUMP_RIGHT = 51,

  /**
   * Infrared Character Left
   * Packet ID: 52
   * Data Bytes: 1, unsigned
   *
   * This value identifies the 8-bit IR character currently being received by
   * Roomba’s left receiver. A value of 0 indicates that no character is being
   * received. These characters include those sent by the Roomba Remote, Dock,
   * Virtual Walls, Create robots using the Send-IR command, and user-created
   * devices.
   *
   * Range: 0 – 255
   */
  ROOMBA_IR_OPCODE_LEFT = 52,

  /**
   * Infrared Character Right
   * Packet ID: 53
   * Data Bytes: 1, unsigned
   *
   * This value identifies the 8-bit IR character currently being received by
   * Roomba’s right receiver. A value of 0 indicates that no character is being
   * received. These characters include those sent by the Roomba Remote, Dock,
   * Virtual Walls, Create robots using the Send-IR command, and user-created
   * devices.
   *
   * Range: 0 – 255
   */
  ROOMBA_IR_OPCODE_RIGHT = 53,

  /**
   * Left Motor Current
   * Packet ID: 54
   * Data Bytes: 2, signed
   *
   * This returns the current being drawn by the left wheel motor as an
   * unsigned 16 bit value, high byte first.
   *
   * Range: -32768 – 32767 mA
   */
  ROOMBA_LEFT_MOTOR_CURRENT = 54,

  /**
   * Right Motor Current
   * Packet ID: 55
   * Data Bytes: 2, signed
   *
   * This returns the current being drawn by the right wheel motor as an
   * unsigned 16 bit value, high byte first.
   *
   * Range: -32768 – 32767 mA
   */
  ROOMBA_RIGHT_MOTOR_CURRENT = 55,

  /**
   * Main Brush Motor Current
   * Packet ID: 56
   * Data Bytes: 2, signed
   *
   * This returns the current being drawn by the main brush motor as an
   * unsigned 16 bit value, high byte first.
   *
   * Range: -32768 – 32767 mA
   */
  ROOMBA_MAIN_BRUSH_CURRENT =56,

  /**
   * Side Brush Motor Current
   * Packet ID: 57
   * Data Bytes: 2, signed
   * This returns the current being drawn by the side brush motor as an
   * unsigned 16 bit value, high byte first.
   *
   * Range: -32768 – 32767 mA
   */
  ROOMBA_SIDE_BRUSH_CURRENT = 57,

  /**
   * Stasis
   * Packet ID: 58
   * Data Bytes: 1
   *
   * The stasis caster sensor returns 1 when the robot is making forward
   * progress and 0 when it is not. This always returns 0 when the robot is
   * turning, driving backward, or not driving. Range: 0 – 1
   */
  ROOMBA_STASIS = 58,

} ROOMBA_PACKET_CODE;

typedef enum {
  G0 = 0,
  G1 = 1,
  G2 = 2,
  G3 = 3,
  G4 = 4,
  G5 = 5,
  G6 = 6,
  ALL_PACKETS = 100,
  G101 =101,
  G106 = 106,
  G107 = 107,
} ROOMBA_PACKET_GROUP;

typedef enum _roomba_op_mode {
  ROOMBA_OFF_MODE,
  ROOMBA_PASSIVE_MODE,
  ROOMBA_SAFE_MODE,
  ROOMBA_FULL_MODE,
} ROOMBA_MODE;

typedef enum {
	ROOMBA_300BPS    = 0,
	ROOMBA_600BPS    = 1,
	ROOMBA_1200BPS   = 2,
	ROOMBA_2400BPS   = 3,
	ROOMBA_4800BPS   = 4,
	ROOMBA_9600BPS   = 5,
	ROOMBA_14400BPS  = 6,
	ROOMBA_19200BPS  = 7,
	ROOMBA_28800BPS  = 8,
	ROOMBA_38400BPS  = 9,
	ROOMBA_57600BPS  = 10,
	ROOMBA_115200BPS = 11,
} ROOMBA_BITRATE;

typedef struct _pkt_group_0 {
  uint8_t bumps_wheeldrops;
  uint8_t wall;
  uint8_t cliff_left;
  uint8_t cliff_front_left;
  uint8_t cliff_front_right;
  uint8_t cliff_right;
  uint8_t virtual_wall;
  uint8_t overcurrents;
  uint8_t dirt_detect;
  uint8_t unused_1;
  uint8_t ir_opcode;
  uint8_t buttons_pkt;
  int16_t distance;
  int16_t angle;
  uint8_t charging_state;
  uint16_t voltage;
  int16_t current;
  int8_t temperature;
  uint16_t battery_charge;
  uint16_t battery_capacity;
} ROOMBA_PACKET_GROUP_0;

typedef struct _pkt_group_1 {
  uint8_t bumps_wheeldrops;
  uint8_t wall;
  uint8_t cliff_left;
  uint8_t cliff_front_left;
  uint8_t cliff_front_right;
  uint8_t cliff_right;
  uint8_t virtual_wall;
  uint8_t overcurrents;
  uint8_t dirt_detect;
  uint8_t unused_1;
} ROOMBA_PACKET_GROUP_1;

typedef struct _pkt_group_2 {
  uint8_t ir_opcode;
  uint8_t buttons_pkt;
  int16_t distance;
  int16_t angle;
} ROOMBA_PACKET_GROUP_2;

typedef struct _pkt_group_3 {
  uint8_t charging_state;
  uint16_t voltage;
  int16_t current;
  int8_t temperature;
  uint16_t battery_charge;
  uint16_t battery_capacity;
} ROOMBA_PACKET_GROUP_3;

typedef struct _pkt_group_4 {
  uint16_t wall_signal;
  uint16_t cliff_left_signal;
  uint16_t cliff_front_left_signal;
  uint16_t cliff_front_right_signal;
  uint16_t cliff_right_signal;
  uint8_t unused_2;
  uint16_t unused_3;
  uint8_t charger_available;
} ROOMBA_PACKET_GROUP_4;

typedef struct _pkt_group_5 {
  uint8_t open_interface_mode;
  uint8_t song_number;
  uint8_t song_playing;
  uint8_t oi_stream_num_packets;
  int16_t velocity;
  int16_t radius;
  int16_t velocity_right;
  int16_t velocity_left;
} ROOMBA_PACKET_GROUP_5;

typedef struct _pkt_group_6 {
  uint8_t bumps_wheeldrops;
  uint8_t wall;
  uint8_t cliff_left;
  uint8_t cliff_front_left;
  uint8_t cliff_front_right;
  uint8_t cliff_right;
  uint8_t virtual_wall;
  uint8_t overcurrents;
  uint8_t dirt_detect;
  uint8_t unused_1;
  uint8_t ir_opcode;
  uint8_t buttons_pkt;
  int16_t distance;
  int16_t angle;
  uint8_t charging_state;
  uint16_t voltage;
  int16_t current;
  int8_t temperature;
  uint16_t battery_charge;
  uint16_t battery_capacity;
  uint16_t wall_signal;
  uint16_t cliff_left_signal;
  uint16_t cliff_front_left_signal;
  uint16_t cliff_front_right_signal;
  uint16_t cliff_right_signal;
  uint8_t unused_2;
  uint16_t unused_3;
  uint8_t charger_available;
  uint8_t open_interface_mode;
  uint8_t song_number;
  uint8_t song_playing;
  uint8_t oi_stream_num_packets;
  int16_t velocity;
  int16_t radius;
  int16_t velocity_right;
  int16_t velocity_left;
} ROOMBA_PACKET_GROUP_6;

typedef struct _pkt_group_100 {
  uint8_t bumps_wheeldrops;
  uint8_t wall;
  uint8_t cliff_left;
  uint8_t cliff_front_left;
  uint8_t cliff_front_right;
  uint8_t cliff_right;
  uint8_t virtual_wall;
  uint8_t overcurrents;
  uint8_t dirt_detect;
  uint8_t unused_1;
  uint8_t ir_opcode;
  uint8_t buttons_pkt;
  int16_t distance;
  int16_t angle;
  uint8_t charging_state;
  uint16_t voltage;
  int16_t current;
  int8_t temperature;
  uint16_t battery_charge;
  uint16_t battery_capacity;
  uint16_t wall_signal;
  uint16_t cliff_left_signal;
  uint16_t cliff_front_left_signal;
  uint16_t cliff_front_right_signal;
  uint16_t cliff_right_signal;
  uint8_t unused_2;
  uint16_t unused_3;
  uint8_t charger_available;
  uint8_t open_interface_mode;
  uint8_t song_number;
  uint8_t song_playing;
  uint8_t oi_stream_num_packets;
  int16_t velocity;
  int16_t radius;
  int16_t velocity_right;
  int16_t velocity_left;
  uint16_t encoder_counts_left;
  uint16_t encoder_counts_right;
  uint8_t light_bumper;
  uint16_t light_bump_left;
  uint16_t light_bump_front_left;
  uint16_t light_bump_center_left;
  uint16_t light_bump_center_right;
  uint16_t light_bump_front_right;
  uint16_t light_bump_right;
  uint8_t ir_opcode_left;
  uint8_t ir_opcode_right;
  int16_t left_motor_current;
  int16_t right_motor_current;
  int16_t main_brush_current;
  int16_t side_brush_current;
  uint8_t stasis;
} ROOMBA_PACKET_GROUP_100;

typedef struct _pkt_group_101 {
  uint16_t encoder_counts_left;
  uint16_t encoder_counts_right;
  uint8_t light_bumper;
  uint16_t light_bump_left;
  uint16_t light_bump_front_left;
  uint16_t light_bump_center_left;
  uint16_t light_bump_center_right;
  uint16_t light_bump_front_right;
  uint16_t light_bump_right;
  uint8_t ir_opcode_left;
  uint8_t ir_opcode_right;
  int16_t left_motor_current;
  int16_t right_motor_current;
  int16_t main_brush_current;
  int16_t side_brush_current;
  uint8_t stasis;
} ROOMBA_PACKET_GROUP_101;

typedef struct _pkt_group_106 {
  uint16_t light_bump_left;
  uint16_t light_bump_front_left;
  uint16_t light_bump_center_left;
  uint16_t light_bump_center_right;
  uint16_t light_bump_front_right;
  uint16_t light_bump_right;
} ROOMBA_PACKET_GROUP_106;

typedef struct _pkt_group_107 {
  int16_t left_motor_current;
  int16_t right_motor_current;
  int16_t main_brush_current;
  int16_t side_brush_current;
  uint8_t stasis;
} ROOMBA_PACKET_GROUP_107;

/*******************************************************************************
 MACROS
*******************************************************************************/

//#define ROOMBA_GET_PACKET_GROUP(ROOMBA_PACKET_CODE code) ()
/*
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
      x = min(a, b);          ==>  x = ((a) < (b) ? (a) : (b));
      y = min(1, 2);          ==>  y = ((1) < (2) ? (1) : (2));
      z = min(a + 28, *p);    ==>  z = ((a + 28) < (*p) ? (a + 28) : (*p));
*/


/*******************************************************************************
 * Function
 ******************************************************************************/

int is_valid_roomba_command (uint8_t command[], uint16_t size);

/**
 * @param uart_send_byte_callback_function a function that sends a uart byte to
 * the roomba set to a baud rate of 19200.
 */
 //void roomba_init(void(*uart_send_byte_fn)(uint8_t));

/**
 * @param bitrate the bitrate to use when sending data to and from the roomba
 * @param uart_send_byte_callback_function a function that sends a uart byte to
 * the roomba set to the chosen baud rate.
 */
//void roomba_start(void(*uart_send_byte_fn)(uint8_t));

//void roomba_set_baud(void(*uart_send_byte_fn)(uint8_t), ROOMBA_BITRATE baudrate);

//void roomba_send_command(ROOMBA_OP_CODE code, char * data);

//char * roomba_get_packet(ROOMBA_PACKET_CODE code);



#endif /* ROOMBA_H_ */
