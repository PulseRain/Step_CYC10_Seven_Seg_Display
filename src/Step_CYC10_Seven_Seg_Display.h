/*
###############################################################################
# Copyright (c) 2019, PulseRain Technology LLC 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (LGPL) as 
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE.  
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
*/

#ifndef STEP_CYC10_SEVEN_SEG_DISPLAY
#define STEP_CYC10_SEVEN_SEG_DISPLAY

#include "Arduino.h"


//----------------------------------------------------------------------------
//  Arduino Library for the 7-segment display on Step CYC10 FPGA board 
//----------------------------------------------------------------------------

class Step_CYC10_Seven_Seg_Display
{
    public:
    
//----------------------------------------------------------------------------
// reset()
//
// Parameters:
//      value_to_display    : 16 bit value to be displayed on the 4-position
//           7-seg display
//
//      refresh_rate_in_Hz  : refresh rate for the 7-segment display. It is 
//           suggested to be no more than 400 Hz to avoid flicking 
//
//      active_dp_mask      : 4 bit mask for the decimal point. 0xF to be all 
//           on, 0x0 to be all off.
//     
// Return Value:
//      None
//
// Remarks:
//      Function to set the value / refresh_frequency for 7-seg display
//----------------------------------------------------------------------------
        void reset (uint16_t value_to_display, uint32_t refresh_rate_in_Hz, uint8_t active_dp_mask  = 0)
        {
            value_ = value_to_display;
            index_ = 0;
            active_dp_mask_ = active_dp_mask;
            
            refresh_count_ = TIMER_RESOLUTION / refresh_rate_in_Hz;
        }



//----------------------------------------------------------------------------
// Step_CYC10_Seven_Seg_Display()
//
// Parameters:
//      value_to_display    : 16 bit value to be displayed on the 4-position
//           7-seg display
//
//      refresh_rate_in_Hz  : refresh rate for the 7-segment display. It is 
//           suggested to be no more than 400 Hz to avoid flicking 
//
//      active_dp_mask      : 4 bit mask for the decimal point. 0xF to be all 
//           on, 0x0 to be all off.
//     
// Return Value:
//      None
//
// Remarks:
//      constructor for the class
//----------------------------------------------------------------------------

        Step_CYC10_Seven_Seg_Display (uint16_t value_to_display, uint32_t refresh_rate_in_Hz) 
        {
            reset (value_to_display, refresh_rate_in_Hz); 
        }



//----------------------------------------------------------------------------
// set_display_value()
//
// Parameters:
//      value           : 16 bit value to be displayed on the 4-position 
//               7-seg display
//
//      active_dp_mask  : 4 bit mask for the decimal point. 0xF to be all 
//           on, 0x0 to be all off.
//     
// Return Value:
//      None
//
// Remarks:
//      change the display value for the 7-seg display
//----------------------------------------------------------------------------
        void set_display_value (uint16_t value, uint8_t active_dp_mask)
        {
            value_          = value;
            active_dp_mask_ = active_dp_mask;
        }
        
//----------------------------------------------------------------------------
// start_refresh()
//
// Parameters:
//      None
//     
// Return Value:
//      None
//
// Remarks:
//      Install the timer ISR for 7-seg display
//----------------------------------------------------------------------------
        
        void start_refresh ()
        { 
            timer_advance_ (refresh_count_);
            attachInterrupt (INT_TIMER_INDEX, seven_segment_display_timer_isr_, RISING);
        }

//----------------------------------------------------------------------------
// stop_refresh()
//
// Parameters:
//      None
//     
// Return Value:
//      None
//
// Remarks:
//      Uninstall the timer ISR for 7-seg display
//----------------------------------------------------------------------------

        void stop_refresh ()
        {
            detachInterrupt (INT_TIMER_INDEX);      
        }


    private:

//----------------------------------------------------------------------------
//  Internal variables
//----------------------------------------------------------------------------
        static uint16_t value_;
        static uint8_t index_;
        static uint8_t active_dp_mask_;
        static uint32_t refresh_count_;

//----------------------------------------------------------------------------
//  display format for 7-segment display
//----------------------------------------------------------------------------

        static constexpr uint8_t seven_seg_display_encoding_ [16] = {
           0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 
        };

//----------------------------------------------------------------------------
// timer_advance_()
//
// Parameters:
//      count : the number of ticks to advance for the system timer
//     
// Return Value:
//      None
//
// Remarks:
//      Internal function to advance system timer
//----------------------------------------------------------------------------

        static void timer_advance_ (uint32_t count)
        {
            uint32_t low, high;
            uint64_t finish_time, current_time;
            uint32_t interrupt_saved_mstatus_timer_advance;

            interrupt_saved_mstatus_timer_advance = read_csr (mstatus);
            
            write_csr (mstatus, 0);
                low  = (*REG_MTIME_LOW);
                high = (*REG_MTIME_HIGH);
            finish_time = ((uint64_t)high << 32) + (uint64_t)(low) + (uint64_t)count;
    
                low  = finish_time & 0xFFFFFFFF;
                high = (finish_time >> 32) & 0xFFFFFFFF;
    
            (*REG_MTIMECMP_LOW) = 0xFFFFFFFF;
            (*REG_MTIMECMP_HIGH) = high;
            (*REG_MTIMECMP_LOW)  = low ;    

            write_csr (mstatus, interrupt_saved_mstatus_timer_advance);
        }

//----------------------------------------------------------------------------
// _refresh()
//
// Parameters:
//      None
//     
// Return Value:
//      None
//
// Remarks:
//      Internal function to move the active display position
//----------------------------------------------------------------------------
        static void _refresh ()
        {
            uint8_t t;

            GPIO_P1 = 1 << index_;
            t = (value_ >> (index_ * 4)) & 0xF;
            
            GPIO_P0 = seven_seg_display_encoding_[t] | ((active_dp_mask_ >> index_) & 1) << 7;
            index_ = (index_ + 1) % 4;

        }


//----------------------------------------------------------------------------
// seven_segment_display_timer_isr_()
//
// Parameters:
//      None
//     
// Return Value:
//      None
//
// Remarks:
//      ISR for system timer
//----------------------------------------------------------------------------
        static void seven_segment_display_timer_isr_()
        {
            _refresh ();
            timer_advance_ (refresh_count_);
        }

};

extern Step_CYC10_Seven_Seg_Display SEVEN_SEG_DISPLAY;


#endif
