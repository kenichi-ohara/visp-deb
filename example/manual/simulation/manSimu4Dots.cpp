/****************************************************************************
 *
 * $Id: manSimu4Dots.cpp 4056 2013-01-05 13:04:42Z fspindle $
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
 * Description:
 * Simulation of a visual servoing with visualization and image generation.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file manSimu4Dots.cpp
  \brief Visual servoing experiment on 4 points with a visualization and image generation
  from the camera and from an external view using vpSimulator.
*/

/*!
  \example manSimu4Dots.cpp
  Visual servoing experiment on 4 points with a visualization and image generation
  from the camera and from an external view using vpSimulator.
*/

#include <visp/vpConfig.h>
#include <visp/vpDebug.h>


#if (defined(VISP_HAVE_COIN_AND_GUI) && (defined(VISP_HAVE_GTK) || defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI)))

#include <visp/vpImage.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpTime.h>
#include <visp/vpImage.h>
#include <visp/vpImageConvert.h>
#include <visp/vpSimulator.h>

#if defined(VISP_HAVE_X11)
#  include <visp/vpDisplayX.h>
#elif defined(VISP_HAVE_GDI)
#  include <visp/vpDisplayGDI.h>
#elif defined(VISP_HAVE_GTK)
#  include <visp/vpDisplayGTK.h>
#endif
// You may have strange compiler issues using the simulator based on SoQt 
// and the vpDisplayGTK. In that case prefer to use another display like
// vpDisplayX under linux or vpDisplayGDI under Windows
#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpPose.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpDot2.h>
#include <visp/vpServo.h>
#include <visp/vpServoDisplay.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpIoTools.h>

static
void *mainLoop (void *_simu)
{
  // pointer copy of the vpSimulator instance
  vpSimulator *simu = (vpSimulator *)_simu ;

  // Simulation initialization
  simu->initMainApplication() ;

  ///////////////////////////////////
  // Set the initial camera location
  vpHomogeneousMatrix cMo(0.3,0.2,3,
                          vpMath::rad(0),vpMath::rad(0),vpMath::rad(40));

  ///////////////////////////////////
  // Initialize the robot
  vpRobotCamera robot ;
  robot.setSamplingTime(0.04); // 40ms 
  robot.setPosition(cMo) ;
  // Send the robot position to the visualizator
  simu->setCameraPosition(cMo) ;
  // Initialize the camera parameters
  vpCameraParameters cam ;
  simu->getCameraParameters(cam);

  ////////////////////////////////////////
  // Desired visual features initialization

  // sets the points coordinates in the object frame (in meter)
  vpPoint point[4] ;
  point[0].setWorldCoordinates(-0.1,-0.1,0) ;
  point[1].setWorldCoordinates(0.1,-0.1,0) ;
  point[2].setWorldCoordinates(0.1,0.1,0) ;
  point[3].setWorldCoordinates(-0.1,0.1,0) ;

  // sets the desired camera location
  vpHomogeneousMatrix cMo_d(0,0,1,0,0,0) ;

  // computes the 3D point coordinates in the camera frame and its 2D coordinates
  for (int i = 0 ; i < 4 ; i++)
    point[i].project(cMo_d) ;

  // creates the associated features
  vpFeaturePoint pd[4] ;
  for (int i = 0 ; i < 4 ; i++)
    vpFeatureBuilder::create(pd[i],point[i]) ;



  ///////////////////////////////////////
  // Current visual features initialization
  unsigned int height = simu->getInternalHeight();
  unsigned int width = simu->getInternalWidth();

  // Create a greyscale image
  vpImage<unsigned char> I(height,width);

  //Display initialization
#if defined(VISP_HAVE_X11)
  vpDisplayX disp;
#elif defined(VISP_HAVE_GDI)
  vpDisplayGDI disp;
#elif defined(VISP_HAVE_GTK)
  vpDisplayGTK disp;
#endif
  disp.init(I,100,100,"Simulation display");
  //  disp(I);
  // Get the current image
  vpTime::wait(500); // wait to be sure the image is generated
  simu->getInternalImage(I);

  // Display the current image
  vpDisplay::display(I);
  vpDisplay::flush(I);

  // Initialize the four dots tracker
  std::cout << "A click in the four dots clockwise. " << std::endl;
  vpDot2 dot[4];
  vpFeaturePoint p[4]; 
  for (int i = 0 ; i < 4 ; i++)
  {
    dot[i].setGraphics(true);
    // Call for a click
    std::cout << "A click in the dot " << i << std::endl;
    dot[i].initTracking(I);
    // Create the associated feature 
    vpFeatureBuilder::create(p[i],cam,dot[i]);
    // flush the display
    vpDisplay::flush(I);
  }  


  /////////////////////////////////
  // Task defintion
  vpServo task ;
  // we want an eye-in-hand control law ;
  task.setServo(vpServo::EYEINHAND_L_cVe_eJe) ;
  task.setInteractionMatrixType(vpServo::DESIRED) ;

  // Set the position of the camera in the end-effector frame
  vpHomogeneousMatrix cMe ;
  vpVelocityTwistMatrix cVe(cMe) ;
  task.set_cVe(cVe) ;
  // Set the Jacobian (expressed in the end-effector frame)
  vpMatrix eJe ;
  robot.get_eJe(eJe) ;
  task.set_eJe(eJe) ;

  // we want to see a point on a point
  for (int i = 0 ; i < 4 ; i++)
    task.addFeature(p[i],pd[i]) ;
  // Set the gain
  task.setLambda(1.0) ;
  // Print the current information about the task
  task.print();

  vpTime::wait(500);

  ////////////////////////////////////////////////
  // The control loop
  int k = 0;
  while(k++ < 200){
    double t = vpTime::measureTimeMs();
    
    // Get the current internal camera view and display it
    simu->getInternalImage(I);
    vpDisplay::display(I);

    // Track the four dots and update the associated visual features
    for (int i = 0 ; i < 4 ; i++)
    {
      dot[i].track(I) ;
      vpFeatureBuilder::create(p[i],cam,dot[i]) ;
    }

    // Display the desired and current visual features
    vpServoDisplay::display(task,cam,I) ;
    vpDisplay::flush(I);
    
    // Update the robot Jacobian
    robot.get_eJe(eJe) ;
    task.set_eJe(eJe) ;
    
    // Compute the control law
    vpColVector v = task.computeControlLaw() ;
    
    // Send the computed velocity to the robot and compute the new robot position
    robot.setVelocity(vpRobot::ARTICULAR_FRAME, v) ;
    robot.getPosition(cMo) ;
    
    // Send the robot position to the visualizator
    simu->setCameraPosition(cMo) ;

    
    // Wait 40 ms
    vpTime::wait(t,40); 
  }  
  // Print information about the task
  task.print();
  task.kill();
  simu->closeMainApplication() ;

  void *a=NULL ;
  return a ;
}


int
main()
{
  vpSimulator simu ;

  // Internal view initialization : view from the robot camera
  simu.initInternalViewer(480, 360) ;
  // External view initialization : view from an external camera
  simu.initExternalViewer(300, 300) ; 

  // Inernal camera paramters initialization
  vpCameraParameters cam(800,800,240,180) ;
  simu.setInternalCameraParameters(cam) ;

  vpTime::wait(500) ;
  // Load the scene
  std::cout << "Load : ./4Points.iv" << std::endl
            << "This file should be in the working directory" << std::endl;

  simu.load("./4points.iv") ;

  // Run the main loop
  simu.initApplication(&mainLoop) ;
  // Run the simulator
  simu.mainLoop() ;
}

#else
int
main()
{  vpTRACE("You should install Coin3D and SoQT or SoWin or SoXt") ;

}
#endif
