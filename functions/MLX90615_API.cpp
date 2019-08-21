/**
 * @copyright (C) 2017 Melexis N.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <MLX90615_SMBus_Driver.h>
#include <MLX90615_API.h>
#include <math.h>

//------------------------------------------------------------------------------
  
int MLX90615_DumpEE(uint8_t slaveAddr, uint16_t *eeData)
{
     int error = 0;
     char address = 0x10;
     uint16_t *p = eeData;
     
     while (address < 0x20 && error == 0)
     {
        error = MLX90615_SMBusRead(slaveAddr, address, p);
        address = address + 1;
        p = p + 1;
     }   
         
     return error;
}

//------------------------------------------------------------------------------

int MLX90615_GetTa(uint8_t slaveAddr, float *ta)
{
    int error = 0;
    uint16_t data = 0;
    
    error = MLX90615_SMBusRead(slaveAddr, 0x26, &data);
    
    if (data > 0x7FFF)
    {
        return -4;
    }
        
    *ta = (float)data * 0.02f - 273.15;
        
    return error;
}    

//------------------------------------------------------------------------------

int MLX90615_GetTo(uint8_t slaveAddr, float *to)
{
    int error = 0;
    uint16_t data = 0;
    
    error = MLX90615_SMBusRead(slaveAddr, 0x27, &data);
    
    if (data > 0x7FFF)
    {
        return -4;
    }
    
    if (error == 0)
    {
        *to = (float)data * 0.02f - 273.15;
    } 
    
    return error;
} 

//------------------------------------------------------------------------------

int MLX90615_GetIRdata(uint8_t slaveAddr, uint16_t *ir)
{
    int error = 0;
    uint16_t data = 0;
    
    error = MLX90615_SMBusRead(slaveAddr, 0x25, &data);
    
    if (error == 0)
    {
        *ir = data;
    }    
    
    return error;
}    

//------------------------------------------------------------------------------

int MLX90615_GetEmissivity(uint8_t slaveAddr, float *emissivity)
{
    int error = 0;
    uint16_t data = 0;
    error = MLX90615_SMBusRead(slaveAddr, 0x13, &data);
    
    if(data < 32768)
    {
        *emissivity = (float)data / 0x4000;
    } 
    else
    {
        *emissivity = (32768 - (float)data) / 0x4000;   
    }
    
    return error;
}

//------------------------------------------------------------------------------

int MLX90615_SetEmissivity(uint8_t slaveAddr, float value)
{
    int error = 0;
    uint16_t newE = 0;
    float temp = 0;
    
    if(value > 1.0 || value < 0.05)
    {
        return -6;
    }
    
    temp = value * 0x4000 + 0.5;
    newE = temp;
               
    error = MLX90615_SMBusWrite(slaveAddr, 0x13, 0x0000);
    
    if(error == 0)
    {
        error = MLX90615_SMBusWrite(slaveAddr, 0x13, newE);
        
    }       
               
    return error;
}
    
//------------------------------------------------------------------------------

int MLX90615_GetIIR(uint8_t slaveAddr, uint8_t *iir)
{
    int error = 0;
    uint16_t data = 0;
    error = MLX90615_SMBusRead(slaveAddr, 0x12, &data);
    
    data = data & 0x7000;
    data = data >> 12;
    
    *iir = data;
    
    return error;
}
 
//------------------------------------------------------------------------------

int MLX90615_SetIIR(uint8_t slaveAddr, uint8_t value)
{
    int error = 0;
    uint16_t data = 0;
    uint16_t val = value & 0x0007;
    
    val = val << 12;
    error = MLX90615_SMBusRead(slaveAddr, 0x12, &data);
    
    if (error == 0)
    {
        data = data & 0x8FFF;
        data = data + val;
        error = MLX90615_SMBusWrite(slaveAddr, 0x12, 0);
        if(error == 0)
        {
            error = MLX90615_SMBusWrite(slaveAddr, 0x12, data);
        }
    }
    
    return error;
}
    
//------------------------------------------------------------------------------

float MLX90615_TemperatureInFahrenheit(float temperature)
{
    return temperature * 1.8f + 32.0;
}

//------------------------------------------------------------------------------
    
int16_t MLX90615_ConvertIRdata(uint16_t ir)
{
    int16_t ir2c = ir;
    
    if(ir > 0x7FFF)
    {
        ir2c = 0x8000 - ir2c;
    }
    
    return ir2c;    
}
    