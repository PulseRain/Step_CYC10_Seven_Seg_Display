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

#include "Step_CYC10_7_seg_display.h"

uint16_t Step_CYC10_7_seg_display::value_;
uint32_t Step_CYC10_7_seg_display::refresh_count_;
uint8_t  Step_CYC10_7_seg_display::index_;
uint8_t  Step_CYC10_7_seg_display::active_dp_mask_;

Step_CYC10_7_seg_display SEVEN_SEG_DISPLAY (0xbeef, 400);
