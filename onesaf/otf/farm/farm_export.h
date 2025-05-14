/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */

#ifndef FARMEXPORT_H
#define FARMEXPORT_H

#ifdef FARM_DLL_EXPORT
  #define FARMEXPORT __declspec(dllexport)
#else
  #define FARMEXPORT
#endif

#endif

