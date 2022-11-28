


#include "devices/ADS1299.hpp"
#include "devices/BHI160.hpp"
#include "devices/MAX30102.hpp"
#include "devices/MCP3561.hpp"
#include "devices/PCF8574.hpp"
#include "devices/TSC2003.hpp"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_util/i2cMaster.hpp"
#include "esp_util/spiHost.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display/display.hpp"
#include "display/displayConfig.hpp"
#include "lvgl.h"

#include "esp_util/i2cDevice.hpp"
#include "i2cConfig.hpp"
#include "spiConfig.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <thread>



extern "C" void app_main() {
    esp::i2cMaster<I2C0_Config>      boardI2C;
    //BHI160<I2C0_Config, GPIO_NUM_39> imu;
    //MAX30102<I2C0_Config>            pulseOxiMeter;
    //PCF8574<I2C0_Config>             ioExpander;
    //TSC2003<I2C0_Config, GPIO_NUM_2> touchScreenController;

    esp::spiHost<BoardSPIConfig>                                                boardSPI;
    //ADS1299<BoardSPIConfig, 4, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_0, GPIO_NUM_36> ecg{boardSPI};
    MCP3561<BoardSPIConfig, 8, GPIO_NUM_33, GPIO_NUM_34>                        adc{boardSPI};

    //Display<displayConfig> display;

    auto next10ms = std::chrono::system_clock::now() + std::chrono::milliseconds(80);
    std::size_t valueCounter{};
    while(1) {
        auto now = std::chrono::system_clock::now();
        if(next10ms < now)
        {
            //display.handler();
            next10ms = now + std::chrono::milliseconds(200);
        }

        if(adc.outputData.has_value() && valueCounter < 1000){
            ++valueCounter;
            //def calcV(LSB, Vref, G):
            //    return (LSB * Vref) / (G * 8388608)
            auto calcVoltage = [](std::int32_t LSB)
                   -> double
                    {
                static constexpr double Vref{2.40};
                static constexpr auto Gain{1};
                static constexpr auto DataSheetConstant{8388608};
                return (LSB * Vref) / (Gain * DataSheetConstant);
            };
            fmt::print("{} {}\n",std::chrono::steady_clock::now().time_since_epoch(),
                       calcVoltage(adc.outputData.value()));
            adc.outputData = {};
        }

        /*
      if(ecg.noiseData.has_value()){
            //fmt::print("Noise Data: {:#032b}\n",ecg.noiseData.value()[0]);
            fmt::print("Noise Data: {}\n",ecg.noiseData.value()[0]);
            ecg.noiseData = {};
        } */
        /*
        if(ecg.ecgData.has_value()){
            //fmt::print("{} {:#034b}\n", std::chrono::steady_clock::now().time_since_epoch() ,ecg.ecgData.value()[0]);
            //fmt::print("{} {}\n", std::chrono::steady_clock::now().time_since_epoch() , ecg.ecgData.value()[0]);
            fmt::print("{} {}\n", std::chrono::steady_clock::now().time_since_epoch() , fmt::join(ecg.ecgData.value(), ", "));
            //display.setECGValue(ecg.ecgData.value()[0]);
            ecg.ecgData = {};
        }
        /*


        if(pulseOxiMeter.IRDValue.has_value() && pulseOxiMeter.RDValue.has_value()){
            //fmt::print("Data: {} {} {}\n",std::chrono::steady_clock::now().time_since_epoch() , pulseOxiMeter.IRDValue.value(), pulseOxiMeter.RDValue.value());
            //display.setOxiValues(pulseOxiMeter.RDValue.value(), pulseOxiMeter.IRDValue.value());
            //pulseOxiMeter.IRDValue = {};
            //pulseOxiMeter.RDValue = {};
        }
        /*


        if(imu.accData.X.has_value() && imu.accData.Y.has_value() && imu.accData.Z.has_value()){
            fmt::print("IMU: {:05}, {:05}, {:05}\n", imu.accData.X.value(),imu.accData.Y.value(),imu.accData.Z.value());
            imu.accData.X = {};
            imu.accData.Y = {};
            imu.accData.Z = {};
        }
*/
        //ecg.handler();
        adc.handler();

        //ioExpander.handler();
        //imu.handler();
        //pulseOxiMeter.handler();
        //touchScreenController.handler();
    }

    return;
}
