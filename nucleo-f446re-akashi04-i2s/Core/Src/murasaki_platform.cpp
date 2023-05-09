/**
 * @file murasaki_platform.cpp
 *
 * @date 2018/05/20
 * @author Seiichi "Suikan" Horie
 * @brief A glue file between the user application and HAL/RTOS.
 */

// Include the definition created by CubeIDE.
#include <murasaki_platform.hpp>
#include "main.h"

// Include the murasaki class library.
#include "murasaki.hpp"
#include "adau1361.hpp"

// Include the prototype  of functions of this file.

/* -------------------- PLATFORM Macros -------------------------- */
#define CODEC_I2C_DEVICE_ADDR 0x38
#define AUDIO_CHANNEL_LEN 128
/* -------------------- PLATFORM Type and classes -------------------------- */

/* -------------------- PLATFORM Variables-------------------------- */

// Essential definition.
// Do not delete
murasaki::Platform murasaki::platform;
murasaki::Debugger *murasaki::debugger;

/* ------------------------ STM32 Peripherals ----------------------------- */

/*
 * Platform dependent peripheral declaration.
 *
 * The variables here are defined at the top of the main.c.
 * Only the variable needed by the InitPlatform() are declared here
 * as external symbols.
 *
 * The declaration here is user project dependent.
 */
// Following block is just sample.
// Original declaration is in the top of main.c.
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef hlpuart1;
extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;

// Definition for Nucleo.
// Note that the given processor name MACRO doesn't have the suffix. Thus, certain processor
// Which has multiple Nucleo ( ex: G431 32/48 pin, F446 48/144 pin ) may cause problem.
#if defined(STM32F091xC)
// For Nucleo F091RC (32pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F446xx)
// For Nucleo G431RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F722xx)
// For Nucleo F722ZE (144pin)
#define UART_PORT huart3
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F746xx)
// For Nucleo F746ZG (144pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G070xx)
// For Nucleo G070RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G431xx)
// For Nucleo G431RB (48pin)
#define UART_PORT hlpuart1
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32H743xx)
// For Nucleo H743ZI (144pin)
#define UART_PORT huart3
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32L152xE)
// For Nucleo L152RE (48pin)
#define UART_PORT hlpuart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32L412xx)
// For Nucleo L412RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD4_GPIO_Port
#define LED_PIN LD4_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G0B1xx)
// For Nucleo G01B1RE (48pin)
#define UART_PORT huart2
#define LED_PORT LED_GREEN_GPIO_Port
#define LED_PIN LED_GREEN_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32H503xx)
// For Nucleo G01B1RE (48pin)
#define UART_PORT huart3
#define LED_PORT USER_LED_GPIO_Port
#define LED_PIN USER_LED_Pin
extern UART_HandleTypeDef UART_PORT;

#else
#error "Unknown nucleo. Please define the UART_PORT, LED_PORT, LED_PIN macro."
#endif

/* -------------------- PLATFORM Prototypes ------------------------- */

void TaskBodyFunction(const void *ptr);

/* -------------------- PLATFORM Implementation ------------------------- */

// Initialization of the system.
void InitPlatform()
{
#if ! MURASAKI_CONFIG_NOCYCCNT
    // Start the cycle counter to measure the cycle in MURASAKI_SYSLOG.
    murasaki::InitCycleCounter();
#endif
    // UART device setting for console interface.
    // On Nucleo, the port connected to the USB port of ST-Link is
    // referred here.
    murasaki::platform.uart_console = new murasaki::DebuggerUart(&UART_PORT);
    while (nullptr == murasaki::platform.uart_console)
        ;  // stop here on the memory allocation failure.

    // UART is used for logging port.
    // At least one logger is needed to run the debugger class.
    murasaki::platform.logger = new murasaki::UartLogger(murasaki::platform.uart_console);
    while (nullptr == murasaki::platform.logger)
        ;  // stop here on the memory allocation failure.

    // Setting the debugger
    murasaki::debugger = new murasaki::Debugger(murasaki::platform.logger);
    while (nullptr == murasaki::debugger)
        ;  // stop here on the memory allocation failure.

    // Set the debugger as AutoRePrint mode, for the easy operation.
    murasaki::debugger->AutoRePrint();  // type any key to show history.

    // For demonstration, one GPIO LED port is reserved.
    // The port and pin names are fined by CubeIDE.
    murasaki::platform.led = new murasaki::BitOut(LED_PORT, LED_PIN);
    MURASAKI_ASSERT(nullptr != murasaki::platform.led)

    murasaki::platform.led_st0 = new murasaki::BitOut(LED1_GPIO_Port, LED1_Pin);
    MURASAKI_ASSERT(nullptr != murasaki::platform.led_st0)
    murasaki::platform.led_st1 = new murasaki::BitOut(LED2_GPIO_Port, LED2_Pin);
    MURASAKI_ASSERT(nullptr != murasaki::platform.led_st1)

    // Create an I2C master controller.
    murasaki::platform.i2c_master = new murasaki::I2cMaster(&hi2c1);
    MURASAKI_ASSERT(nullptr != murasaki::platform.i2c_master)

    // Create an ADAU1361 CODEC controller.
    murasaki::platform.codec = new murasaki::Adau1361(
                                                      48000, /* Fs 48kHz*/
                                                      12000000, /* Master clock Xtal frequency, on the UMB-ADAU1361-A board */
                                                      murasaki::platform.i2c_master, /* I2C master port to intgerface with CODEC */
                                                      CODEC_I2C_DEVICE_ADDR); /* Address in 7 bit */

    MURASAKI_ASSERT(nullptr != murasaki::platform.codec)

    // Create an Audio Port as I2S.
    murasaki::platform.audio_port = new murasaki::I2sPortAdapter(
                                                                 &hi2s2, /* TX port.*/
                                                                 &hi2s3); /* RX port. */

    MURASAKI_ASSERT(nullptr != murasaki::platform.audio_port)

    // Create an Audio Framework
    // For both input and output
    murasaki::platform.audio = new murasaki::DuplexAudio(
                                                         murasaki::platform.audio_port, /* Using the port created above */
                                                         AUDIO_CHANNEL_LEN); /* Length of the each channels. For stereo, both L and R will have this length */
    MURASAKI_ASSERT(nullptr != murasaki::platform.audio)

    // For demonstration of FreeRTOS task.
    murasaki::platform.audio_task = new murasaki::SimpleTask(
                                                             "Audio Task",
                                                             256, /* Stack size */
                                                             murasaki::ktpRealtime, /* Audio signal processing need higher priorirty */
                                                             nullptr, /* Stack is needed to allocate internally */
                                                             &TaskBodyFunction
                                                             );
    MURASAKI_ASSERT(nullptr != murasaki::platform.audio_task)

    // For synchronization between ExecPlatoform() and audio task.
    murasaki::platform.codec_ready = new murasaki::Synchronizer();
    MURASAKI_ASSERT(nullptr != murasaki::platform.codec_ready)


}

void ExecPlatform()
{
    // counter for the demonstration.
    int count = 0;


    // Start audio
    murasaki::platform.audio_task->Start();

    // Wait for the codec is ready.
    murasaki::platform.codec_ready->Wait();

    murasaki::Sleep(30);

    // unmute the input and output channels.
    murasaki::platform.codec->Mute(
                                   murasaki::kccLineInput,
                                   false);                     // unmute
    murasaki::platform.codec->Mute(
                                   murasaki::kccHeadphoneOutput,
                                   false);                     // unmute


    // Loop forever. Just status blinking.
    while (true) {

        // print a message with counter value to the console.
        murasaki::debugger->Printf("Hello %d \n", count);

        // update the counter value.
        count++;

        // wait for a while
        murasaki::Sleep(500);
    }
}


/* ------------------ User Functions -------------------------- */
/**
 * @brief Demonstration task.
 * @param ptr Pointer to the parameter block
 * @details
 * Task body function as demonstration of the @ref murasaki::SimpleTask.
 *
 * Copy input audio to output. Talk through.
 */
void TaskBodyFunction(const void *ptr) {
    // Audio sample bufferes
    float *tx_left = new float[AUDIO_CHANNEL_LEN];
    float *tx_right = new float[AUDIO_CHANNEL_LEN];
    float *rx_left = new float[AUDIO_CHANNEL_LEN];
    float *rx_right = new float[AUDIO_CHANNEL_LEN];

    // Fill by zero to avoid the big noise at beginning.
    for (int i = 0; i < AUDIO_CHANNEL_LEN; i++) {
        tx_left[i] = 0.0;
        tx_right[i] = 0.0;
    }

    // Start codec activity.
    murasaki::platform.codec->Start();

    // Input and Output gain setting. Still muting.
    murasaki::platform.codec->SetGain(
                                      murasaki::kccLineInput,
                                      0.0, /* dB */
                                      0.0); /* dB */

    murasaki::platform.codec->SetGain(
                                      murasaki::kccHeadphoneOutput,
                                      0.0, /* dB */
                                      0.0); /* dB */

    // Tell codec is ready.
    murasaki::platform.codec_ready->Release();


    // Initiate the LED on the AKSAHI 02 board
    murasaki::platform.led_st0->Clear();
    murasaki::platform.led_st1->Set();




    while (true)  // Talk Through
    {
        // Wait the end of current audio transmission & receive.
        // Then, copy the tx buffer to tx DMA buffer.
        // And then copy the rx DMA buffer to rx buffer.
        murasaki::platform.audio->TransmitAndReceive(
                                                     tx_left,
                                                     tx_right,
                                                     rx_left,
                                                     rx_right);
        // Copy RX to TX : talk through
        for (int i = 0; i < AUDIO_CHANNEL_LEN; i++) {
            tx_left[i] = rx_left[i];
            tx_right[i] = rx_right[i];
        }

        // Blink status.
        murasaki::platform.led_st0->Toggle();
        murasaki::platform.led_st1->Toggle();
    }
}

