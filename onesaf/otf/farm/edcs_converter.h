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
#ifndef EDCS_CONVERTER_H
#define EDCS_CONVERTER_H
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "farm_data_types.h"
#include "farm_attribute.h"
#include "farm_feature.h"
#include "farm_enumerant.h"
#include "farm.h"

namespace FARM
{
    // Stores the data value for an attribute.
    //
    union AttributeDataValue
    {
        int int32;
        double float64;
        char str[100];
        char enumeration[100];  // Store the enum label here instead of the enum code
        bool boolean;
    };

    // Used to flag whether or not a value was mapped.
    //
    enum MappingType
    {
        mapped,     // one to one mapping
        not_mapped  // one to zero mapping
    };

    // Maps attribute label to attribute data type.
    //
    typedef std::map< FARM::AttributeLabel, FARM::AttributeDataType >
        AttributeLabelsToDataTypes;

    class EDCSConverter
    {
      public:

        // Initializes maps from configuration files needed to convert to a
        // newer version of Sedris EDCS.
        //
        // Return:  Were the conversion maps created?
        //
        static bool initialize(
            const std::string &edcs_feature_mapping_filename,
            const std::string &edcs_attribute_mapping_filename,
            const std::string &edcs_attrib_enum_mapping_filename);

        // Releases any memory that was allocated for the edcs converter .
        //
        static void destroy(void);

        // Return:  Has the edcs converter been initialized?
        //
        static bool initialized(void);

        static void verify_edcs_converter_initialization(void);

        // Convert EDCS 3.x farm feature to EDCS 4.x farm feature.
        //
        static bool convert_feature(
            const FARM::FeatureLabel &old_label,
            FARM::FeatureLabel &new_label
        );

        // Convert EDCS 3.x farm attribute value to EDCS 4.x farm
        // attribute value.
        //
        static bool convert_value(
            const AttributeLabel &old_label,
            const EnumerantLabel &old_data_value,
            MappingType &mapping_flag,
            AttributeLabel &new_label,
            AttributeDataType &new_data_type,
            AttributeDataValue &new_data_value
        );

        // Returns the respective EDCS 3.x attribute datatype for the given
        // EDCS 3.x attribute label.
        //
        static bool get_3p1_attribute_datatype(
            const AttributeLabel &old_label,
            AttributeDataType &old_datatype
        );

        // Checks to see if a mapping exists for an EDCS 3.x farm attribute,
        // enum pair
        //
        static bool valid_old_enum(
            const AttributeLabel &old_attribute_label,
            const EnumerantLabel &old_enumerant_value
        );

        // Dumps attribute label to attribute data type mappings
        //
        static void dump_attribute_labels_to_types(
            const AttributeLabelsToDataTypes &label_to_type_map
        );

      private:

        static bool
            converter_initialized; // Has the edcs converter been initialized?
    };

    // ------------------------------------------------------------------------
    inline bool EDCSConverter::initialized( void )
    {
        return converter_initialized;
    }

    // ------------------------------------------------------------------------
    // Verifies that the edcs converter has been initialized.
    //
    inline void EDCSConverter::verify_edcs_converter_initialization(void)
    {
        ASSERT(
            FARM::EDCSConverter::initialized(),
            fatal,
            "The EDCSConverter was used before it was initialized!");
    }
}

#endif

