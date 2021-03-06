#############################################################################
#
# $Id: FindPARPORT.cmake 4056 2013-01-05 13:04:42Z fspindle $
#
# This file is part of the ViSP software.
# Copyright (C) 2005 - 2013 by INRIA. All rights reserved.
# 
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# ("GPL") version 2 as published by the Free Software Foundation.
# See the file LICENSE.txt at the root directory of this source
# distribution for additional information about the GNU GPL.
#
# For using ViSP with software that can not be combined with the GNU
# GPL, please contact INRIA about acquiring a ViSP Professional 
# Edition License.
#
# See http://www.irisa.fr/lagadic/visp/visp.html for more information.
# 
# This software was developed at:
# INRIA Rennes - Bretagne Atlantique
# Campus Universitaire de Beaulieu
# 35042 Rennes Cedex
# France
# http://www.irisa.fr/lagadic
#
# If you have questions regarding the use of this file, please contact
# INRIA at visp@inria.fr
# 
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Description:
# Try to find linux/parport.h.h for parallel port usage.
# Once run this will define: 
#
# PARPORT_FOUND
#
# Authors:
# Fabien Spindler
#
#############################################################################

FIND_PATH(PARPORT_INCLUDE_DIR linux/parport.h
  /usr/include 
  /usr/src/linux/include)
  #MESSAGE("DBG PARPORT_INCLUDE_DIR=${PARPORT_INCLUDE_DIR}")  

MARK_AS_ADVANCED(
  PARPORT_INCLUDE_DIR
)
      
IF(PARPORT_INCLUDE_DIR)
  SET(PARPORT_FOUND TRUE)
ELSE(PARPORT_INCLUDE_DIR)
  SET(PARPORT_FOUND FALSE)
ENDIF(PARPORT_INCLUDE_DIR)
  
