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
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "attribute_categories.h"
#include "core/compare.h"
#include "core/config_options.h"
#include "core/core_math.h"
#include "core/core_string.h"
#include "core/directory_parser.h"
#include "core/logger.h"
#include "core/vec.h"
#include "core/version.h"
#include "edcs_converter.h"
#include "farm_data_types.h"
#include "farm_enumerant.h"
#include "feature_categories.h"

namespace
{
    // Maps old EDCS feature label to new EDCS feature label.
    //
    typedef std::map< FARM::FeatureLabel, FARM::FeatureLabel >
        EDCSFeatureMapping;

    // New EDCS enumerant values are defined as a pair consisting of an
    // attribute label and a union value.
    //
    typedef std::pair<FARM::AttributeLabel, FARM::AttributeDataValue>
        AttributeValuePair;

    // Maps old EDCS attribute values to new EDCS attribute values
    // Attribute data types may have changed from one version of
    // EDCS to another.
    //
    typedef std::map< FARM::AttributeEnumPair, AttributeValuePair >
        EDCSAttributeValueMapping;

    // Maps old EDCS attribute label to new EDCS attribute label based
    // on old enum value
    //
    typedef std::map< FARM::AttributeEnumPair, FARM::AttributeLabel >
        EDCSAttributeMappingWithEnum;

    EDCSFeatureMapping
        edcs_feature_map;
    EDCSAttributeValueMapping
        edcs_attribute_value_map;
    FARM::AttributeLabelsToDataTypes
        old_attribute_labels_to_types;  // Stores the mapping of old attribute
                                        // labels to respective old data types
    EDCSAttributeMappingWithEnum
        edcs_attrib_with_enum_map;
    FARM::AttributeLabelsToDataTypes
        attribute_labels_to_types;      // Stores the mapping of attribute
                                        // labels to respective data types

    // Forward declaring these debug functions so we can mark them to prevent
    // GCC from throwing a warning for them.
    void dump_edcs_value_map()  __attribute__ ((unused));
    void dump_edcs_attribute_with_enum_map() __attribute__ ((unused));
    void dump_edcs_feature_map() __attribute__ ((unused));

    // ------------------------------------------------------------------------
    void dump_edcs_feature_map()
    {
        std::cout << "ENTER dump_edcs_feature_map" << std::endl;

        int
            count = 1;

        for (EDCSFeatureMapping::const_iterator
            map_itr = edcs_feature_map.begin();
            map_itr != edcs_feature_map.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }

        std::cout << "EXIT dump_edcs_feature_map" << std::endl;
    }

    // ------------------------------------------------------------------------
    void dump_edcs_attribute_with_enum_map()
    {
        std::cout << "ENTER dump_edcs_attribute_with_enum_map" << std::endl;

        int
            count = 1;

        for (EDCSAttributeMappingWithEnum::const_iterator
            map_itr = edcs_attrib_with_enum_map.begin();
            map_itr != edcs_attrib_with_enum_map.end();
            ++map_itr)
        {
            FARM::AttributeEnumPair
                key = map_itr->first;

            std::cout << count << ") ['" << key.first << "', '" ;
            std::cout << key.second << "'], " ;
            std::cout << map_itr->second << std::endl;

            ++count;
        }

        std::cout << "EXIT dump_edcs_attribute_with_enum_map" << std::endl;
    }

    // ------------------------------------------------------------------------
    void dump_edcs_value_map()
    {
        std::cout << "ENTER dump_edcs_value_map" << std::endl;

        int
            count = 1;
        FARM::AttributeDataType
            new_data_type;

        for (EDCSAttributeValueMapping::const_iterator
            map_itr = edcs_attribute_value_map.begin();
            map_itr != edcs_attribute_value_map.end();
            ++map_itr)
        {
            FARM::AttributeEnumPair
                old_attribute_value = map_itr->first;
            AttributeValuePair
                new_attribute_value = map_itr->second;

            FARM::AttributeLabelsToDataTypes::iterator
                datatype_map_iter =
                    attribute_labels_to_types.find(new_attribute_value.first);

            if (datatype_map_iter != attribute_labels_to_types.end())
            {
                new_data_type = datatype_map_iter->second;
            }
            else
            {
                // Could be a deleted type, in which case the new attribute label
                // would not be available in the edcs_attribute_value_map (above).
                // Use the old attribute label to get the data type.

                FARM::AttributeLabelsToDataTypes::iterator
                    datatype_map_iter2 =
                        attribute_labels_to_types.find(old_attribute_value.first);

                ASSERT(
                    datatype_map_iter2 != attribute_labels_to_types.end(),
                    fatal,
                    "Could not get the data type for '" +
                        new_attribute_value.first + "'.");

                if (datatype_map_iter2 != attribute_labels_to_types.end())
                {
                    new_data_type = datatype_map_iter2->second;
                }
            }

            std::cout << count << ") [" << old_attribute_value.first << ", " ;
            std::cout << old_attribute_value.second;
            std::cout << "], [" << new_attribute_value.first << ", " ;

            switch (new_data_type)
            {
                case FARM::no_data_type:
                {
                    std::cout << "Invalid Data Value";
                    break;
                }

                case FARM::int32:
                {
                    std::cout << new_attribute_value.second.int32;
                    break;
                }

                case FARM::float64:
                {
                    std::cout << new_attribute_value.second.float64;
                    break;
                }

                case FARM::string:
                {
                    std::cout << new_attribute_value.second.str;
                    break;
                }

                case FARM::enumeration:
                {
                    std::cout << new_attribute_value.second.enumeration;
                    break;
                }

                case FARM::boolean:
                {
                    std::cout << new_attribute_value.second.boolean;
                    break;
                }

                default:
                {
                    std::cout << "Invalid Data Type";
                    break;
                }
            }

            std::cout << "]" << std::endl;

            ++count;
        }

        std::cout << "EXIT dump_edcs_value_map" << std::endl;
    }
}

namespace FARM
{
    bool
        EDCSConverter::converter_initialized = false;

    // ------------------------------------------------------------------------
    // Reads the EDCS mapping files for features, attributes, and enums and
    // creates the internal maps for each.
    //
    bool EDCSConverter::initialize(
        const std::string &edcs_feature_mapping_filename,
        const std::string &edcs_attribute_mapping_filename,
        const std::string &edcs_attrib_enum_mapping_filename
    )
    {
        FeatureLabel
            old_feature_label,
            new_feature_label;

        if (not initialized())
        {
            // Initialize the EDCS mapping tables.

            edcs_feature_map.clear();
            edcs_attrib_with_enum_map.clear();
            edcs_attribute_value_map.clear();
            old_attribute_labels_to_types.clear();
            attribute_labels_to_types.clear();

            // Read in Feature Label Mapping data and set up map

            CORE::ConfigurationOptions
                *edcs_config_options_ptr = 0;
            edcs_config_options_ptr =
                new CORE::ConfigurationOptions(edcs_feature_mapping_filename);

            ASSERT(
                edcs_config_options_ptr,
                fatal,
                "Could not allocate the memory for the ConfigurationOptions "
                    "class!");

            ASSERT(
                edcs_config_options_ptr->is_ready(),
                fatal,
                "Could not parse the edcs feature mapping configuration file " +
                edcs_feature_mapping_filename);

            CORE::VecofCells
                edcs_feature_table,
                edcs_feature_entry;

            // Get the feature mapping table.
            //
            ASSERT(
                edcs_config_options_ptr->value(
                    "FeatureMapping", edcs_feature_table),
                high,
                "Unable to get edcs feature mapping.");

            // Iterate through the entries.

            for (int i = 0; i < edcs_feature_table.size(); ++i)
            {
                ASSERT(edcs_config_options_ptr->value(edcs_feature_table, i, edcs_feature_entry),
                    high,
                    "Unable to get entry from edcs feature mapping");

                // Get the old feature label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_feature_entry, 0, old_feature_label),
                    high,
                    "Unable to get old feature label from edcs feature mapping entry");

                // Get the new feature label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_feature_entry, 1, new_feature_label),
                    high,
                    "Unable to get new feature label from edcs feature mapping entry");

                edcs_feature_map.insert(
                    EDCSFeatureMapping::value_type(old_feature_label, new_feature_label));
            }

            //for debug
            //dump_edcs_feature_map();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }

            // Read in Attribute Mapping data and set up map

            AttributeLabel
                old_attribute_label,
                new_attribute_label;

            AttributeEnumPair
                old_attribute_value;

            EnumerantLabel
                old_enumerant_label;

            AttributeDataType
                old_data_type,
                new_data_type;

            std::string
                old_data_type_string,
                new_data_type_string;

            edcs_config_options_ptr =
                new CORE::ConfigurationOptions(edcs_attribute_mapping_filename);

            ASSERT(
                edcs_config_options_ptr,
                fatal,
                "Could not allocate the memory for the ConfigurationOptions "
                    "class!");

            ASSERT(
                edcs_config_options_ptr->is_ready(),
                fatal,
                "Could not parse the edcs attribute mapping configuration file " +
                edcs_attribute_mapping_filename);

            CORE::VecofCells
                edcs_attribute_table,
                edcs_attribute_entry;

            // Get the attribute mapping table.
            //
            ASSERT(
                edcs_config_options_ptr->value(
                    "AttributeMapping", edcs_attribute_table),
                high,
                "Unable to get edcs attribute mapping.");

            // Iterate through the entries.
            //
            //  ( old_attr_code,  old_data_type,  old_attr_label,
            //    new_attr_code,  new_data_type,  new_attr_label)

            for (int i = 0; i < edcs_attribute_table.size(); ++i)
            {
                old_data_type = no_data_type;
                new_data_type = no_data_type;
                old_data_type_string = "";
                new_data_type_string = "";
                old_attribute_label = "";
                new_attribute_label = "";
                old_enumerant_label = "";

                old_attribute_value =
                    AttributeEnumPair(old_attribute_label, old_enumerant_label);

                ASSERT(edcs_config_options_ptr->value(edcs_attribute_table, i, edcs_attribute_entry),
                    high,
                    "Unable to get entry from edcs attribute mapping");

                // Get the old attribute data type.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_attribute_entry, 1, old_data_type_string),
                    high,
                    "Unable to get old data type from edcs attribute mapping entry");

                // Get the old attribute label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_attribute_entry, 2, old_attribute_label),
                    high,
                    "Unable to get old attribute label from edcs attribute mapping entry");

                old_attribute_value.first = old_attribute_label;

                // Get the new attribute data type.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_attribute_entry, 4, new_data_type_string),
                    high,
                    "Unable to get new data type from edcs attribute mapping entry");

                // Get the new attribute label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_attribute_entry, 5, new_attribute_label),
                    high,
                    "Unable to get new attribute label from edcs attribute mapping entry");

                // Set the old attribute datatype

                if (old_data_type_string == "ENUMERATION" or old_data_type_string == "ENUM")
                {
                    old_data_type = enumeration;
                }
                else if (old_data_type_string == "BOOLEAN" or old_data_type_string == "BOOL")
                {
                    old_data_type = boolean;
                }
                else if (old_data_type_string == "STRING" or old_data_type_string == "CONSTRAINED_STRING")
                {
                    old_data_type = string;
                }
                else if (old_data_type_string == "INTEGER")
                {
                    old_data_type = int32;
                }
                else if (old_data_type_string == "REAL")
                {
                    old_data_type = float64;
                }
                else if (old_data_type_string == "UUID")
                {
                    old_data_type = uuid;
                }

                old_attribute_labels_to_types.insert(
                    AttributeLabelsToDataTypes::value_type(old_attribute_label, old_data_type));

                // Store the attribute mappings except for those of enumeration type.

                if (old_data_type != enumeration)
                {
                    // Get the new attribute value (may have changed)

                    if (new_data_type_string == "DELETED" or new_data_type_string == "REMOVED")
                    {
                        new_data_type = deleted;
                        new_attribute_label = old_attribute_label;
                    }
                    else if (new_data_type_string == "ENUM" or new_data_type_string == "ENUMERATION")
                    {
                        new_data_type = enumeration;
                    }
                    else if (new_data_type_string == "BOOLEAN" or new_data_type_string == "BOOL")
                    {
                        new_data_type = boolean;
                    }
                    else if (new_data_type_string == "STRING" or new_data_type_string == "CONSTRAINED_STRING")
                    {
                        new_data_type = string;
                    }
                    else if (new_data_type_string == "INTEGER")
                    {
                        new_data_type = int32;
                    }
                    else if (new_data_type_string == "REAL")
                    {
                        new_data_type = float64;
                    }

                    edcs_attrib_with_enum_map.insert(
                        EDCSAttributeMappingWithEnum::value_type(
                            old_attribute_value, new_attribute_label));

                    attribute_labels_to_types.insert(
                        AttributeLabelsToDataTypes::value_type(
                            new_attribute_label, new_data_type));
                }
            }

            //for debug
            //dump_attribute_labels_to_types(old_attribute_labels_to_types);
            //dump_attribute_labels_to_types(attribute_labels_to_types);
            //dump_edcs_attribute_with_enum_map();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }

            // Read in Enumerant Mapping data and set up map

            CORE::Int32
                old_enumerant_code,
                new_enumerant_code;

            AttributeValuePair
                new_attribute_value;

            AttributeDataValue
                new_data_value;
            std::string
                mapping_type,
                tmp_string;

            edcs_config_options_ptr =
                new CORE::ConfigurationOptions(edcs_attrib_enum_mapping_filename);

            ASSERT(
                edcs_config_options_ptr,
                fatal,
                "Could not allocate the memory for the ConfigurationOptions "
                    "class!");

            ASSERT(
                edcs_config_options_ptr->is_ready(),
                fatal,
                "Could not parse the edcs enumerant mapping configuration file " +
                edcs_attrib_enum_mapping_filename);

            CORE::VecofCells
                edcs_enum_table,
                edcs_enum_entry;

            // Get the enumerant mapping table.
            //
            ASSERT(edcs_config_options_ptr->value("EnumerantMapping", edcs_enum_table),
                high, "Unable to get edcs enumerant mapping");

            // Iterate through the entries.
            //
            // 0 old_attr_code, 1 old_enum_code, 2 old_attr_label, 3 old_enum_label,
            // 4 mapping_type,  5 data_type,
            // 6 new_attr_code, 7 new_enum_code, 8 new_attr_label, 9 new_enum_label

            for (int i = 0; i < edcs_enum_table.size(); ++i)
            {
                old_attribute_label = "";
                new_attribute_label = "";
                new_data_type = no_data_type;
                new_data_type_string = "";
                tmp_string = "";

                old_enumerant_code = -999;
                new_enumerant_code = -999;

                mapping_type = "";
                old_enumerant_label = "";
                std::strcpy(new_data_value.enumeration, "");

                old_attribute_value =
                    AttributeEnumPair(old_attribute_label, old_enumerant_label);
                new_attribute_value =
                    AttributeValuePair(new_attribute_label, new_data_value);

                ASSERT(edcs_config_options_ptr->value(edcs_enum_table, i, edcs_enum_entry),
                    high,
                    "Unable to get entry from edcs enum mapping");

                // Get the old enumerant code.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 1, old_enumerant_code),
                    high,
                    "Unable to get new enum code from edcs enum mapping entry");

                // Get the old attribute label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 2, old_attribute_label),
                    high,
                    "Unable to get old attribute label from edcs enum mapping entry");

                old_attribute_value.first = old_attribute_label;

                // Get the old enumerant label.
                //
                ASSERT_WITH_STREAM(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 3, old_enumerant_label),
                    high,
                    "Unable to get old enum label from edcs enum mapping "
                        "entry at row " << i << ".");

                old_attribute_value.second = old_enumerant_label;

                // Get the mapping type.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 4, mapping_type),
                    high,
                    "Unable to get mapping type from edcs enum mapping entry");

                // Get the new attribute data type.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 5, new_data_type_string),
                    high,
                    "Unable to get new data type from edcs enum mapping entry");

                // Get the new enumerant code.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 7, new_enumerant_code),
                    high,
                    "Unable to get new enum code from edcs enum mapping entry");

                // Get the new attribute label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_enum_entry, 8, new_attribute_label),
                    high,
                    "Unable to get new attribute label from edcs enum mapping entry");

                new_attribute_value.first = new_attribute_label;

                // Get the new attribute value (may have changed from enumerant
                // to something else).

                if (mapping_type == "DELETED" or mapping_type == "REMOVED")
                {
                    new_data_type = deleted;
                    new_attribute_label = old_attribute_label;
                }
                else if (new_data_type_string == "ENUM" or new_data_type_string == "ENUMERATION")
                {
                    new_data_type = enumeration;

                    ASSERT(edcs_config_options_ptr->value(
                        edcs_enum_entry, 9, tmp_string),
                        high,
                        "Unable to get new enum label from edcs enum mapping entry");

                    // store the label
                    //
                    std::strcpy(
                        new_data_value.enumeration, tmp_string.c_str());
                }
                else if (new_data_type_string == "BOOLEAN" or new_data_type_string == "BOOL")
                {
                    ASSERT(
                        edcs_config_options_ptr->value(edcs_enum_entry, 9, tmp_string),
                        high,
                        "Unable to get new enum label from edcs enum mapping entry");

                    new_data_type = boolean;
                    new_data_value.boolean = (tmp_string == "TRUE" ? true : false);
                }
                else if (new_data_type_string == "STRING" or new_data_type_string == "CONSTRAINED_STRING")
                {
                    new_data_type = string;

                    ASSERT(
                        edcs_config_options_ptr->value(edcs_enum_entry, 9, tmp_string),
                        high,
                        "Unable to get new enum label from edcs enum mapping entry");

                    std::strcpy(new_data_value.str, tmp_string.c_str());
                }

                else if (new_data_type_string == "INTEGER")
                {
                    new_data_type = int32;

                    ASSERT(
                        edcs_config_options_ptr->value(edcs_enum_entry, 9, new_data_value.int32),
                        high,
                        "Unable to get new enum label from edcs enum mapping entry");
                }
                else if (new_data_type_string == "REAL")
                {
                    new_data_type = float64;

                    ASSERT_WITH_STREAM(
                        edcs_config_options_ptr->value(
                            edcs_enum_entry, 9, new_data_value.float64),
                        high,
                        "Unable to get new enum label from edcs enum mapping "
                            "entry at row " << i << ".");
                }

                new_attribute_value.second = new_data_value;

                edcs_attribute_value_map.insert(
                    EDCSAttributeValueMapping::value_type(old_attribute_value, new_attribute_value));

                // Add the attribute mappings that come out of the enumeration mapping file

                edcs_attrib_with_enum_map.insert(
                    EDCSAttributeMappingWithEnum::value_type(old_attribute_value, new_attribute_label));

                attribute_labels_to_types.insert(
                    AttributeLabelsToDataTypes::value_type(new_attribute_label, new_data_type));
            }

            //for debug
            //dump_attribute_labels_to_types(attribute_labels_to_types);
            //dump_edcs_value_map();
            //dump_edcs_attribute_with_enum_map();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }
        }

        converter_initialized =
            edcs_feature_map.size() and
            edcs_attrib_with_enum_map.size() and
            edcs_attribute_value_map.size() and
            old_attribute_labels_to_types.size() and
            attribute_labels_to_types.size();

        if (converter_initialized)
        {
            converter_initialized =
                FeatureAttributeMapping::initialize_edcs_maps(
                    edcs_feature_mapping_filename,
                    edcs_attribute_mapping_filename,
                    edcs_attrib_enum_mapping_filename);
        }

        return initialized();
    }

    // ------------------------------------------------------------------------
    void EDCSConverter::destroy(void)
    {
        if (initialized())
        {
            edcs_feature_map.clear();
            edcs_attrib_with_enum_map.clear();
            edcs_attribute_value_map.clear();
            old_attribute_labels_to_types.clear();
            attribute_labels_to_types.clear();

            converter_initialized = false;
        }
    }

    // ------------------------------------------------------------------------
    bool EDCSConverter::convert_feature(
        const FeatureLabel &old_label,
        FeatureLabel &new_label
    )
    {
        bool
            status = false;

        // Get the new label from map

        EDCSFeatureMapping::iterator
            feature_map_iter = edcs_feature_map.find(old_label);

        status = feature_map_iter != edcs_feature_map.end();

        ASSERT(status,
            fatal,
            "Could not get the new edcs feature label for '" + old_label + "'.");

        if (status)
        {
            new_label = feature_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool EDCSConverter::convert_value(
        const AttributeLabel &old_label,
        const EnumerantLabel &old_data_value,
        MappingType &mapping_flag,
        AttributeLabel &new_label,
        AttributeDataType &new_data_type,
        AttributeDataValue &new_data_value
    )
    {
        bool
            status = false;

        AttributeDataType
            old_data_type;

        mapping_flag = not_mapped;  // no mapping exists (default)

        // First get the new attribute label from map

        AttributeEnumPair
            old_attribute_value(old_label, old_data_value);

        EDCSAttributeMappingWithEnum::iterator
            attrib_map_iter =
                edcs_attrib_with_enum_map.find(old_attribute_value);

        status = attrib_map_iter != edcs_attrib_with_enum_map.end();

        ASSERT(status,
            fatal,
            "Could not get the new edcs attribute label for [" +
            old_attribute_value.first + ", " +
            old_attribute_value.second + "].");

        if (status)
        {
            new_label = attrib_map_iter->second;

            // Next get the new data type from map

            AttributeLabelsToDataTypes::iterator
                datatype_map_iter = attribute_labels_to_types.find(new_label);

            status = datatype_map_iter != attribute_labels_to_types.end();

            ASSERT(status,
                fatal,
                "Could not get the new edcs datatype for '" + new_label + "'.");

            if (status)
            {
                mapping_flag = mapped;     // one to one mapping exists
                new_data_type = datatype_map_iter->second;

                status = FARM::EDCSConverter::get_3p1_attribute_datatype(
                    old_label, old_data_type);

                if (old_data_type == enumeration and new_data_type != deleted)
                {
                    // Lastly get the new data value from map

                    EDCSAttributeValueMapping::iterator
                        map_itr = edcs_attribute_value_map.find(old_attribute_value);

                    status = map_itr != edcs_attribute_value_map.end();

                    ASSERT(status,
                        fatal,
                        "Could not get the new edcs attribute value for '[" +
                            old_label + ", " + old_data_value + "]'.");

                    if (status)
                    {
                        new_data_value = map_itr->second.second;
                    }
                    else
                    {
                        mapping_flag = not_mapped;  // no mapping exists
                    }
                }
            }
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool EDCSConverter::get_3p1_attribute_datatype(
        const AttributeLabel &old_label,
        AttributeDataType &old_datatype
    )
    {
        AttributeLabelsToDataTypes::iterator
            datatype_map_iter = old_attribute_labels_to_types.find(old_label);

        bool
            status = datatype_map_iter != old_attribute_labels_to_types.end();

        if (status)
        {
            old_datatype = datatype_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool EDCSConverter::valid_old_enum(
        const AttributeLabel &old_attribute_label,
        const EnumerantLabel &old_enumerant_value
    )
    {
        AttributeEnumPair
            old_attribute_value(old_attribute_label, old_enumerant_value);

        EDCSAttributeValueMapping::iterator
            map_itr = edcs_attribute_value_map.find(old_attribute_value);

        return map_itr != edcs_attribute_value_map.end();
    }

    // ------------------------------------------------------------------------
    void EDCSConverter::dump_attribute_labels_to_types(
        const AttributeLabelsToDataTypes &label_to_type_map
    )
    {
        std::cout << "ENTER dump_attribute_labels_to_types" << std::endl;

        int
            count = 1;

        for (AttributeLabelsToDataTypes::const_iterator
            map_itr = label_to_type_map.begin();
            map_itr != label_to_type_map.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }

        std::cout << "EXIT dump_attribute_labels_to_types" << std::endl;
    }
}
