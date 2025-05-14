/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
/*
 * Classification:  Unclassified
 *
 * Project Name:  OneSAF
 *
 * DISTRIBUTION STATEMENT C.
 *
 * Distribution is authorized to U.S. Government Agencies and their
 * contractors. Export Controlled: 23 August 2012. Other requests for
 * this document shall be referred to U.S. Army PEO STRI.
 *
 */
#ifndef ENUM_VALUES_H
#define ENUM_VALUES_H
#include "farm.h"

namespace FARM
{
    class EnumValues
    {
      public:

        // Precompute and store certain enumeration values.  The FARM class
        // must be intialized before this is called.
        //
        // Return:  Were the enumeration values initialized successfully?
        //
        static bool initialize();

        // Return:  Enumeration value for 'PERENNIAL_OR_PERMANENT'.
        //
        static Enumerant perennial_or_permanent();

      private:

        static bool
            initialized; // Has this class been initialized?

        // If the category elements have not been initialized report an error.
        // Used to factor out the test and error report from the various
        // methods.
        //
        static void validate();

        static Enumerant
            my_perennial_or_permanent;
    };

    // ------------------------------------------------------------------------
    inline void EnumValues::validate()
    {
        if (not initialized)
        {
            LOG(fatal, "EnumValues used before initialization.");
        }
    }

    // ------------------------------------------------------------------------
    inline Enumerant EnumValues::perennial_or_permanent()
    {
        validate();

        return my_perennial_or_permanent;
    }
}

#endif

