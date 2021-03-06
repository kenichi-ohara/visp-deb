/****************************************************************************
 *
 * $Id: servoAfma6FourPoints2DArtVelocity.cpp 4056 2013-01-05 13:04:42Z fspindle $
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
 *   tests the control law
 *   eye-in-hand control
 *   velocity computed in the articular frame
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/
/*!
  \file servoAfma6FourPoints2DArtVelocity.cpp

  \brief Example of eye-in-hand control law. We control here a real robot, the
  Afma6 robot (cartesian robot, with 6 degrees of freedom). The velocity is
  computed in articular.  Visual features are the image coordinates of 4 vdot
  points.

*/


/*!
  \example servoAfma6FourPoints2DArtVelocity.cpp

  Example of eye-in-hand control law. We control here a real robot, the Afma6
  robot (cartesian robot, with 6 degrees of freedom). The velocity is computed
  in articular.  Visual features are the image coordinates of 4 vdot points.

*/



#include <visp/vpConfig.h>
#include <visp/vpDebug.h> // Debug trace
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#if (defined (VISP_HAVE_AFMA6) && defined (VISP_HAVE_DC1394_2))

#include <visp/vp1394TwoGrabber.h>
#include <visp/vpImage.h>
#include <visp/vpImagePoint.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayGTK.h>

#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpPoint.h>
#include <visp/vpServo.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpIoTools.h>
#include <visp/vpRobotAfma6.h>

// Exception
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>
#include <visp/vpServoDisplay.h>

#include <visp/vpDot.h>

int
main()
{
  // Log file creation in /tmp/$USERNAME/log.dat
  // This file contains by line:
  // - the 6 computed joint velocities (m/s, rad/s) to achieve the task
  // - the 6 mesured joint velocities (m/s, rad/s)
  // - the 6 mesured joint positions (m, rad)
  // - the 8 values of s - s*
  std::string username;
  // Get the user login name
  vpIoTools::getUserName(username);

  // Create a log filename to save velocities...
  std::string logdirname;
  logdirname ="/tmp/" + username;

  // Test if the output path exist. If no try to create it
  if (vpIoTools::checkDirectory(logdirname) == false) {
    try {
      // Create the dirname
      vpIoTools::makeDirectory(logdirname);
    }
    catch (...) {
      std::cerr << std::endl
                << "ERROR:" << std::endl;
      std::cerr << "  Cannot create " << logdirname << std::endl;
      exit(-1);
    }
  }
  std::string logfilename;
  logfilename = logdirname + "/log.dat";

  // Open the log file name
  std::ofstream flog(logfilename.c_str());

  try {
    // Define the square CAD model
    // Square dimention
#define L 0.075
    // Distance between the camera and the square at the desired
    // position after visual servoing convergence
#define D 0.5

    vpServo task ;

    vpImage<unsigned char> I ;
    int i ;

    vp1394TwoGrabber g;
    g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_640x480_MONO8);
    g.setFramerate(vp1394TwoGrabber::vpFRAMERATE_60);
    g.open(I) ;

    g.acquire(I) ;

#ifdef VISP_HAVE_X11
    vpDisplayX display(I,100,100,"Current image") ;
#elif defined(VISP_HAVE_OPENCV)
    vpDisplayOpenCV display(I,100,100,"Current image") ;
#elif defined(VISP_HAVE_GTK)
    vpDisplayGTK display(I,100,100,"Current image") ;
#endif

    vpDisplay::display(I) ;
    vpDisplay::flush(I) ;

    std::cout << std::endl ;
    std::cout << "-------------------------------------------------------" << std::endl ;
    std::cout << " Test program for vpServo "  <<std::endl ;
    std::cout << " Eye-in-hand task control, velocity computed in the joint space" << std::endl ;
    std::cout << " Use of the Afma6 robot " << std::endl ;
    std::cout << " task : servo 4 points on a square with dimention " << L << " meters" << std::endl ;
    std::cout << "-------------------------------------------------------" << std::endl ;
    std::cout << std::endl ;


    vpDot dot[4] ;
    vpImagePoint cog;

    std::cout << "Click on the 4 dots clockwise starting from upper/left dot..."
              << std::endl;

    for (i=0 ; i < 4 ; i++) {
      dot[i].initTracking(I) ;
      cog = dot[i].getCog();
      vpDisplay::displayCross(I, cog, 10, vpColor::blue) ;
      vpDisplay::flush(I);
    }

    vpRobotAfma6 robot ;

    vpCameraParameters cam ;

    // Update camera parameters
    robot.getCameraParameters (cam, I);

    // Sets the current position of the visual feature
    vpFeaturePoint p[4] ;
    for (i=0 ; i < 4 ; i++)
      vpFeatureBuilder::create(p[i],cam, dot[i])  ;  //retrieve x,y and Z of the vpPoint structure

    // sets the desired position of the visual feature
    vpFeaturePoint pd[4] ;

    pd[0].buildFrom(-L,-L,D) ;
    pd[1].buildFrom(L,-L,D) ;
    pd[2].buildFrom(L,L,D) ;
    pd[3].buildFrom(-L,L,D) ;

    // We want to see a point on a point
    std::cout << std::endl ;
    for (i=0 ; i < 4 ; i++)
      task.addFeature(p[i],pd[i]) ;

    // Set the proportional gain
    task.setLambda(0.2) ;

    // Display task information
    task.print() ;

    // Define the task
    // - we want an eye-in-hand control law
    // - articular velocity are computed
    task.setServo(vpServo::EYEINHAND_L_cVe_eJe) ;
    task.setInteractionMatrixType(vpServo::DESIRED, vpServo::PSEUDO_INVERSE) ;
    task.print() ;

    vpVelocityTwistMatrix cVe ;
    robot.get_cVe(cVe) ;
    task.set_cVe(cVe) ;
    task.print() ;

    // Set the Jacobian (expressed in the end-effector frame)
    vpMatrix eJe ;
    robot.get_eJe(eJe) ;
    task.set_eJe(eJe) ;
    task.print() ;

    // Initialise the velocity control of the robot
    robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL) ;

    std::cout << "\nHit CTRL-C to stop the loop...\n" << std::flush;
    for ( ; ; ) {
      // Acquire a new image from the camera
      g.acquire(I) ;

      // Display this image
      vpDisplay::display(I) ;

      try {
        // For each point...
        for (i=0 ; i < 4 ; i++) {
          // Achieve the tracking of the dot in the image
          dot[i].track(I) ;
          // Get the dot cog
          cog = dot[i].getCog();
          // Display a green cross at the center of gravity position in the
          // image
          vpDisplay::displayCross(I, cog, 10, vpColor::green) ;
        }
      }
      catch(...) {
        flog.close() ; // Close the log file
        vpTRACE("Error detected while tracking visual features") ;
        robot.stopMotion() ;
        exit(1) ;
      }

      // Update the point feature from the dot location
      for (i=0 ; i < 4 ; i++)
        vpFeatureBuilder::create(p[i],cam, dot[i]);

      // Get the jacobian of the robot
      robot.get_eJe(eJe) ;
      // Update this jacobian in the task structure. It will be used to compute
      // the velocity skew (as an articular velocity)
      // qdot = -lambda * L^+ * cVe * eJe * (s-s*)
      task.set_eJe(eJe) ;

      vpColVector v ;
      // Compute the visual servoing skew vector
      v = task.computeControlLaw() ;

      // Display the current and desired feature points in the image display
      vpServoDisplay::display(task,cam,I) ;

      // Apply the computed joint velocities to the robot
      robot.setVelocity(vpRobot::ARTICULAR_FRAME, v) ;

      // Save velocities applied to the robot in the log file
      // v[0], v[1], v[2] correspond to joint translation velocities in m/s
      // v[3], v[4], v[5] correspond to joint rotation velocities in rad/s
      flog << v[0] << " " << v[1] << " " << v[2] << " "
           << v[3] << " " << v[4] << " " << v[5] << " ";

      // Get the measured joint velocities of the robot
      vpColVector qvel;
      robot.getVelocity(vpRobot::ARTICULAR_FRAME, qvel);
      // Save measured joint velocities of the robot in the log file:
      // - qvel[0], qvel[1], qvel[2] correspond to measured joint translation
      //   velocities in m/s
      // - qvel[3], qvel[4], qvel[5] correspond to measured joint rotation
      //   velocities in rad/s
      flog << qvel[0] << " " << qvel[1] << " " << qvel[2] << " "
           << qvel[3] << " " << qvel[4] << " " << qvel[5] << " ";

      // Get the measured joint positions of the robot
      vpColVector q;
      robot.getPosition(vpRobot::ARTICULAR_FRAME, q);
      // Save measured joint positions of the robot in the log file
      // - q[0], q[1], q[2] correspond to measured joint translation
      //   positions in m
      // - q[3], q[4], q[5] correspond to measured joint rotation
      //   positions in rad
      flog << q[0] << " " << q[1] << " " << q[2] << " "
           << q[3] << " " << q[4] << " " << q[5] << " ";

      // Save feature error (s-s*) for the 4 feature points. For each feature
      // point, we have 2 errors (along x and y axis).  This error is expressed
      // in meters in the camera frame
      flog << ( task.getError() ).t() << std::endl;

      // Flush the display
      vpDisplay::flush(I) ;

      //	vpTRACE("\t\t || s - s* || = %f ", ( task.getError() ).sumSquare()) ;
    }

    vpTRACE("Display task information " ) ;
    task.print() ;
    task.kill();
    flog.close() ; // Close the log file
    return 0;
  }
  catch (...)
  {
    flog.close() ; // Close the log file
    vpERROR_TRACE(" Test failed") ;
    return 0;
  }
}

#else
int
main()
{
  vpERROR_TRACE("You do not have an afma6 robot or a firewire framegrabber connected to your computer...");

}

#endif
