/****************************************************************************
 *
 * $Id: vpFeatureBuilderPoint3D.cpp 4056 2013-01-05 13:04:42Z fspindle $
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
 * Conversion between tracker and visual feature 3D Point.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpFeatureBuilderPoint3D.cpp
  \brief  conversion between tracker
  and visual feature 3D Point
*/
#include <visp/vpFeatureBuilder.h>
#include <visp/vpFeatureException.h>
#include <visp/vpException.h>


/*!

  Initialize a 3D point feature using the coordinates of the point
  \f$(X,Y,Z)\f$ in the camera frame. The values of \f$(X,Y,Z)\f$ are
  expressed in meters.

  \warning To be sure that the vpFeaturePoint is well initialized, you have to be sure that at least the point
  coordinates in the camera frame are computed and stored in the vpPoint.

  \param s : Visual feature to initialize.

  \param t : The vpPoint used to create the vpFeaturePoint.
*/
void
vpFeatureBuilder::create(vpFeaturePoint3D &s, const vpPoint &t )
{
  try
  {



    s.set_X( t.cP[0]/t.cP[3]) ;
    s.set_Y( t.cP[1]/t.cP[3])  ;
    s.set_Z( t.cP[2]/t.cP[3])  ;


  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}



/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
