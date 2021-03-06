/****************************************************************************
 *
 * $Id: vpDirectShowGrabber.h 4056 2013-01-05 13:04:42Z fspindle $
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
 * DirectShow framegrabber.
 *
 * Authors:
 * Bruno Renier
 * Anthony Saunier
 *
 *****************************************************************************/


#ifndef vpDirectShowGrabber_hh
#define vpDirectShowGrabber_hh

#include <visp/vpConfig.h>
#if ( defined(VISP_HAVE_DIRECTSHOW) )

#include <visp/vpImage.h>
#include <visp/vpRGBa.h>
#include <visp/vpFrameGrabber.h>

class vpDirectShowGrabberImpl;

/*!
  \class vpDirectShowGrabber
  \brief class for windows direct show devices

  This class' goal is to totally hide the implementation of the framegrabber
  from the rest of the application. This is necessary to avoid conflicts
  between dshow's Qedit.h and Direct3D's includes.

  \ingroup Framegrabber CameraDriver
*/

class VISP_EXPORT vpDirectShowGrabber : public vpFrameGrabber
{
	vpDirectShowGrabberImpl * grabber;


	public:

		unsigned int getHeight();
		unsigned int getWidth();

		vpDirectShowGrabber();
		virtual ~vpDirectShowGrabber();

		void open();
		void open(vpImage<unsigned char> &I);
		void open(vpImage<vpRGBa> &I);

		void acquire(vpImage<unsigned char> &I);
		void acquire(vpImage<vpRGBa> &I);

		void close();

		//get the number of capture devices
		unsigned int getDeviceNumber();

		//change the capture device
		bool setDevice(unsigned int id);

		//displays a list of available devices
		void displayDevices();

		//set image size
		bool setImageSize(unsigned int width,unsigned int height);

		//set source framerate
		bool setFramerate(double framerate);


		//set capture format
		bool setFormat(unsigned int width,unsigned int height, double framerate);

		//get capture format
		void getFormat(unsigned int &width,unsigned int &height, double &framerate);

		//Get the available capture formats
		bool getStreamCapabilities();

		//Set capture MediaType
		bool setMediaType(int mediaTypeID);

		//Get current capture MediaType
		int getMediaType();

};
#endif
#endif
