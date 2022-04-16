/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file sparc_regformat.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-02-26
*/
#ifndef __SPARC_REGFORMAT_H__
#define __SPARC_REGFORMAT_H__
enum regnames { G0, G1, G2, G3, G4, G5, G6, G7,
                O0, O1, O2, O3, O4, O5, SP, O7,
                L0, L1, L2, L3, L4, L5, L6, L7,
                I0, I1, I2, I3, I4, I5, FP, I7,
                /*  From here down cannot be used in REG()  */
                F0, F1, F2, F3, F4, F5, F6, F7,
                F8, F9, F10, F11, F12, F13, F14, F15,
                F16, F17, F18, F19, F20, F21, F22, F23,
                F24, F25, F26, F27, F28, F29, F30, F31,
                Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR };
#endif
