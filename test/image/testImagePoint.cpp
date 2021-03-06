/****************************************************************************
 *
 * $Id: testImagePoint.cpp 4056 2013-01-05 13:04:42Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2013 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * Test for vpImagePoint class.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/
/*!
  \example testImagePoint.cpp

  \brief Test vpImagePoint functionalities.

*/

#include <visp/vpImagePoint.h>
#include <iostream>

int main()
{
  vpImagePoint ip1, ip2, ip3;

  ip1.set_u(-11.1);
  ip1.set_v(10);

  ip2.set_j(-11.1);
  ip2.set_i(10);

  ip3.set_j(11.10001);
  ip3.set_i(10.1);

  std::cout << "We define ip1 with coordinates: " 
            << ip1 << std::endl;

  
  std::cout << "We define ip2 with coordinates: " 
            << ip2 << std::endl;

  std::cout << "We define ip3 with coordinates: " 
            << ip3 << std::endl;

  if (ip1 == ip2) {
    std::cout << "ip1 == ip2" << std::endl;
  }
  else {
    std::cout << "ip1 != ip2 (bad result)" << std::endl;
    return -1;
  }

  if (ip1 != ip2) {
    std::cout << "ip1 != ip2 (bad result)" << std::endl;
    return -1;
  } 
  else {
    std::cout << "ip1 == ip2" << std::endl;
  }

  if (ip1 == ip3) {
    std::cout << "ip1 == ip3 (bad result)" << std::endl;
    return -1;
  }
  else {
    std::cout << "ip1 != ip3" << std::endl;
  }

  if (ip1 != ip3) {
    std::cout << "ip1 != ip3" << std::endl;
  }
  else {
    std::cout << "ip1 == ip3 (bad result)" << std::endl;
    return -1;
  }


  return 0;
}
