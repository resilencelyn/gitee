/////////////////////////////////////////////////////////////////////////
// $Id: kio.h,v 1.15 2009/02/08 09:05:52 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2008  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#ifndef _BX_KIO_H
#define _BX_KIO_H

#if BX_SUPPORT_KIO

class bx_kio_c : public bx_kio_stub_c {
public:
  bx_kio_c();
  virtual ~bx_kio_c() {}
  virtual void init(void);
  virtual void reset (unsigned type) {}
  virtual void mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  virtual void mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);

	void* gpio_conf_obj[4];
	Bit32u* gpio_value[4];
	void *gpio;
private:
  //Bit16u kio_registers[8]; /* different gpio value */
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned io_len);
};

#endif

#endif
