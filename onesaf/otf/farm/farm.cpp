/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
/*
 * Copyrights 2006-2017
 *               Leidos
 *               The Aegis Technologies Group
 *               Cornerstone Software Solutions
 *               Cuspy Engineering Solutions
 *               Dignitas Technologies LLC
 *               GameSim Technologies
 *               Productivity Apex
 *               SoarTech Technology
 *               StackFrame LLC
 *               Lockheed Martin Corporation
 *               Northrop Grumman Corporation
 *               All rights reserved.
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
#include "enum_values.h"
#include "farm.h"
#include "farm_attribute.h"
#include "farm_data_types.h"
#include "farm_enumerant.h"
#include "feature_categories.h"

#define EDM_DEBUG 0

namespace
{
    const std::string
        // File the FARM is stored in for the terrain database.
        //
        farm_file_label = "farm.dat",
        // Binary file used to initialize the FARM inside of the terrain
        // compiler.  This binary file has a format that is simpler than the
        // FARM OTF format.  It is completely independent of the FARM OTF
        // format and is used to recompile a FARM OTF file from an older OTF
        // version to a newer OTF version.
        //
        farm_binary_input_file = "FARM.bin";

    // Constants for reading the FARM configuration files.
    //
    std::string
        farm_point = "POINT",
        farm_linear = "LINE",
        farm_areal = "AREA";

    bool
        edcs_maps_initialized = false;

    typedef std::map<FARM::FeatureLabelAndGeometry, FARM::FeatureCategory>
        FeatureLabelsGeometriesToCategories;
    typedef std::vector<FARM::Feature>
        FeatureCategoriesToFeatures;
    typedef std::map<FARM::EnumerantCode, FARM::Enumerant>
        AttributeEnums; // Stores the possible enumerations for an attribute.
    typedef std::map<FARM::AttributeCode, AttributeEnums>
        AttributeCodesToEnums;

    // Maps feature label to feature code.
    //
    typedef std::map< FARM::FeatureLabel, FARM::FeatureCode >
        FeatureLabelsToCodes;

    // Maps feature code to feature label.
    //
    typedef std::map< FARM::FeatureCode, FARM::FeatureLabel >
        FeatureCodesToLabels;

    // Maps attribute label to attribute code.
    //
    typedef std::map< FARM::AttributeLabel, FARM::AttributeCode >
        AttributeLabelsToCodes;

    // Maps attribute code to attribute label.
    //
    typedef std::map< FARM::AttributeCode, FARM::AttributeLabel >
        AttributeCodesToLabels;

    // Enumerant can be defined as a pair consisting of an
    // attribute label and an enumerant code.
    //
    typedef std::pair<FARM::AttributeLabel, FARM::EnumerantCode>
        AttributeEnumCodePair;

    // Maps enumerant values (attribute label and enumerant label combined)
    // to enumerant codes.
    //
    typedef std::map< FARM::AttributeEnumPair, FARM::EnumerantCode >
        EnumerantLabelsToCodes;

    // Maps enumerant codes (attribute label and enumerant code combined)
    // to enumerant labels.
    //
    typedef std::map< AttributeEnumCodePair, FARM::EnumerantLabel >
        EnumerantCodesToLabels;

    FeatureLabelsGeometriesToCategories
        feature_labels_and_geometries_to_categories; // Maps feature labels and
                                                     // geometries to feature
                                                     // categories.
    FeatureCategoriesToFeatures
        feature_categories_to_features; // Map Feature Categories to features.

    AttributeCodesToEnums
        attribute_codes_to_enums; // Stores the possible enumerations for
                                  // all of the enumerated attributes in
                                  // the database.

    // Caches attribute labels mapped to FARM attributes.  This data strucuture
    // is populated on the fly as attribute labels are converted to FARM
    // attributes.
    //
    std::map<FARM::AttributeLabel, FARM::Attribute>
        attribute_labels_to_attributes;

    FeatureLabelsToCodes
        feature_labels_to_codes;    // Stores the mapping of feature
                                    // labels to respective feature codes
    FeatureCodesToLabels
        feature_codes_to_labels;    // Stores the mapping of feature
                                    // codes to respective feature labels
    AttributeLabelsToCodes
        attribute_labels_to_codes;  // Stores the mapping of attribute
                                    // labels to respective attribute codes
    AttributeCodesToLabels
        attribute_codes_to_labels;  // Stores the mapping of attribute
                                    // codes to respective attribute labels
    EnumerantLabelsToCodes
        enum_labels_to_codes;       // Stores the mapping of enum labels
                                    // to respective enum codes
    EnumerantCodesToLabels
        enum_codes_to_labels;       // Stores the mapping of enum codes

    // Forward declaring these debug functions so we can mark them in order to prevent
    // GCC from throwing a warning for em.
    void dump_enum_labels_to_codes() __attribute__ ((unused));
    void dump_enum_codes_to_labels() __attribute__ ((unused));
    void dump_attribute_codes_to_labels() __attribute__ ((unused));
    void dump_attribute_labels_to_codes() __attribute__ ((unused));
    void dump_feature_codes_to_labels() __attribute__ ((unused));
    void dump_feature_labels_to_codes() __attribute__ ((unused));
    void local_read( std::istream&,FARM::Enumerant&) __attribute__ ((unused));
    void local_write(std::ostream&,const FARM::Enumerant&) __attribute__ ((unused));
    

    // ------------------------------------------------------------------------
    // Reads the next string (surrounded by double quotes) in a FARM
    // configuration file.
    //
    // Return:  Was the next string found?
    //
    bool get_next_string(
        std::istream &stream,
        std::string &next_string
    )
    {
        bool
            found_next_string,
            found_closing_quote;
        char
            input_char = stream.get();

        // Eat the white space.
        //
        while (input_char == ' ' or input_char == '\t' or input_char == '\n')
        {
            input_char = stream.get();
        }

        next_string.clear();

        found_next_string = input_char == '"';

        if (found_next_string)
        {
            // Read the next string.

            input_char = stream.get();

            found_closing_quote = input_char == '"';

            while (not found_closing_quote and not stream.eof())
            {
                next_string.append(1, input_char);

                input_char = stream.get();

                found_closing_quote = input_char == '"';
            }

            if (not found_closing_quote)
            {
                // The closing double quote was not found.
                //
                next_string.clear();
            }
        }

        return found_next_string and found_closing_quote;
    }

    // ------------------------------------------------------------------------
    // Reads the next number in a FARM configuration file.
    //
    // Return:  Was the next number found?
    //
    bool get_next_number(
        std::istream &stream,
        std::string &next_number
    )
    {
        bool
            found_next_number,
            found_period = false;
        char
            input_char = stream.get();

        // Eat the white space.
        //
        while (input_char == ' ' or input_char == '\t' or input_char == '\n')
        {
            input_char = stream.get();
        }

        next_number.clear();

        found_next_number =
            std::isdigit(input_char) or input_char == '-' or input_char == '+';

        if (found_next_number)
        {
            // Read the next number.
            //
            do
            {
                if (input_char == '.')
                {
                    ASSERT(
                        not found_period,
                        fatal,
                        "Found a number with multiple decimal points!");

                    found_period = true;
                }

                next_number.append(1, input_char);

                input_char = stream.get();
            }
            while (std::isdigit(input_char) or input_char == '.' );
        }

        if (not stream.eof())
        {
            // Put the last character back on the stream.
            //
            stream.putback(input_char);
        }

        return found_next_number;
    }

    // ------------------------------------------------------------------------
    // Return:  Is the token an integer (does not contain a decimal point)?
    //
    bool is_integer(const std::string &token)
    {
        return token.find(".", 0) == std::string::npos;
    }

    // ------------------------------------------------------------------------
    // If the token is a usage, then update the usage for the feature with the
    // proper usage bitmask.
    //
    // Return:  Was the token a usage?
    //
    bool updated_usage_bitmask(
        const std::string &token,
        FARM::Feature &feature
    )
    {
        bool
            found_usage = true;

        if (token == "avenue" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::avenue));
        }
        else if (token == "aperture" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::aperture));
        }
        else if (token == "building" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::building));
        }
        else if (token == "farm" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::agriculture_farm));
        }
        else if (token == "forest" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::forest));
        }
        else if (token == "furniture")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::furniture));
        }
        else if (token == "raisedCombatPos" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::raisedCombatPos));
        }
        else if (token == "dugInCombatPos" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::dugInCombatPos));
        }
        else if (token == "lane" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::lane));
        }
        else if (token == "multiBldg" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::multiBldg));
        }
        else if (token == "LFSmlVehObstacle" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::LFSmlVehObstacle));
        }
        else if (token == "vehObstacle" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::vehObstacle));
        }
        else if (token == "airVehObstacle" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::airVehObstacle));
        }
        else if (token == "urban" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() |
                FARM::urban));
        }
        else if (token == "NBC" )
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::NBC));
        }
        else if (token == "blocksLSmlVehLOS")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::blocksLSmlVehLOS));
        }
        else if (token == "blocksVehLOS")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::blocksVehLOS));
        }
        else if (token == "blocksLOS")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::blocksLOS));
        }
        else if (token == "protectsLSmlVeh")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::protectsLSmlVeh));
        }
        else if (token == "protectsVeh")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::protectsVeh));
        }
        else if (token == "bodyOfWater")
        {
            feature.set_usage_bitmask(static_cast<FARM::UsageBitmask>(
                feature.get_usage_bitmask() | FARM::bodyOfWater));
        }
        else
        {
            // The token was not a usage.
            //
            found_usage = false;
        }

        return found_usage;
    }

    // ------------------------------------------------------------------------
    // Reads a UInt16 from the binary stream.
    //
    void local_read(
        std::istream &stream,
        CORE::UInt16 &uint16
    )
    {
        stream.read(reinterpret_cast<char *>(&uint16), sizeof(uint16));
    }

    // ------------------------------------------------------------------------
    // Writes a UInt16 to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        CORE::UInt16 uint16
    )
    {
        stream.write(reinterpret_cast<char *>(&uint16), sizeof(uint16));
    }

    // ------------------------------------------------------------------------
    // Reads a string from the binary stream.
    //
    void local_read(
        std::istream &stream,
        std::string &string
    )
    {
        CORE::UInt16
            string_size;
        char
            padding;

        // Read the size of the string.
        //
        local_read(stream, string_size);

        string.resize(string_size);

        // Read the characters in the string.
        //
        stream.read(&(string[0]), string.size());

        if (CORE::odd(string.size()))
        {
            stream.read(&padding, sizeof(padding));
        }
    }

    // ------------------------------------------------------------------------
    // Writes a string to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        const std::string &string
    )
    {
        const char
            padding = 0;

        // Write the size of the string.
        //
        local_write(stream, static_cast<CORE::UInt16>(string.size()));

        // Write the characters in the string.
        //
        stream.write(&(string[0]), string.size());

        if (CORE::odd(string.size()))
        {
            stream.write(&padding, sizeof(padding));
        }
    }

    // ------------------------------------------------------------------------
    // Reads a feature label and geometry from the binary stream.
    //
    void local_read(
        std::istream &stream,
        FARM::FeatureLabelAndGeometry &label_and_geometry
    )
    {
        CORE::UInt16
            geometry;

        // Read the feature's label.
        //
        local_read(stream, label_and_geometry.first);

        // Read the feature's geometry.
        //
        local_read(stream, geometry);
        label_and_geometry.second =
            static_cast<FARM::FeatureGeometry>(geometry);
    }

    // ------------------------------------------------------------------------
    // Writes a feature label and geometry to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        const FARM::FeatureLabelAndGeometry &label_and_geometry
    )
    {
        // Write the feature's label.
        //
        local_write(stream, label_and_geometry.first);

        // Write the feature's geometry.
        //
        local_write(
            stream, static_cast<CORE::UInt16>(label_and_geometry.second));
    }

    // ------------------------------------------------------------------------
    // Writes a attribute to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        const FARM::Attribute &attribute
    )
    {
        // Write the attribute
        //
        attribute.write(stream);
    }

    // ------------------------------------------------------------------------
    // Reads a attribute from the binary stream.
    //
    void local_read(
        std::istream &stream,
        FARM::Attribute &attribute
    )
    {
        // Write the attribute
        //
        attribute.read(stream);
    }

    // ------------------------------------------------------------------------
    // Writes a feature to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        const FARM::Feature &feature
    )
    {
        // Write the feature
        //
        feature.write(stream);
    }

    // ------------------------------------------------------------------------
    // Reads a feature from the binary stream.
    //
    void local_read(
        std::istream &stream,
        FARM::Feature &feature
    )
    {
        // Write the feature
        //
        feature.read(stream);
    }

    // ------------------------------------------------------------------------
    // Writes a enumerant to the binary stream.
    //
    void local_write(
        std::ostream &stream,
        const FARM::Enumerant &enumerant
    )
    {
        // Write the enumerant
        //
        enumerant.write(stream);
    }

    // ------------------------------------------------------------------------
    // Reads a feature from the binary stream.
    //
    void local_read(
        std::istream &stream,
        FARM::Enumerant &enumerant
    )
    {
        // Write the enumerant
        //
        enumerant.read(stream);
    }

    // ------------------------------------------------------------------------
    // Reads an array from the binary stream.  You can specify the data types
    // stored in the array and the data types used in the stream to store the
    // array.
    //
    template<
        class DataType, // Data type for the elements in the array.
        class FileDataType> // Data type to store the array elements in the
                            // stream as.
    void read_array(
        std::istream &stream,
        std::vector<DataType> &array
    )
    {
        CORE::UInt16
            array_size;
        FileDataType
            element;

        // Read the number of items in the array.
        //
        local_read(stream, array_size);

        array.resize(array_size);

        // Read the elements in the array.
        //
        for (int index = 0; index < array.size(); ++index)
        {
            local_read(stream, element);

            array[index] = static_cast<DataType>(element);
        }
    }

    // ------------------------------------------------------------------------
    // Writes an array to the binary stream.  You can specify the data types
    // stored in the array and the data types used in the stream to store the
    // array.
    //
    template<
        class DataType, // Data type for the elements in the array.
        class FileDataType> // Data type to store the array elements in the
                    // stream as.
    void write_array(
        std::ostream &stream,
        const std::vector<DataType> &array
    )
    {
        // Write the number of items in the array.
        //
        local_write(stream, static_cast<CORE::UInt16>(array.size()));

        // Write the elements in the array.
        //
        for (int index = 0; index < array.size(); ++index)
        {
            // Sun Forte has a problem with parameters that are passed by const
            // reference and are then passed by value to another function.
            //
            local_write(stream, static_cast<FileDataType>(array[index]));
        }
    }

    // ------------------------------------------------------------------------
    // Reads a map from the binary stream.  You can specify the data types used
    // in the map and the data types used in the stream to store the map.
    //
    template<
        class KeyDataType, // Data type for the keys in the map.
        class KeyFileDataType, // Data type to store the map keys in the stream
                               // as.
        class ValueDataType, // Data type for the values in the map.
        class ValueFileDataType> // Data type to store the map values in the
                                 // stream as.
    void read_map(
        std::istream &stream,
        std::map<KeyDataType, ValueDataType> &map
    )
    {
        CORE::UInt16
            map_size;
        KeyFileDataType
            key;
        ValueFileDataType
            value;
        map.clear();

        // Read the number of items in the map.
        //
        local_read(stream, map_size);

        // Read the keys and values in the map.
        //

        for (int index = 0; index < map_size; ++index)
        {
            local_read(stream, key);
            local_read(stream, value);

            std::pair<typename std::map<KeyDataType, ValueDataType>::iterator,
            bool> insert_result =
                    map.insert(typename std::map<KeyDataType,
                        ValueDataType>::value_type(
                        static_cast<KeyDataType>(key),
                        static_cast<ValueDataType>(value)));

            ASSERT(insert_result.second, fatal, "Map insert failed in FARM");
        }

        ASSERT(
            map.size() == map_size,
            fatal,
            "Could not insert all of the items into a map!");
    }

    // ------------------------------------------------------------------------
    // Writes a map to the binary stream.  You can specify the data types used
    // in the map and the data types used in the stream to store the map.
    //
    template<
        class KeyDataType, // Data type for the keys in the map.
        class KeyFileDataType, // Data type to store the map keys in the stream
                               // as.
        class ValueDataType, // Data type for the values in the map.
        class ValueFileDataType> // Data type to store the map values in the
                                 // stream as.
    void write_map(
        std::ostream &stream,
        const std::map<KeyDataType, ValueDataType> &map
    )
    {
        typename std::map<KeyDataType, ValueDataType>::const_iterator
            iterator = map.begin();

        // Write the number of items in the map.
        //
        local_write(
            stream, static_cast<CORE::UInt16>(map.size()));

        // Write the keys and values in the map.
        //
        while (iterator != map.end())
        {
            local_write(stream, static_cast<KeyFileDataType>(iterator->first));
            local_write(
                stream, static_cast<ValueFileDataType>(iterator->second));

            ++iterator;
        }
    }

    // ------------------------------------------------------------------------
    void dump_feature_labels_to_codes()
    {
        int
            count = 1;

        for (FeatureLabelsToCodes::const_iterator
            map_itr = feature_labels_to_codes.begin();
            map_itr != feature_labels_to_codes.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }
    }

    // ------------------------------------------------------------------------
    void dump_feature_codes_to_labels()
    {
        int
            count = 1;

        for (FeatureCodesToLabels::const_iterator
            map_itr = feature_codes_to_labels.begin();
            map_itr != feature_codes_to_labels.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }
    }

    // ------------------------------------------------------------------------
    void dump_attribute_labels_to_codes()
    {
        int
            count = 1;

        for (AttributeLabelsToCodes::const_iterator
            map_itr = attribute_labels_to_codes.begin();
            map_itr != attribute_labels_to_codes.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }
    }

    // ------------------------------------------------------------------------
    void dump_attribute_codes_to_labels()
    {
        int
            count = 1;

        for (AttributeCodesToLabels::const_iterator
            map_itr = attribute_codes_to_labels.begin();
            map_itr != attribute_codes_to_labels.end();
            ++map_itr)
        {
            std::cout << count << ") " <<
            map_itr->first << ", " <<
            map_itr->second << std::endl;
            ++count;
        }
    }

    // ------------------------------------------------------------------------
    void dump_enum_labels_to_codes()
    {
        int
            count = 1;

        for (EnumerantLabelsToCodes::const_iterator
            map_itr = enum_labels_to_codes.begin();
            map_itr != enum_labels_to_codes.end();
            ++map_itr)
        {
            FARM::AttributeEnumPair
                key = map_itr->first;

            std::cout << count << ") [" << key.first << ", ";
            std::cout << key.second << "], ";
            std::cout << map_itr->second << std::endl;

            ++count;
        }
    }

    // ------------------------------------------------------------------------
    void dump_enum_codes_to_labels()
    {
        int
            count = 1;

        for (EnumerantCodesToLabels::const_iterator
            map_itr = enum_codes_to_labels.begin();
            map_itr != enum_codes_to_labels.end();
            ++map_itr)
        {
            AttributeEnumCodePair
                key = map_itr->first;

            std::cout << count << ") [" << key.first << ", ";
            std::cout << key.second << "], ";
            std::cout << map_itr->second << std::endl;

            ++count;
        }
    }
}

namespace FARM
{
    bool
        FeatureAttributeMapping::farm_initialized = false;
    FeatureAttributeMapping::AttributeCodesToAttributes
        FeatureAttributeMapping::attribute_codes_to_attributes;
    std::vector<FeatureAttributeMapping::FarmAttributeCodeToDataType>
        FeatureAttributeMapping::farm;

    // ------------------------------------------------------------------------
    // Initializes the EDCS-related maps; labels-to-codes and codes-to-labels
    // for features, attributes, and enums.
    //
    // Return:  Were the maps created?
    //
    bool FeatureAttributeMapping::initialize_edcs_maps(
        const std::string &edcs_feature_mapping_filename,
        const std::string &edcs_attribute_mapping_filename,
        const std::string &edcs_attrib_enum_mapping_filename
    )
    {
        if (not edcs_maps_initialized)
        {
            FeatureLabel
                new_feature_label;

            CORE::Int32
                new_feature_code;

            // Initialize the EDCS mapping tables.

            feature_labels_to_codes.clear();
            feature_codes_to_labels.clear();
            attribute_labels_to_codes.clear();
            attribute_codes_to_labels.clear();
            enum_labels_to_codes.clear();
            enum_codes_to_labels.clear();

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
                "Could not parse the edcs feature mapping configuration "
                    "file " + edcs_feature_mapping_filename);

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
                new_feature_code = -999;

                ASSERT(edcs_config_options_ptr->value(edcs_feature_table, i, edcs_feature_entry),
                    high,
                    "Unable to get entry from edcs feature mapping");

                // Get the new feature label.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_feature_entry, 1, new_feature_label),
                    high,
                    "Unable to get new feature label from edcs feature mapping entry");

                // Get the new feature code.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_feature_entry, 2, new_feature_code),
                    high,
                    "Unable to get new feature code from edcs feature mapping entry");

                feature_labels_to_codes.insert(
                    FeatureLabelsToCodes::value_type(new_feature_label, new_feature_code));
                feature_codes_to_labels.insert(
                    FeatureCodesToLabels::value_type(new_feature_code, new_feature_label));
            }

            //for debug
            //dump_feature_labels_to_codes();
            //dump_feature_codes_to_labels();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }

            // Read in Attribute Mapping data and set up maps

            AttributeLabel
                new_attribute_label;

            CORE::Int32
                new_attribute_code;

            std::string
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
                new_data_type_string = "";
                new_attribute_label = "";
                new_attribute_code = -999;

                ASSERT(edcs_config_options_ptr->value(edcs_attribute_table, i, edcs_attribute_entry),
                    high,
                    "Unable to get entry from edcs attribute mapping");

                // Get the new attribute code.
                //
                ASSERT(edcs_config_options_ptr->value(edcs_attribute_entry, 3, new_attribute_code),
                    high,
                    "Unable to get new attribute code from edcs attribute mapping entry");

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

//should we ignore attribute mappings for enumeration types here? MMR
                // Store the attribute mappings.
                //
                if (new_data_type_string != "DELETED" and
                    new_data_type_string != "REMOVED" and
                    new_attribute_label != "" and
                    new_attribute_code != 0)
                {
                    attribute_labels_to_codes.insert(
                        AttributeLabelsToCodes::value_type(
                            new_attribute_label, new_attribute_code));

                    attribute_codes_to_labels.insert(
                        AttributeCodesToLabels::value_type(
                            new_attribute_code, new_attribute_label));
                }
            }

            //for debug
            //dump_attribute_labels_to_codes();
            //dump_attribute_codes_to_labels();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }

            // Read in Enumerant Mapping data and set up maps

            CORE::Int32
                new_enumerant_code;

            AttributeEnumCodePair
                attribute_enum_code;

            std::string
                mapping_type,
                new_enumerant_label_string;

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
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_table, i, edcs_enum_entry),
                    high,
                    "Unable to get entry from edcs enum mapping");

                // Get the mapping type.
                //
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 4, mapping_type),
                    high,
                    "Unable to get mapping type from edcs enum mapping entry");

                // Get the new attribute data type.
                //
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 5, new_data_type_string),
                    high,
                    "Unable to get new data type from edcs enum mapping "
                        "entry");

                // Get the new attribute code.
                //
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 6, new_attribute_code),
                    high,
                    "Unable to get new attribute code from edcs enum mapping "
                        "entry");

                // Get the new enumerant code.
                //
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 7, new_enumerant_code),
                    high,
                    "Unable to get new enum code from edcs enum mapping "
                        "entry");

                // Get the new attribute label.
                //
                ASSERT(
                    edcs_config_options_ptr->value(
                        edcs_enum_entry, 8, new_attribute_label),
                    high,
                    "Unable to get new attribute label from edcs enum mapping "
                        "entry");

                // Add the attribute mappings that come out of the enumeration
                // mapping file.
                //
                if (mapping_type != "DELETED" and mapping_type != "REMOVED")
                {
                    attribute_labels_to_codes.insert(AttributeLabelsToCodes::
                        value_type(new_attribute_label, new_attribute_code));

                    attribute_codes_to_labels.insert(AttributeCodesToLabels::
                        value_type(new_attribute_code, new_attribute_label));

                    if (new_data_type_string == "ENUM")
                    {
                        // If the new data is an enumeration, populate these
                        // maps for cross-referencing.
                        //
                        ASSERT(
                            edcs_config_options_ptr->value(
                                edcs_enum_entry,
                                9,
                                new_enumerant_label_string),
                            high,
                            "Unable to get new enum label from edcs enum "
                                "mapping entry");

                        AttributeEnumPair
                            new_enum_label(
                                new_attribute_label,
                                new_enumerant_label_string);

                        enum_labels_to_codes.insert(
                            EnumerantLabelsToCodes::value_type(
                                new_enum_label, new_enumerant_code));

                        attribute_enum_code.first = new_attribute_label;
                        attribute_enum_code.second = new_enumerant_code;

                        enum_codes_to_labels.insert(
                            EnumerantCodesToLabels::value_type(
                                attribute_enum_code,
                                new_enumerant_label_string));
                    }
                }
            }

            //for debug
            //dump_attribute_labels_to_codes();
            //dump_attribute_codes_to_labels();
            //dump_enum_labels_to_codes();
            //dump_enum_codes_to_labels();

            if (edcs_config_options_ptr)
            {
                delete edcs_config_options_ptr;
                edcs_config_options_ptr = 0;
            }

            edcs_maps_initialized =
                feature_labels_to_codes.size() and
                feature_codes_to_labels.size() and
                attribute_labels_to_codes.size() and
                attribute_codes_to_labels.size() and
                enum_labels_to_codes.size() and
                enum_codes_to_labels.size();
        }

        return edcs_maps_initialized;
    }

    // ------------------------------------------------------------------------
    // Reads the data in the FDF configuration file and builds the appropriate
    // data structures.  The FDF file contains the types of features that can
    // be stored in the terrain database.  The label, FACC, geometry,
    // description, and precedence is specified for each feature.
    //
    void FeatureAttributeMapping::read_fdf(const std::string &fdf_file_label)
    {
        FeatureLabel
            label;
        FeatureGeometry
            geometry;
        std::string
            farm_geometry,
            description,
            precedence_str;
        FeaturePrecedence
            precedence;
        FeatureCategory
            feature_category = 0;
        std::ifstream
            fdf_file(fdf_file_label.c_str());

        ASSERT(
            fdf_file.is_open(),
            fatal,
            "Could not open the FDF file '" + fdf_file_label + "'!");

         // initialize to known size - this can grow if needed
         //
         feature_categories_to_features.resize(384);

        // Read the label, FACC, geometry, description, and precedence for the
        // feature types in the terrain database.
        //
        while (get_next_string(fdf_file, label))
        {
            // Read the geometry.
            //
            ASSERT(
                get_next_string(fdf_file, farm_geometry),
                fatal,
                "Could not read the geometry for a feature in the FDF file!");

            if (farm_geometry == farm_point)
            {
                geometry = point;
            }
            else if (farm_geometry == farm_linear)
            {
                geometry = linear;
            }
            else if (farm_geometry == farm_areal)
            {
                geometry = areal;
            }
            else
            {
                LOG(
                    fatal,
                    "Found an unknown feature geometry in the FDF file: " +
                        farm_geometry );
            }

            // Read the description.
            //
            ASSERT(
                get_next_string(fdf_file, description),
                fatal,
                "Could not read the description for a feature in the FDF "
                    "file!");

            // Read the precedence.
            //
            ASSERT(
                get_next_number(fdf_file, precedence_str),
                fatal,
                "Could not read the precedence for a feature in the FDF "
                    "file!");

            // Update the FARM data structures.

            Feature
                feature = Feature(label, geometry, feature_category);

            ASSERT_WITH_STREAM(
                feature.valid(),
                fatal,
                "Could construct a new Feature with label & geometry from FDF "
                    "File.  Label was '" << label << "'.  Geometry was " <<
                    geometry << ".");

            ASSERT(
                CORE::from_string( precedence_str, precedence ),
                fatal,
                "Could not convert a string to a feature precedence!");

            feature.set_precedence( precedence );

            ASSERT_WITH_STREAM(
                feature_labels_and_geometries_to_categories.insert(
                    FeatureLabelsGeometriesToCategories::value_type(
                        FeatureLabelAndGeometry(label, geometry),
                        feature_category)).second,
                fatal,
                "Could not insert a feature type's label, geometry, and "
                    "category in a map.  The feature label was '" << label <<
                    "'.  The feature geometry was " << geometry << ".");

            if (feature_categories_to_features.size() < (feature_category + 1))
            {
                feature_categories_to_features.resize(feature_category+1);
            }

            feature_categories_to_features[feature_category]=feature;

            ++feature_category;
        }

        ASSERT(
            0 < feature_category,
            fatal,
            "The FDF file contained no features!");

        ASSERT(
            fdf_file.eof(),
            fatal,
            "Did not read the entire FDF file!");
    }

    // ------------------------------------------------------------------------
    // Reads the data in the ADF configuration file and builds the appropriate
    // data structures.  The ADF file contains the type of attributes that the
    // features in the terrain database can contain.  The label, EDM data type,
    // units, editability field, and description is specified for each
    // attribute.
    //
    void FeatureAttributeMapping::read_adf(const std::string &adf_file_label)
    {
        AttributeLabel
            attribute_label;
        std::string
            edm_data_type,
            units_field,
            editability_field,
            description;
        AttributeDataType
            data_type = no_data_type;
        AttributeUnits
            units = unitless;
        bool
            editability = false;
        int
            attribute_count = 0;
        std::ifstream
            adf_file(adf_file_label.c_str());

        ASSERT(
            adf_file.is_open(),
            fatal,
            "Could not open the ADF file '" + adf_file_label + "'!");

        // Read the configuration data for all of the attributes in the terrain
        // database.
        //
        while (get_next_string(adf_file, attribute_label))
        {
            Attribute
                attribute(attribute_label);

            // Read the EDM data type.
            //
            ASSERT(
                get_next_string(adf_file, edm_data_type),
                fatal,
                "Could not read the EDM data type for an attribute in the ADF "
                    "file!");

            // Convert the EDM data type to a FARM data type.
            //
            if (edm_data_type == "INTEGER" or
                edm_data_type == "COUNT" or
                edm_data_type == "RANGE" )
            {
                data_type = int32;
            }
            else if (
                edm_data_type == "REAL" or edm_data_type == "REAL_INTERVAL")
            {
                data_type = float64;
            }
            else if (
                edm_data_type == "STRING" or
                edm_data_type == "CONSTRAINED_STRING")
            {
                data_type = string;
            }
            else if (edm_data_type == "ENUMERATION")
            {
                data_type = enumeration;
            }
            else if (edm_data_type == "LOGICAL")
            {
                data_type = boolean;
            }
            else if (edm_data_type == "UUID")
            {
                data_type = uuid;
            }
            else
            {
                LOG(
                    fatal,
                    "Found an unsupported EDM data type in the ADF file:  " +
                        edm_data_type);
            }

            // Set the units for the attribute.
            //
            if (data_type == string  or 
                data_type == boolean or 
                data_type == uuid)
            {
                units = unitless;
            }
            else if (data_type == enumeration)
            {
                units = enumeration_units;
            }
            else
            {
                // Read the units field.
                //
                ASSERT(
                    get_next_string(adf_file, units_field),
                    fatal,
                    "Could not read the units for an attribute in the ADF "
                        "file.");

                // Convert the string to a unit.
                //
                if (units_field == "UNITLESS")
                {
                    units = unitless;
                }
                else if (units_field == "METRE")
                {
                    units = meters;
                }
                else if (units_field == "METRE_PER_SECOND")
                {
                    units = meters_per_second;
                }
                else if (units_field == "SQUARE_METRE")
                {
                    units = square_meters;
                }
                else if (units_field == "DEGREE")
                {
                    units = degrees;
                }
                else if (units_field == "KILOGRAM")
                {
                    units = kilograms;
                }
                else if (units_field == "KILOGRAMS_PER_CUBIC_METRE")
                {
                    units = kilograms_per_cubic_meter;
                }
                else if (units_field == "CELSIUS")
                {
                    units = celsius;
                }
                else if (units_field == "LITRE")
                {
                    units = liters;
                }
                else if (units_field == "LUX")
                {
                    units = lux;
                }
                else if (units_field == "PASCAL")
                {
                    units = pascals;
                }
                else if (units_field == "MILLISECOND")
                {
                    units = milliseconds;
                }
                else
                {
                    LOG(
                        fatal,
                        "Found the unsupported unit '" + units_field + "'.");
                }
            }

            // Read the editability field.
            //
            ASSERT(
                get_next_string(adf_file, editability_field),
                fatal,
                "Could not read the editability field for an attribute in the "
                    "ADF file.");

            if( editability_field == "Editable" )
            {
                editability = true;
            }
            else if( editability_field == "Not Editable" )
            {
                editability = false;
            }
            else
            {
                LOG(
                    fatal,
               "Found a unsupported editability field for an attribute in the "
                        "ADF file: " + editability_field);
            }

            // Read the description.
            //
            ASSERT(
                get_next_string(adf_file, description),
                fatal,
                "Could not read the description for an attribute in the ADF "
                    "file!");

            // Update the FARM data structures.
            //
            attribute.set_data_type(data_type);
            attribute.set_units(units);
            attribute.set_editability(editability);

            if (attribute_codes_to_attributes.size() < attribute.get_code()+1)
            {
                attribute_codes_to_attributes.resize(attribute.get_code()+1);
            }

            ASSERT(
                not attribute_codes_to_attributes[attribute.get_code()].valid(),
                high,
                "The attribute '" + attribute.get_label() + "' is not unique.");

            attribute_codes_to_attributes[attribute.get_code()] = attribute;

            ++attribute_count;
        }

        ASSERT(
            0 < attribute_count,
            fatal,
            "The ADF file contained no attributes!");

        ASSERT(
            adf_file.eof(),
            fatal,
            "Did not read the entire ADF file!");
    }

    // ------------------------------------------------------------------------
    // If the token is an attribute code, then read the ranges for the
    // attribute.
    //
    // Return:  Was the token an attribute code?
    //
    bool FeatureAttributeMapping::read_attribute_ranges(
        std::ifstream &faa_file,
        std::string &attribute_label,
        std::string &new_geometry,
        const FeatureCategory &feature_category
    )
    {
        bool
            status;
        std::string
            string;
        Attribute
            attribute;
        CORE::Int32
            def_int32,
            min_int32,
            max_int32;
        CORE::Float64
            def_float64,
            min_float64,
            max_float64;
        Enumerants
            enumerants;

        status = attribute.set_label(attribute_label);

        if ( status )
        {
            if ( attribute_codes_to_attributes[attribute.get_code()].valid() )
            {
                attribute = attribute_codes_to_attributes[attribute.get_code()];
                status = true;
            }
            else
            {
                status = false;
            }
        }

        if ( status )
        {
            attribute_label = "";
            new_geometry = "";

            // The token is an attribute label.  Read the ranges for the
            // attribute.
            //
            switch (attribute.get_data_type())
            {
                case int32:
                {
                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the default for an Int32!");

                    ASSERT(
                        is_integer(string),
                        fatal,
                        "The default for an attribute was not an integer!");

                    ASSERT(
                        CORE::from_string(string, def_int32),
                        fatal,
                        "Could not convert a string to an Int32 default!");

                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the minimum for an Int32!");

                    ASSERT(
                        is_integer(string),
                        fatal,
                        "The minimum for an attribute was not an integer!");

                    ASSERT(
                        CORE::from_string(string, min_int32),
                        fatal,
                        "Could not convert a string to an Int32 minimum!");

                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the maximum for an Int32!");

                    ASSERT(
                        is_integer(string),
                        fatal,
                        "The maximum for an attribute was not an integer!");

                    ASSERT(
                        CORE::from_string(string, max_int32),
                        fatal,
                        "Could not convert a string to an Int32 maximum: " +
                        string );

                    // Associate the attribute with the feature since it is
                    // not in the DAF file!.
                    //
                    farm[feature_category][attribute.get_code()] =
                            new InstantiatedDataType<CORE::Int32>(
                                def_int32, min_int32, max_int32);

                    break;
                }

                case float64:
                {
                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the default for a Float64!");

                    ASSERT(
                        CORE::from_string(string, def_float64),
                        fatal,
                        "Could not convert a string to a Float64 default!");

                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the minimum for a Float64!");

                    ASSERT(
                        CORE::from_string(string, min_float64),
                        fatal,
                        "Could not convert a string to a Float64 minimum!");

                    ASSERT(
                        get_next_number(faa_file, string),
                        fatal,
                        "Could not get the maximum for a Float64!");

                    ASSERT(
                        CORE::from_string(string, max_float64),
                        fatal,
                        "Could not convert a string to a Float64 maximum!");

                    // Associate the attribute with the feature since it is
                    // not in the DAF file!.
                    //
                    farm[feature_category][attribute.get_code()] =
                            new InstantiatedDataType<CORE::Float64>(
                                def_float64, min_float64, max_float64);

                    break;
                }

                case boolean:
                {
                    bool
                        value = false;

                    ASSERT(
                        get_next_string(faa_file, string),
                            fatal,
                            "Could not get the default for an Boolean!");

                    if ( string == "TRUE" )
                    {
                        value = true;
                    }
                    else if ( string == "FALSE" )
                    {
                        value = false;
                    }
                    else
                    {
                        LOG(
                            fatal,
                            "Found an Boolean Attribute with an "
                                "invalid default!");
                    }

                    farm[feature_category][attribute.get_code()] =
                            new BooleanDataType(value);

                    break;
                }

                case FARM::string:
                {
                    // Associate the attribute with the feature since it is
                    // not in the DAF file!.
                    //
                    farm[feature_category][attribute.get_code()] =
                            new StringDataType();

                    break;
                }

                case enumeration:
                {
                    std::string
                        default_value;
                    Enumerant
                        enumerant,
                        default_enum;
                    bool
                        valid_enum = true,
                        new_feature = false;

                    enumerants.clear();

                    ASSERT(
                        get_next_string( faa_file, default_value ),
                        fatal,
                        "Could not get the default for an Enumeration!");

                    ASSERT(
                        default_enum.set_labels(
                            attribute.get_label(),
                            default_value ),
                        fatal,
                        "Attribute and Default Enumerant Label are not valid!"
                        "  ATTRIBUTE: " + attribute.get_label() +
                               "  Default Value: " + default_value);

                    ASSERT(
                        get_next_string(faa_file, string),
                        fatal,
                        "Could not get the next possible enumerations!");

                    valid_enum = enumerant.set_labels(
                        attribute.get_label(),
                        string );

                    while (valid_enum and not new_feature
                        and not faa_file.eof())
                    {
                        ASSERT(
                            enumerants.insert(enumerant).second,
                            fatal,
                            "Could not add an Enumerant to a set!" );

                        if (attribute_label == "")
                        {
                            valid_enum = get_next_string(faa_file, string);
                        }
                        else
                        {
                            string = attribute_label;
                            attribute_label = "";
                        }

                        // Tundra and dragon teeth can also be features

                        if (valid_enum
                            and
                            ( string == "TUNDRA"
                              or
                              string == "DRAGON_TEETH" ) )
                        {
                            std::string
                                temp;

                            ASSERT(
                                get_next_string(faa_file, temp),
                                fatal,
                                "Could not get the next string "
                                    "after new_feature!");

                            if ( temp == "AREA" or temp == "POINT" )
                            {
                                new_feature = true;
                                new_geometry = temp;
                            }
                            else
                            {
                                attribute_label = temp;
                            }
                        }

                        if( valid_enum and not new_feature )
                        {
                            valid_enum = enumerant.set_labels(
                                attribute.get_label(),
                                string );
                        }
                    }

                    if (not faa_file.eof())
                    {
                        attribute_label = string;
                    }

                    // Associate the attribute with the feature since it is
                    // not in the DAF file!.
                    //
                    farm[feature_category][attribute.get_code()] =
                            new EnumerantDataType(default_enum, enumerants);

                    break;
                }

                case uuid:
                {
                    // The UUID has no default, min, or max values.
                    //
                    farm[feature_category][attribute.get_code()] =
                            new UUIDDataType();
                    break;
                }

                default:
                {
                    LOG(
                        fatal,
                        "Found an invalid data type: " +
                            CORE::to_string(attribute.get_data_type()) + ".");

                    break;
                }
            };

            status =
                static_cast<bool>(farm[feature_category][attribute.get_code()]);

            ASSERT(
                status,
                fatal,
                "Could not allocate memory for an entry in the FARM!");
        }

        return status;
    }

    // ------------------------------------------------------------------------
    // Calculates the offsets and overlay size for the attributes in the
    // feature.
    //
    void FeatureAttributeMapping::calculate_offsets_overlay_size(
        const FeatureCategory &feature_category
    )
    {
        int
            overlay_size = 0;

        // Set the offsets for the attributes that are eight bytes.
        //
        for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
            if (farm[feature_category][attr] and
                (dynamic_cast<InstantiatedDataType<CORE::Float64> *>(
                    farm[feature_category][attr]) or
                dynamic_cast<StringDataType *>(
                    farm[feature_category][attr]) or
                dynamic_cast<EnumerantDataType *>(
                    farm[feature_category][attr]) ) )
            {
                farm[feature_category][attr]->
                    set_offset(overlay_size);

                overlay_size += 8;
            }
        }

        // Set the offsets for the attributes that are four bytes.
        //
        for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
            if (farm[feature_category][attr] and
                not dynamic_cast<InstantiatedDataType<CORE::Float64> *>(
                    farm[feature_category][attr]) and
                not dynamic_cast<StringDataType *>(
                    farm[feature_category][attr]) and
                not dynamic_cast<EnumerantDataType *>(
                    farm[feature_category][attr]) )
            {
                farm[feature_category][attr]->
                    set_offset(overlay_size);

                overlay_size += 4;
            }
        }

        // Set the offsets for the attributes that are sixteen bytes.
        //
       for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
            if (farm[feature_category][attr] and
                (dynamic_cast<UUIDDataType *>(
                    farm[feature_category][attr])))
            {
                farm[feature_category][attr]->
                    set_offset(overlay_size);

                overlay_size += 16;
            }
        }

        feature_categories_to_features[feature_category].
            set_attributes_overlay_size(overlay_size);
    }

    // ------------------------------------------------------------------------
    // Reads the data in the FAA configuration file and builds the appropriate
    // data structures.  The FAA files contains which attributes the different
    // feature types contain and also what the different feature types are used
    // for.
    //
    void FeatureAttributeMapping::read_faa(const std::string &faa_file_label)
    {
        FeatureLabel
            feature_label;
        std::string
            token;
        FeatureGeometry
            feature_geometry = null;
        FeatureLabelsGeometriesToCategories::const_iterator
            iterator;
        bool
            found_next_feature;
        std::ifstream
            faa_file(faa_file_label.c_str());

        ASSERT(
            faa_file.is_open(),
            fatal,
            "Could not open the FAA file '" + faa_file_label + "'!");

        // Initialize the FARM to the correct size and initialize all of the
        // pointers in it to null.
        //
        farm.resize(feature_categories_to_features.size());

        for (int row = 0; row < farm.size(); ++ row)
        {
            farm[row].resize(attribute_codes_to_attributes.size());
            for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
            {
                farm[row][attr]=0;
            }

        }

        // Read the feature label, FACC, geometry, usages, attributes, and
        // attribute ranges for all of the possible features in the terrain
        // database.
        //
        ASSERT(
            get_next_string(faa_file, feature_label),
            fatal,
            "Could not get the label for a feature in the FAA file!");

        std::string
            next_geometry = "";

        do
        {
            // Read the geometry.
            //
            if (next_geometry == "")
            {
                bool success = get_next_string(faa_file, token);
                if (not success)
                {
                    std::cout << "Failed to get geometry for " <<
                        feature_label << std::endl;
                }

                ASSERT(
                    success,
                    fatal,
                    "Could not get the geometry for a feature in the FAA file!");
            }
            else
            {
                token = next_geometry;
            }

            // Convert the string to a geometry.
            //
            if (token == farm_point)
            {
                feature_geometry = point;
            }
            else if (token == farm_linear)
            {
                feature_geometry = linear;
            }
            else if (token == farm_areal)
            {
                feature_geometry = areal;
            }
            else
            {
                LOG_WITH_STREAM(
                    fatal,
                    "Found an unknown feature geometry in the FAA file for label:" <<
                    feature_label << ".  The geometry was '" << token << "'.");
            }

            // Calculate the feature category.
            //
            iterator = feature_labels_and_geometries_to_categories.find(
                FeatureLabelAndGeometry(feature_label, feature_geometry));

            ASSERT(
                iterator != feature_labels_and_geometries_to_categories.end(),
                fatal,
                "Found an invalid feature label in the FAA file: EC_Label: " +
                    feature_label + "; Geometry: " + token);

            // Read the usages and attributes in the feature.

            found_next_feature = false;

            bool
                got_next_string = get_next_string(faa_file, token);
            std::string
                next_token = token;

        #if EDM_DEBUG
            std::cout <<
                "Parsing attributes for: " << feature_geometry << ":" <<
                feature_label << "\n" << std::flush;
        #endif

            while (not found_next_feature and got_next_string)
            {
                  if (not updated_usage_bitmask(token,
                      feature_categories_to_features[iterator->second]))
                {
                    next_token = token;

                #if EDM_DEBUG
                    std::cout <<
                        "\tnext_token: " << next_token << "\n" << std::flush;
                #endif

                    found_next_feature = not (
                        read_attribute_ranges(
                            faa_file,
                            next_token,
                            next_geometry,
                            iterator->second));

                    if (next_geometry != "")
                    {
                        found_next_feature = true;
                    }

                    if (found_next_feature)
                    {
                        // Calculate the offsets and overlay size for the
                        // attributes in the feature.
                        //
                        calculate_offsets_overlay_size(iterator->second);

                        // The token was a feature label.
                        //
                        feature_label = next_token;
                    }
                    else if (not faa_file.eof() and next_token == "")
                    {
                        got_next_string = get_next_string(faa_file, next_token);
                    }
                    else
                    {
                        got_next_string = true;
                    }
                    token = next_token;
                }
                else
                {
                    ASSERT(
                        get_next_string(faa_file, token),
                        fatal,
                        "Failed to get next token!!");
                }
            }
        }
        while (found_next_feature);

        // Calculate the offsets and overlay size for the attributes in the
        // last feature in the FAA file.
        //
        calculate_offsets_overlay_size(iterator->second);

        ASSERT(
            faa_file.eof(),
            fatal,
            "Did not read the entire FAA file!");

        // Ensure that all of the features contains at least one attribute and
        // usage.
        //
        for (int i = 0;
            i < feature_categories_to_features.size();
            ++i)
        {
            if (feature_categories_to_features[i].valid()
                and
                0 >= feature_categories_to_features[i].get_attributes_overlay_size())
            {
                std::cout << "Feature does not contain an attribute:  " <<
                    feature_categories_to_features[i] << std::endl;

                ASSERT(
                0 < feature_categories_to_features[i].get_attributes_overlay_size(),
                    info,
                    "A feature did not contain at least one attribute!");

                if ( ! feature_categories_to_features[i].get_usage_bitmask() )
                {
                    std::cout << "Feature does not contain a usage:  " <<
                        feature_categories_to_features[i] << std::endl;
                }

                ASSERT(
                    feature_categories_to_features[i].get_usage_bitmask(),
                    info,
                    "A feature did not contain at least one usage!");
            }
        }
    }

    // ------------------------------------------------------------------------
    // Reads the FARM table (2-dimensional array that maps features to
    // attributes) from a binary stream.
    //
    void FeatureAttributeMapping::read_farm_table(std::istream &stream)
    {
        CORE::UInt16
            num_rows,
            num_columns,
            code,
            code_max=0,
            data_type;
        std::vector<AttributeCode>
            codes;

        // Read the dimensions of the FARM table.
        //
        local_read(stream, num_rows);
        local_read(stream, num_columns);

        for( int column = 0; column < num_columns; ++column )
        {
            local_read(stream, code);

            if (code > code_max)
              code_max=code;

            codes.push_back(static_cast<AttributeCode>(code));
        }

        farm.resize(num_rows);

        // Read the entries in the FARM table.
        //
        for (int row = 0; row < farm.size();  ++row)
        {
            // resize to the possible number of attributes for each category
            //
            farm[row].resize(code_max+1);

            for (int column = 0; column < num_columns; ++column)
            {
                // Read the data type for the table entry.  Allocate memory for
                // and read the table entry if need be.
                //
                local_read(stream, data_type);

                switch (data_type)
                {
                    case no_data_type:
                    {
                        // The feature does not contain the attribute.
                        //
                        farm[row][codes[column]]=0;
                        break;
                    }

                    case int32:
                    {
                        farm[row][codes[column]]=
                             new InstantiatedDataType<CORE::Int32>();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    case float64:
                    {
                        farm[row][codes[column]]=
                             new InstantiatedDataType<CORE::Float64>();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    case string:
                    {
                        farm[row][codes[column]]= new StringDataType();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    case enumeration:
                    {
                        farm[row][codes[column]]= new EnumerantDataType();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    case boolean:
                    {
                        farm[row][codes[column]]= new BooleanDataType();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    case uuid:
                    {
                        farm[row][codes[column]]= new UUIDDataType();

                        ASSERT(
                            farm[row][codes[column]],
                            fatal,
                            "Could not allocate memory for an entry in the "
                                "FARM table!");
                        break;
                    }

                    default:
                    {
                        LOG(
                            fatal,
                            "Found an unsupported data type in the FARM "
                                "binary file!  DataType:  " +
                                CORE::to_string(data_type) );

                        break;
                    }
                };

                if (farm[row][codes[column]])
                {
                    // The feature contains the attribute.
                    //
                    farm[row][codes[column]]->read(stream);
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // Writes the FARM table (2-dimensional array that maps features to
    // attributes) to a binary stream.
    //
    void FeatureAttributeMapping::write_farm_table(std::ostream &stream)
    {
        ASSERT(
            farm.size(),
            fatal,
            "There are no features in the FARM!");

        ASSERT(
            farm[0].size(),
            fatal,
            "There are no attributes in the FARM!");

        // Write the dimensions of the FARM table.
        //
        local_write(
            stream, static_cast<CORE::UInt16>(farm.size()));
        int
            count=0;
        for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
             if(attribute_codes_to_attributes[attr].valid())
                 count++;
        }
        local_write(
            stream,
            static_cast<CORE::UInt16>(count));

        for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
            if(attribute_codes_to_attributes[attr].valid())
            local_write(stream, static_cast<AttributeCode>(attr));
        }

        // Write the entries in the FARM table.
        //
        for (int row = 0; row < farm.size();  ++row)
        {
            ASSERT(
                farm[0].size() == farm[row].size(),
                fatal,
                "Two rows in the FARM were not the same size!\n"
                "farm[0] = " + CORE::to_string(farm[0].size()) +
                "; farm[" + CORE::to_string(row) + "] = " +
                CORE::to_string(farm[row].size()));

           for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
            {
                int
                    column = attr;

                // Write the table entry.
                //
                if (attribute_codes_to_attributes[attr].valid())
                    if(not farm[row][column])
                {
                    // The feature does not contain the attribute.
                    //
                    local_write(
                        stream, static_cast<CORE::UInt16>(no_data_type));
                }
                else
                {
                    // The feature contains the attribute.  Write the correct
                    // data type for the attribute.
                    //
                    if (dynamic_cast<InstantiatedDataType<CORE::Int32> *>(
                        farm[row][column]))
                    {
                        local_write(stream, static_cast<CORE::UInt16>(int32));
                    }
                    else if (dynamic_cast<InstantiatedDataType<
                        CORE::Float64> *>(farm[row][column]))
                    {
                        local_write(
                            stream, static_cast<CORE::UInt16>(float64));
                    }
                    else if (dynamic_cast<StringDataType *>(
                        farm[row][column]))
                    {
                        local_write(stream, static_cast<CORE::UInt16>(string));
                    }
                    else if (dynamic_cast<EnumerantDataType *>(
                        farm[row][column]))
                    {
                        local_write(
                            stream, static_cast<CORE::UInt16>(enumeration));
                    }
                    else if(dynamic_cast<BooleanDataType *>(farm[row][column]))
                    {
                        local_write(
                            stream, static_cast<CORE::UInt16>(boolean));
                    }
                    else if (dynamic_cast<UUIDDataType *>(
                        farm[row][column]))
                    {
                        local_write(stream, static_cast<CORE::UInt16>(uuid));
                    }
                    else
                    {
                        LOG(
                            fatal,
                            "Found an unsupported data type for an entry in "
                                "the FARM!");
                    }

                    farm[row][column]->write(stream);
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // Writes the feature labels and geometries to feature categories map to
    // the binary stream in a format that the terrain compiler can read.
    //
    void FeatureAttributeMapping::dump_feature_labels_geometries_to_categories(
        std::ostream &stream
    )
    {
        FeatureLabelsGeometriesToCategories::const_iterator
            iter = feature_labels_and_geometries_to_categories.begin();

        // Write the number of items in the map.
        //
        CORE::Int32
            int32 = feature_labels_and_geometries_to_categories.size();

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        // Write the items in the map.
        //
        while (iter != feature_labels_and_geometries_to_categories.end())
        {
            CORE::dump_string(stream, iter->first.first);
            int32 = iter->first.second;
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
            int32 = iter->second;
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

            ++iter;
        }
    }

    // ------------------------------------------------------------------------
    // Reads the feature labels and geometries to feature categories map from
    // the binary stream.
    //
    void FeatureAttributeMapping::load_feature_labels_geometries_to_categories(
        std::istream &stream
    )
    {
        CORE::Int32
            num_features,
            int32;
        FeatureLabelAndGeometry
            label_and_geometry;
        FeatureCategory
            category;

        feature_labels_and_geometries_to_categories.clear();

        // Read the number of items in the map.
        //
        stream.read(
            reinterpret_cast<char *>(&num_features), sizeof(num_features));

        // Read the items in the map.
        //
        for (int index = 0; index < num_features; ++index)
        {
            // Read the feature label.
            //
            CORE::load_string(stream, label_and_geometry.first);

            // Read the geometry.
            //
            stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
            label_and_geometry.second = static_cast<FeatureGeometry>(int32);

            // Read the category.
            //
            stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
            category = int32;

            ASSERT(
                feature_labels_and_geometries_to_categories.insert(
                    FeatureLabelsGeometriesToCategories::value_type(
                        label_and_geometry, category)).second,
                fatal,
                "Could not insert a feature label and geometry.");
        }
    }

    // ------------------------------------------------------------------------
    // Writes the feature categories to features map to the binary stream in a
    // format that the terrain compiler can read.
    //
    void FeatureAttributeMapping::dump_feature_categories_to_features(
        std::ostream &stream
    )
    {
        // Write the number of items in the map.
        //
        CORE::Int32
            int32 = feature_categories_to_features.size();

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        // Write the items in the map.
        //
        for (int i=0; i < feature_categories_to_features.size(); i++)
            if (feature_categories_to_features[i].valid())
            {
                int32 = i;
                stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
                feature_categories_to_features[i].dump(stream);
            }
    }

    // ------------------------------------------------------------------------
    // Reads the feature categories to features map from the binary stream.
    //
    void FeatureAttributeMapping::load_feature_categories_to_features(
        std::istream &stream
    )
    {
        CORE::Int32
            num_features,
            category;
        Feature
            feature;

        feature_categories_to_features.clear();

        // Read the number of items in the map.
        //
        stream.read(
            reinterpret_cast<char *>(&num_features), sizeof(num_features));

        feature_categories_to_features.resize(num_features);

        // Read the items in the map.
        //
        for (int index = 0; index < num_features; ++index)
        {
            // Read the category.
            //
            stream.read(reinterpret_cast<char *>(&category), sizeof(category));

            // Read the FARM feature.
            //
            feature.load(stream);

            if(feature_categories_to_features.size() < category+1)
                feature_categories_to_features.resize(category+1);

            feature_categories_to_features[category]=feature;
            ASSERT(
                feature_categories_to_features[category].valid(),
                fatal,
                "Could not add a feature category.");
        }
    }

    // ------------------------------------------------------------------------
    // Writes the attribute codes to attributes map to the binary stream in a
    // format that the terrain compiler can read.
    //
    void FeatureAttributeMapping::dump_attribute_codes_to_attributes(
        std::ostream &stream
    )
    {

        // Write the number of items in the map.
        //
        CORE::Int32
            int32 = attribute_codes_to_attributes.size();

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        // Write the items in the map.
        //
       for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
        {
            int32 = attr;
            if(attribute_codes_to_attributes[attr].valid())
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
        }
    }

    // ------------------------------------------------------------------------
    // Reads the attribute codes to attributes map from the binary stream.
    //
    void FeatureAttributeMapping::load_attribute_codes_to_attributes(
        std::istream &stream
    )
    {
        CORE::Int32
            num_attributes,
            attribute_code;
        Attribute
            attribute;

        attribute_codes_to_attributes.clear();

        // Read the number of items in the map.
        //
        stream.read(
            reinterpret_cast<char *>(&num_attributes), sizeof(num_attributes));

       attribute_codes_to_attributes.resize(num_attributes+1);

        // Read the items in the map.
        //
        for (int index = 0; index < num_attributes; ++index)
        {
            // Read the attribute code.
            //
            stream.read(
                reinterpret_cast<char *>(&attribute_code),
                sizeof(attribute_code));

            // Read the FARM attribute.
            //
            attribute.load(stream);

            if(attribute_codes_to_attributes.size()< attribute_code+1)
                attribute_codes_to_attributes.resize(attribute_code+1);

           attribute_codes_to_attributes[attribute_code]=attribute;
        }
    }

    // ------------------------------------------------------------------------
    // Writes the attribute codes to enums map to the binary stream in a format
    // that the terrain compiler can read.
    //
    void FeatureAttributeMapping::dump_attribute_codes_to_enums(
        std::ostream &stream
    )
    {
        AttributeCodesToEnums::const_iterator
            attr_iter = attribute_codes_to_enums.begin();
        AttributeEnums::const_iterator
            enum_iter;

        // Write the number of items in the map.
        //
        CORE::Int32
            int32 = attribute_codes_to_enums.size();

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        // Write the items in the map.
        //
        while (attr_iter != attribute_codes_to_enums.end())
        {
            // Write the attribute code.
            //
            int32 = attr_iter->first;
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

            // Write the number of enumerations for the attribute.
            //
            int32 = attr_iter->second.size();
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

            // Write the enumerations for the attribute.
            //
            enum_iter = attr_iter->second.begin();

            while (enum_iter != attr_iter->second.end())
            {
                int32 = enum_iter->first;
                stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

                enum_iter->second.dump(stream);

                ++enum_iter;
            }

            ++attr_iter;
        }
    }

    // ------------------------------------------------------------------------
    // Reads the attribute codes to enums map from the binary stream.
    //
    void FeatureAttributeMapping::load_attribute_codes_to_enums(
        std::istream &stream
    )
    {
        CORE::Int32
            num_attributes,
            attr_code,
            num_enums,
            enum_code;
        std::pair<AttributeCodesToEnums::iterator, bool>
            insert_result;
        Enumerant
            enum_value;

        attribute_codes_to_enums.clear();

        // Read the number of items in the map.
        //
        stream.read(
            reinterpret_cast<char *>(&num_attributes), sizeof(num_attributes));
        // Read the items in the map.
        //
        for (int attr_index = 0; attr_index < num_attributes; ++attr_index)
        {
            // Read the attribute code.
            //
            stream.read(
                reinterpret_cast<char *>(&attr_code), sizeof(attr_code));

            // Insert the attribute code in the map.
            //
            insert_result = attribute_codes_to_enums.insert(
                AttributeCodesToEnums::value_type(
                    attr_code, AttributeEnums()));

            ASSERT(
                insert_result.second,
                fatal,
                "Could not add an attribute code.");

            // Read the number of enumerations for the attribute.
            //
            stream.read(
                reinterpret_cast<char *>(&num_enums), sizeof(num_enums));

            // Read the enumerations for the attribute.
            //
            for (int enum_index = 0; enum_index < num_enums; ++enum_index)
            {
                // Read the enumeration code.
                //
                stream.read(
                    reinterpret_cast<char *>(&enum_code), sizeof(enum_code));

                // Read the enumeration.
                //
                enum_value.load(stream);

                ASSERT(
                    insert_result.first->second.insert(AttributeEnums::
                        value_type(enum_code, enum_value)).second,
                    fatal,
                    "Could not add an enumeration.");
            }
        }
    }

    // ------------------------------------------------------------------------
    // Writes the FARM table to the binary stream in a format that the terrain
    // compiler can read.
    //
    void FeatureAttributeMapping::dump_farm_table(std::ostream &stream)
    {
        FarmAttributeCodeToDataType::const_iterator
            iter;

        // Write the number of feature categories.
        //
        CORE::Int32
            int32 = farm.size(),
            is_present=0;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        for (int index = 0; index < farm.size(); ++index)
        {
            // Write the number of attributes for the feature.
            //
            int32 = farm[index].size();
            stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

            // Write the attributes for the feature.
            //
            iter = farm[index].begin();
            int32=0;
            while (iter != farm[index].end())
            {
                // Write the attribute code.
                //
                stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

                // Write whether the feature contains the attribute.
                //
                is_present = *iter ? 1 : 0;
                stream.write(reinterpret_cast<char *>(&is_present),
                sizeof(is_present));

                if (*iter)
                {
                    // Write the data for the attribute.
                    //
                    (*iter)->dump(stream);
                }
                ++int32;
                ++iter;
            }
        }
    }

    // ------------------------------------------------------------------------
    // Reads the FARM table from the binary stream.
    //
    void FeatureAttributeMapping::load_farm_table(std::istream &stream)
    {
        CORE::Int32
            num_features,
            num_attributes,
            attr_code,
            contains_attr;
        AttributeCodesToAttributes::const_iterator
            iter;
        DataType
            *data_type = NULL;

        // Read the number of feature categories.
        //
        stream.read(
            reinterpret_cast<char *>(&num_features), sizeof(num_features));
        farm.resize(num_features);

        for (int feat_index = 0; feat_index < farm.size(); ++feat_index)
        {
            // Read the number of attributes for the feature.
            //
            stream.read(
                reinterpret_cast<char *>(&num_attributes),
                sizeof(num_attributes));

            farm[feat_index].clear();

            // Read the attributes for the feature.
            //
            for (int attr_index = 0; attr_index < num_attributes; ++attr_index)
            {
                // Read the attribute code.
                //
                stream.read(
                    reinterpret_cast<char *>(&attr_code), sizeof(attr_code));

                // Read whether the feature contains the attribute.
                //
                stream.read(
                    reinterpret_cast<char *>(&contains_attr),
                    sizeof(contains_attr));

                if (not contains_attr)
                {
                    // The feature does not have the attribute.
                    //
                    data_type = 0;
                }
                else
                {
                    // The feature contains the attribute.  Determine the data
                    // type for the attribute.
                    //
                    switch (attribute_codes_to_attributes[attr_code].
                        get_data_type())
                    {
                        case int32:
                        {
                            data_type =
                                new InstantiatedDataType<CORE::Int32>();
                            break;
                        }

                        case float64:
                        {
                            data_type =
                                new InstantiatedDataType<CORE::Float64>();
                            break;
                        }

                        case string:
                        {
                            data_type = new StringDataType();
                            break;
                        }

                        case enumeration:
                        {
                            data_type = new EnumerantDataType();
                            break;
                        }

                        case boolean:
                        {
                            data_type = new BooleanDataType();
                            break;
                        }

                        case uuid:
                        {
                            data_type = new UUIDDataType();
                            break;
                        }

                        default:
                        {
                            LOG(fatal, "Found an unsupported data type.");
                            break;
                        }
                    };

                    ASSERT(
                        data_type,
                        fatal,
                        "Could not allocate memory for a data type.");

                    // Read the data for the attribute.
                    //
                    data_type->load(stream);
                }

                // Add the attribute and data type to the FARM table.
                //
                ASSERT(
                    farm[feat_index][attr_code]=data_type,
                    fatal,
                    "Could not add an attribute for a feature.");
            }
        }
    }

    // ------------------------------------------------------------------------
    // Initializes the FARM using a binary file.
    //
    void FeatureAttributeMapping::initialize_farm_from_binary_file(
        const std::string &database_directory
    )
    {
        // Open the binary file to read.
        //
        std::string
            file_name = database_directory + "/" + farm_binary_input_file;
        std::ifstream
            file(file_name.c_str(), std::ios::in | std::ios::binary);

        ASSERT(
            file.is_open(),
            fatal,
            "Could not open the file '" + file_name + "'.");

        if (file.is_open())
        {
            // Read the tables and maps for the FARM from the file.
            //
            load_feature_labels_geometries_to_categories(file);
            load_feature_categories_to_features(file);
            load_attribute_codes_to_attributes(file);
            load_attribute_codes_to_enums(file);
            load_farm_table(file);
        }
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::initialize(
        const std::string &database_directory,
        const std::string &fdf_file_label,
        const std::string &adf_file_label,
        const std::string &faa_file_label,
        const std::string &edcs_3p1_4p3_feature_mapping,
        const std::string &edcs_3p1_4p3_attribute_mapping,
        const std::string &edcs_3p1_4p3_enum_mapping
    )
    {
        bool
            edcs_initialized = false;

        if(initialized())
        {
            edcs_initialized = true;
        }
        else
        {
            // The FARM has not been initialized.  Initialize it.
            //
            if (fdf_file_label != "" and
                adf_file_label != "" and
                faa_file_label != "")
            {
                // Initialize the software to convert EDCS 3.1 to EDCS 4.3.
                //
                ASSERT(
                    EDCSConverter::initialize(
                        edcs_3p1_4p3_feature_mapping,
                        edcs_3p1_4p3_attribute_mapping,
                        edcs_3p1_4p3_enum_mapping),
                    fatal,
                    "Could not initialize the EDCS 3.1 to EDCS 4.3 converter.");

                edcs_initialized = EDCSConverter::initialized();

                ASSERT(
                    edcs_initialized,
                    fatal,
                    "Could not initialize the EDCS data.");

                // Initialize the FARM using the configuration files.
                //
                read_fdf(fdf_file_label);
                read_adf(adf_file_label);
                read_faa(faa_file_label);
            }
            else
            {
                // Initialize the EDCS-related maps
                //
                edcs_initialized = initialize_edcs_maps(
                    edcs_3p1_4p3_feature_mapping,
                    edcs_3p1_4p3_attribute_mapping,
                    edcs_3p1_4p3_enum_mapping);

                ASSERT(
                    edcs_initialized,
                    fatal,
                    "Could not initialize the EDCS data.");

                // Initialize the FARM using the binary file.
                //
                initialize_farm_from_binary_file(database_directory);
            }

            // originally assigned to true but changed to edcs until the
            // edcs transition is over
            //
            farm_initialized = edcs_initialized;

            // Initialize the feature categories that are commonly used.
            //
            ASSERT(
                FeatureCategories::initialize(),
                fatal,
                "Could not initialize the feature categories.");

            // Initialize the attribute categories that are commonly used.
            //
            ASSERT(
                AttributeCategories::initialize(),
                fatal,
                "Could not initialize the attribute categories.");

            // Initialize the enumerations that are commonly used.
            //
            ASSERT(
                EnumValues::initialize(),
                fatal,
                "Could not initialize the commonly used enumeration values.");

            attribute_labels_to_attributes.clear();
        }

        ASSERT(initialized(),fatal,"Farm did not initialize");
        ASSERT(edcs_initialized,fatal,"EDCS did not initialize");
        return initialized() and edcs_initialized;
    }

    // ------------------------------------------------------------------------
    // Return:  Is the feature category valid and is not the feature category
    // used to represent all of the feature types.
    //
    bool FeatureAttributeMapping::valid_not_all_feature_category(
        const FeatureCategory &feature_category
    )
    {
        verify_farm_initialization();

        return CORE::ordered<int>(
            0,
            feature_category,
            feature_categories_to_features.size() - 1);
    }

    // ------------------------------------------------------------------------
    void FeatureAttributeMapping::destroy(void)
    {
        if (initialized())
        {
            // Delete the memory for the two-dimensional array.
            //
            for (int row = 0; row < farm.size(); ++row)
            {
                for (int col = 0; col < farm[row].size(); ++col)
                {
                    if (farm[row][col])
                    {
                        delete farm[row][col];
                    }
                }

                farm[row].clear();
            }

            farm.clear();

            feature_labels_and_geometries_to_categories.clear();
            feature_categories_to_features.clear();
            attribute_codes_to_attributes.clear();
            attribute_codes_to_enums.clear();
            attribute_labels_to_attributes.clear();

            feature_labels_to_codes.clear();
            feature_codes_to_labels.clear();
            attribute_labels_to_codes.clear();
            attribute_codes_to_labels.clear();
            enum_labels_to_codes.clear();
            enum_codes_to_labels.clear();

            farm_initialized = false;
            edcs_maps_initialized = false;
        }
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_features(std::list<Feature> &features)
    {
        verify_farm_initialization();

        features.clear();

        for(int i=0; i < feature_categories_to_features.size(); i++)
            if(feature_categories_to_features[i].valid())
                features.push_back(feature_categories_to_features[i]);

        return features.size() > 0;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_label_and_geometries(
        std::list<FeatureLabelAndGeometry> &feature_label_and_geometries
    )
    {
        verify_farm_initialization();

        feature_label_and_geometries.clear();

         for(int i=0; i < feature_categories_to_features.size(); i++)
            if(feature_categories_to_features[i].valid())
                feature_label_and_geometries.push_back(
                        FeatureLabelAndGeometry(
                        feature_categories_to_features[i].get_label(),
                        feature_categories_to_features[i].get_geometry() ) );

        return feature_label_and_geometries.size() > 0;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_categories(
        std::list<FeatureCategory> &feature_categories
    )
    {
        verify_farm_initialization();

        feature_categories.clear();

         for(int i=0; i < feature_categories_to_features.size(); i++)
            if(feature_categories_to_features[i].valid())
                feature_categories.push_back(i);

        return feature_categories.size() > 0;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature(
        const FeatureLabel &feature_label,
        const FeatureGeometry &feature_geometry,
        Feature &feature
    )
    {
        bool
            status;

        verify_farm_initialization();

        FeatureLabelAndGeometry
            label_and_geometry = FeatureLabelAndGeometry(
                feature_label,
                feature_geometry );

        FeatureLabelsGeometriesToCategories::iterator
            category_itr = feature_labels_and_geometries_to_categories.find(
                label_and_geometry );

        status =
            category_itr != feature_labels_and_geometries_to_categories.end();

        if ( status )
        {
            if ( status=feature_categories_to_features[category_itr->second]
                .valid() )

                feature = feature_categories_to_features[category_itr->second];
        }

        return status;
    }
    // ------------------------------------------------------------------------
     const Feature *FeatureAttributeMapping::get_feature(
        const FeatureLabel &feature_label,
        const FeatureGeometry &feature_geometry)
    {
        bool
            status;

        Feature *feature = 0;

        verify_farm_initialization();

        FeatureLabelAndGeometry
            label_and_geometry = FeatureLabelAndGeometry(
                feature_label,
                feature_geometry );

        FeatureLabelsGeometriesToCategories::iterator
            category_itr = feature_labels_and_geometries_to_categories.find(
                label_and_geometry );

        status =
            category_itr != feature_labels_and_geometries_to_categories.end();

        if ( status )
        {

            if ( feature_categories_to_features[category_itr->second].valid() )
            {
              feature = &(feature_categories_to_features.at(category_itr->second));
            }
        }

        return feature;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_category(
        const FeatureLabel &feature_label,
        const FeatureGeometry &feature_geometry,
        FeatureCategory &feature_category
    )
    {
        bool
            status = farm_initialized;

        verify_farm_initialization();

        if (status)
        {
            FeatureLabelAndGeometry
                label_and_geometry =
                    FeatureLabelAndGeometry(feature_label, feature_geometry );

            FeatureLabelsGeometriesToCategories::iterator
                category_itr = feature_labels_and_geometries_to_categories.
                    find(label_and_geometry);

            status = category_itr !=
                feature_labels_and_geometries_to_categories.end();

            if (status)
            {
                feature_category = category_itr->second;
            }
        }

        return status;
    }

    // ------------------------------------------------------------------------
    FARM::FeatureCategory FeatureAttributeMapping::get_feature_category(
        const FeatureLabel &feature_label,
        const FeatureGeometry &feature_geometry
    )
    {
        verify_farm_initialization();

        FeatureLabelAndGeometry
            label_and_geometry = FeatureLabelAndGeometry(
                feature_label,
                feature_geometry );

        FeatureLabelsGeometriesToCategories::iterator
            category_itr = feature_labels_and_geometries_to_categories.find(
                label_and_geometry );

        FARM::FeatureCategory cat = 0;

        if (category_itr != feature_labels_and_geometries_to_categories.end())
        {
            cat = category_itr->second;
        }

        return cat;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature(
        const FeatureCategory &feature_category,
        Feature &feature
    )
    {
        bool
            status = valid_not_all_feature_category(feature_category);

        if( status )
        {

           if ( status=feature_categories_to_features[feature_category].valid())
            {
                feature = feature_categories_to_features[feature_category];
            }
        }

        return status;
    }

    //--------------------------------------------------------------------------
    const Feature *FeatureAttributeMapping::get_feature(
        const FeatureCategory &feature_category)
    {
        bool
            status = valid_not_all_feature_category(feature_category);

        const Feature *feature = 0;

        if( status )
        {
            if ( feature_categories_to_features[feature_category].valid() )
            {
                feature = &(feature_categories_to_features[feature_category]);
            }
        }

        return feature;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_label(
        const FeatureCategory &feature_category,
        FeatureLabel &feature_label
    )
    {
        const Feature
            *feature=get_feature( feature_category);

        if( feature )
            feature_label = feature->get_label();

        return (feature !=NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_code(
        const FeatureCategory &feature_category,
        FeatureCode &feature_code
    )
    {
        const Feature
            *feature=get_feature( feature_category);

        if( feature )
            feature_code = feature->get_code();

        return (feature !=NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_label_from_code(
        const FeatureCode &code,
        FeatureLabel &label
    )
    {
        FeatureCodesToLabels::iterator
            label_map_iter = feature_codes_to_labels.find(code);

        bool
            status = label_map_iter != feature_codes_to_labels.end();

        ASSERT(status,
            fatal,
            "Could not get the edcs feature label for feature code: "
                + code);

        if (status)
        {
            label = label_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_code(
        const FeatureLabel &label,
        FeatureCode &code
    )
    {
        FeatureLabelsToCodes::iterator
            code_map_iter = feature_labels_to_codes.find(label);

        bool
            status = code_map_iter != feature_labels_to_codes.end();

        ASSERT(status,
            fatal,
            "Could not get the edcs feature code for '" + label + "'.");

        if (status)
        {
            code = code_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_geometry(
        const FeatureCategory &feature_category,
        FeatureGeometry &feature_geometry
    )
    {
        const Feature
            *feature = get_feature(feature_category);

        if( feature )
            feature_geometry = feature->get_geometry();

        return (feature !=NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_usage_bitmask(
        const FeatureCategory &feature_category,
        UsageBitmask &usage_bitmask
    )
    {
        const Feature
            *feature = get_feature( feature_category );

        if( feature )
            usage_bitmask = feature->get_usage_bitmask();

        return (feature != NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_feature_precedence(
        const FeatureCategory &feature_category,
        FeaturePrecedence &feature_precedence
    )
    {
        const Feature
            *feature = get_feature( feature_category );

        if( feature )
            feature_precedence = feature->get_precedence();

        return (feature != NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attributes_overlay_size(
        const FeatureCategory &feature_category,
        int &attributes_overlay_size
    )
    {
        const Feature
            *feature = get_feature( feature_category );

        if( feature )
            attributes_overlay_size = feature->get_attributes_overlay_size();

        return (feature != NULL);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_feature(
        const Feature &feature
    )
    {
        verify_farm_initialization();

        return feature.valid() and CORE::ordered<int>(
            0,
            feature.get_category(),
            feature_categories_to_features.size());
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_feature_category(
        const FeatureCategory &feature_category
    )
    {
        verify_farm_initialization();

        return CORE::ordered<int>(
            0,
            feature_category,
            feature_categories_to_features.size());
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_all_features_category(
        FeatureCategory &feature_category
    )
    {
        verify_farm_initialization();

        feature_category = feature_categories_to_features.size();

        return initialized();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::is_all_features(
        const FeatureCategory &feature_category
    )
    {
        verify_farm_initialization();

        return feature_category == feature_categories_to_features.size();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_all_attributes(
        std::list<Attribute> &attributes
    )
    {
        int
                count=0;
        verify_farm_initialization();

        attributes.clear();

        for(int attr=0; attr< attribute_codes_to_attributes.size(); attr++)
        {
             if(attribute_codes_to_attributes[attr].valid())
             {
            attributes.push_back( attribute_codes_to_attributes[attr] );
             count++;
             }
        }

        return attributes.size() == count;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_all_attribute_categories(
        std::list<AttributeCategory> &attribute_categories
    )
    {
        int
                count=0;
        verify_farm_initialization();

        attribute_categories.clear();

        for(int attr=0; attr< attribute_codes_to_attributes.size(); attr++)
        {
             if(attribute_codes_to_attributes[attr].valid())
             {
            attribute_categories.push_back( attr );
            count++;
             }
        }

        return attribute_categories.size() ==
                count;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute(
        const AttributeCategory &attribute_category,
        Attribute &attribute
    )
    {
        bool
            success = false;

        if (success=attribute_codes_to_attributes[ attribute_category ].valid())
        {
            attribute = attribute_codes_to_attributes[ attribute_category ];
        }

        return success;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_label(
        const AttributeCategory &attribute_category,
        AttributeLabel &attribute_label
    )
    {
        bool
            successful =
                CORE::ordered(
                    0,
                    attribute_category,
                    static_cast<int>(
                        attribute_codes_to_attributes.size() - 1)) and
                attribute_codes_to_attributes[attribute_category].valid();

        if (successful)
        {
            attribute_label =
                attribute_codes_to_attributes[attribute_category].get_label();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_code(
        const AttributeLabel &label,
        AttributeCode &code
    )
    {
        AttributeLabelsToCodes::iterator
            code_map_iter = attribute_labels_to_codes.find(label);
        bool
            status = code_map_iter != attribute_labels_to_codes.end();

        if (status)
        {
            code = code_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_label_from_code(
        const AttributeCode &code,
        AttributeLabel &label
    )
    {
        AttributeCodesToLabels::iterator
            label_map_iter = attribute_codes_to_labels.find(code);

        bool
            status = label_map_iter != attribute_codes_to_labels.end();

        ASSERT(status,
            fatal,
            "Could not get the edcs attribute label for attribute code: "
                + code);

        if (status)
        {
            label = label_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_units(
        const AttributeCategory &attribute_category,
        AttributeUnits &attribute_units
    )
    {
        bool
            successful =
                attribute_codes_to_attributes[attribute_category].valid();

        if (successful)

            attribute_units =
                attribute_codes_to_attributes[attribute_category].get_units();

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::is_editable(
        const AttributeCategory &attribute_category
    )
    {
        bool
            successful =
                attribute_codes_to_attributes[attribute_category].valid();

        if (successful)
        {
           return
           attribute_codes_to_attributes[attribute_category].get_editability();
        }
        else
        {
            return false;
        }

    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute(
        const AttributeLabel &attribute_label,
        Attribute &attribute
    )
    {
        // Look in the cache for the attribute label.
        //
        std::map<AttributeLabel, Attribute>::const_iterator
            iter = attribute_labels_to_attributes.find(attribute_label);
        bool
            successful = iter != attribute_labels_to_attributes.end();

        if (successful)
        {
            // Found the attribute label in the cache.  Return the FARM
            // attribute.
            //
            attribute = iter->second;
        }
        else
        {
            // Could not find the attribute label in the cache.  Get the FARM
            // attribute.
            //
            successful =
                attribute.set_label(attribute_label) and
                attribute.valid();

            if (successful)
            {
                AttributeCode
                    code = attribute.get_code();

                if (attribute_codes_to_attributes.size() < (code + 1))
                {
                    attribute_codes_to_attributes.resize(code + 1);
                }

                // Make sure the editability is set for the attribute.
                //
                attribute = attribute_codes_to_attributes[code];

                successful = attribute.valid();

                if (successful)
                {
                    // Add the attribute label to the cache.
                    //
                    ASSERT(
                        attribute_labels_to_attributes.insert(
                            std::map<AttributeLabel, Attribute>::value_type(
                                attribute_label, attribute)).second,
                        high,
                        "Could not cache an attribute label.");
                }
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_category(
        const AttributeLabel &attribute_label,
        AttributeCategory &attribute_category
    )
    {
        // Try to find the attribute label in the cache.
        //
        std::map<AttributeLabel, Attribute>::const_iterator
            iter = attribute_labels_to_attributes.find(attribute_label);
        bool
            successful = iter != attribute_labels_to_attributes.end();

        if (successful)
        {
            // Find the attribute label in the cache.  Get the attribute
            // category.
            //
            attribute_category = iter->second.get_category();
        }
        else
        {
            // Could not find the attribute label in the cache.  Get the FARM
            // attribute for the label.
            //
            Attribute
                attribute;

            successful = get_attribute(attribute_label, attribute);

            if (successful)
            {
                // Get the attribute category and add the attribute label to
                // the cache.
                //
                attribute_category = attribute.get_code();
            }
            else
            {

                ASSERT(
                    attribute_labels_to_attributes.insert(
                        std::map<AttributeLabel, Attribute>::value_type(
                            attribute_label, attribute)).second,
                    high,
                    "Could not cache an attribute label.");
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attributes(
        const FeatureCategory &feature_category,
        std::list<Attribute> &attributes
    )
    {
        bool
            successful = valid_not_all_feature_category(feature_category);

        if (successful)
        {
            attributes.clear();

            // Find the attributes that are contained in the feature.
            //
            for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
            {
                if (farm[feature_category][attr])
                {
                    attributes.push_back(attribute_codes_to_attributes[attr]);
                }
            }
        }

        return successful and 0 < attributes.size();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_labels(
        const FeatureCategory &feature_category,
        std::list<AttributeLabel> &attribute_labels
    )
    {
        bool
            successful = valid_not_all_feature_category(feature_category);

        if (successful)
        {
            attribute_labels.clear();

            // Find the attributes that are contained in the feature.
            //
            for (int attr=0; attr<attribute_codes_to_attributes.size(); attr++)
            {
                if (farm[feature_category][attr])
                {
                    attribute_labels.push_back(
                        attribute_codes_to_attributes[attr].get_label() );
                }
            }
        }

        return successful and 0 < attribute_labels.size();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_attribute_categories(
        const FeatureCategory &feature_category,
        std::list<AttributeCategory> &attribute_categories
    )
    {
        bool
            successful = valid_not_all_feature_category(feature_category);

        if (successful)
        {
            attribute_categories.clear();

            // Find the attributes that are contained in the feature.
            //
            for (int attr=0; attr<farm[feature_category].size(); attr++)
            {
                if (farm[feature_category][attr])
                {
                    attribute_categories.push_back(attr);
                }
            }
        }

        return successful and 0 < attribute_categories.size();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_string_attributes(
        const FeatureCategory &feature_category,
        std::list<StringAttribute> &string_attributes
    )
    {
        bool
            successful = valid_not_all_feature_category(feature_category);

        if (successful)
        {
            string_attributes.clear();

            // Find the attributes that are strings and are contained in the
            // feature.
            //
           for (int attr=0; attr<farm[feature_category].size(); attr++)
            {
                if (farm[feature_category][attr] and
                    attribute_codes_to_attributes[attr].get_data_type() ==
                    string)
                {
                    string_attributes.push_back(StringAttribute(
                        attr,
                        farm[feature_category][attr]->get_offset()));
                }
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute_category(
        const AttributeCategory &attribute_category
    )
    {
        verify_farm_initialization();

        return  attribute_category >= 0 and
                attribute_category < attribute_codes_to_attributes.size() and
                attribute_codes_to_attributes[attribute_category].valid();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const Attribute &attribute
    )
    {
         return  valid_attribute_category(attribute.get_category());

    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const int attribute_value
    )
    {
        bool
            successful;
        int
            minimum,
            maximum;

        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute.");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute.");

        successful = farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
            get_data_type() == int32;

        if (successful)
        {
            minimum = static_cast<InstantiatedDataType<int> *>(
                farm[feature_category][attribute_category])->get_minimum();
            maximum = static_cast<InstantiatedDataType<int> *>(
                farm[feature_category][attribute_category])->get_maximum();

            successful =  CORE::ordered(minimum, attribute_value, maximum);
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const double attribute_value
    )
    {
        bool
            successful;
        double
            minimum,
            maximum;

        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute.");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute.");

        successful = farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
            get_data_type() == float64;

        if (successful)
        {
            minimum = static_cast<InstantiatedDataType<double> *>(
                farm[feature_category][attribute_category])->get_minimum();
            maximum = static_cast<InstantiatedDataType<double> *>(
                farm[feature_category][attribute_category])->get_maximum();

            successful = CORE::ordered(minimum, attribute_value, maximum);
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const std::string &// Leaving name blank as it is unused parameter and used for function overloading 
    )
    {
        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute.");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute.");

        return farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
            get_data_type() == string;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const Enumerant &attribute_value
    )
    {
        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute.");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute.");

        return
            farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
                get_data_type() == enumeration and
            attribute_value.valid() and
            static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category]
            )->enumerants().find(attribute_value) !=
                static_cast<EnumerantDataType *>(
                    farm[feature_category][attribute_category]
                )->enumerants().end();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const bool // Leaving name blank since its an unused parameter and used for function overloading 
    )
    {
        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute() (bool).");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute() "
                "(bool).");

        return farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
            get_data_type() == boolean;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::valid_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const CORE::UUID & //Leaving name blank since its an unused parameter and used for function overloading
    )
    {
        ASSERT(
            valid_not_all_feature_category(feature_category),
            fatal,
            "Passed an invalid feature category to valid_attribute.");

        ASSERT(
            valid_attribute_category(attribute_category),
            fatal,
            "Passed an invalid attribute category to valid_attribute.");

        return farm[feature_category][attribute_category] and
            attribute_codes_to_attributes[attribute_category].
            get_data_type() == uuid;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_default(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        int &default_value
    )
    {
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                get_data_type() == int32;

        if (successful)
        {
            default_value = static_cast<InstantiatedDataType<int> *>(
                farm[feature_category][attribute_category])->get_default();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_default(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        double &default_value
    )
    {
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                get_data_type() == float64;

        if (successful)
        {
            default_value = static_cast<InstantiatedDataType<double> *>(
                farm[feature_category][attribute_category])->get_default();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_default(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        bool &default_value
    )
    {
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                get_data_type() == boolean;

        if (successful)
        {
            default_value = static_cast<BooleanDataType *>(
                farm[feature_category][attribute_category])->get_default();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_default(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        Enumerant &default_value
    )
    {
            EnumerantCode
                ee_code;
            bool
                successful =
                    valid_not_all_feature_category(feature_category) and
                    valid_attribute_category(attribute_category) and
                    contains_attribute(
                        feature_category,
                        attribute_category) and
                    attribute_codes_to_attributes[attribute_category].
                        get_data_type() == enumeration;

            if (successful)
            {
                ee_code = static_cast<EnumerantDataType *>(
                    farm[feature_category][attribute_category])->get_default();
                successful = default_value.set_codes(
                    attribute_category,
                    ee_code);
            }

            return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_min_max(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        int &minimum_value,
        int &maximum_value
    )
    {
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == int32;

        if (successful)
        {
            // The feature contains the attribute and the attribute is an
            // Int32.  Get the minimum and maximum values.
            //
            minimum_value = static_cast<InstantiatedDataType<CORE::Int32> *>(
                farm[feature_category][attribute_category])->get_minimum();
            maximum_value = static_cast<InstantiatedDataType<CORE::Int32> *>(
                farm[feature_category][attribute_category])->get_maximum();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_min_max(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        double &minimum_value,
        double &maximum_value
    )
    {
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == float64;

        if (successful)
        {
            // The feature contains the attribute and the attribute is a
            // Float64.  Get the minimum and maximum values.
            //
            minimum_value = static_cast<InstantiatedDataType<CORE::Float64> *>(
                farm[feature_category][attribute_category])->get_minimum();
            maximum_value = static_cast<InstantiatedDataType<CORE::Float64> *>(
                farm[feature_category][attribute_category])->get_maximum();
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_valid_enumerants(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        std::list<Enumerant> &enumerants
    )
    {
        Enumerants::const_iterator
            enum_values_iter;
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(
                    feature_category,
                    attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == enumeration;

        if (successful)
        {
            // The feature contains the attribute and the attribute is an
            // enumeration.  Calculate the valid enumeration strings.
            //
            enumerants.clear();

            enum_values_iter = static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                begin();

            // Loop through the valid enumeration values for the attribute and
            // calculate the enumeration strings.
            //
            while (enum_values_iter != static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                end())
            {
                enumerants.push_back( *enum_values_iter );

                ++enum_values_iter;
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_valid_enumerant_strings(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        std::list<EnumerantLabel> &enumerant_strings
    )
    {
        Enumerants::const_iterator
            enum_values_iter;
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(
                    feature_category,
                    attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == enumeration;

        if (successful)
        {
            // The feature contains the attribute and the attribute is an
            // enumeration.  Calculate the valid enumeration strings.
            //
            enumerant_strings.clear();

            enum_values_iter = static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                begin();

            // Loop through the valid enumeration values for the attribute and
            // calculate the enumeration strings.
            //
            while (enum_values_iter != static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                end())
            {
                enumerant_strings.push_back( enum_values_iter->get_ee_label() );

                ++enum_values_iter;
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_enumeration_value(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const EnumerantLabel &enumerant_label,
        Enumerant &enum_value
    )
    {
        AttributeLabel
            attribute_label;
        Enumerant
            enum_tmp;
        Enumerants::const_iterator
            enum_itr;
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == enumeration;

        if (successful)
        {
            // The feature contains the attribute and the attribute is an
            // enumeration.  Get the enumeration value.
            //
            attribute_label=
                attribute_codes_to_attributes[attribute_category].get_label();

            enum_tmp = Enumerant( attribute_label, enumerant_label );

            ASSERT(
                enum_tmp.valid(),
                info,
                "Attribute Label and Enumerant Label do not make a valid "
                    "Enumerant: EA_Label = " + attribute_label +
                    "; EE_Label = " + enumerant_label);

            enum_itr = static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                    find(enum_tmp);

            ASSERT(
                enum_itr != static_cast<EnumerantDataType *>(
                    farm[feature_category][attribute_category])->
                        enumerants().end(),
                info,
                "The enumerant '" + enumerant_label + "' is not valid.");

            if (successful)
            {
                enum_value = enum_tmp;
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_enumeration_value(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        const EnumerantCode &enumerant_code,
        Enumerant &enum_value
    )
    {
        Enumerant
            enum_tmp;
        Enumerants::const_iterator
            enum_itr;
        bool
            successful =
                valid_not_all_feature_category(feature_category) and
                valid_attribute_category(attribute_category) and
                contains_attribute(feature_category, attribute_category) and
                attribute_codes_to_attributes[attribute_category].
                    get_data_type() == enumeration;

        if (successful)
        {
            // The feature contains the attribute and the attribute is an
            // enumeration.  Get the enumeration value.
            //
            enum_tmp = Enumerant(attribute_category, enumerant_code);

            ASSERT_WITH_STREAM(
                enum_tmp.valid(),
                info,
                "Attribute Label and Enumerant Label do not make a valid "
                    "Enumerant: EA_Code = " << attribute_category <<
                    "; EE_Code = " << enumerant_code);

            enum_itr = static_cast<EnumerantDataType *>(
                farm[feature_category][attribute_category])->enumerants().
                    find(enum_tmp);

            ASSERT_WITH_STREAM(
                enum_itr != static_cast<EnumerantDataType *>(
                    farm[feature_category][attribute_category])->
                        enumerants().end(),
                info,
                "The enumerant '" << enumerant_code << "' is not valid.");

            if (successful)
            {
                enum_value = enum_tmp;
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_enumerant_label(
        const AttributeLabel &attrib_label,
        const EnumerantCode &enumerant_code,
        EnumerantLabel &enumerant_label
    )
    {
        AttributeEnumCodePair
            code(attrib_label, enumerant_code);

        EnumerantCodesToLabels::iterator
            label_map_iter = enum_codes_to_labels.find(code);

        bool
            status = label_map_iter != enum_codes_to_labels.end();

        ASSERT(status,
            fatal,
            "Could not get the edcs enumerant label for attribute: "
                + attrib_label + ", enumerant code: "
                + CORE::to_string(enumerant_code));

        if (status)
        {
            enumerant_label = label_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_enumerant_code(
        const AttributeLabel &attrib_label,
        const EnumerantLabel &enumerant_label,
        EnumerantCode &enumerant_code
    )
    {
        AttributeEnumPair
            label(attrib_label, enumerant_label);

        EnumerantLabelsToCodes::iterator
            code_map_iter = enum_labels_to_codes.find(label);
        bool
            status = code_map_iter != enum_labels_to_codes.end();

        if (status)
        {
            enumerant_code = code_map_iter->second;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::get_offsets_and_data_types(
        const FeatureCategory &feature_category,
        OffsetsAndDataTypes &offsets_and_data_types
    )
    {
        bool
            successful = valid_not_all_feature_category(feature_category);

        verify_farm_initialization();

        if (successful)
        {
            offsets_and_data_types.clear();

            // Get the offsets and data types for the attributes in the
            // feature.
            //
            for (int attr=0;attr<attribute_codes_to_attributes.size();attr++)
            {
                if (farm[feature_category][attr])
                {
                    // The feature contains the attribute.

                    offsets_and_data_types.push_back(OffsetAndDataType(
                        farm[feature_category][attr]->get_offset(),
                     attribute_codes_to_attributes[attr].get_data_type()));
                }
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::read(
        const std::string &database_directory,
        const std::string &config_directory,
        std::string &failure_reason
    )
    {
        CORE::UInt16
            little_endian;
        const CORE::Version
            attr_vers(CORE::Version::land_version);
        CORE::Version
            file_vers(CORE::Version::land_version);
        std::ifstream
            file;
        bool
            edcs_initialized = false;

        if (failure_reason == "" and not initialized())
        {
            // Initialize the EDCS-related maps
            //
            edcs_initialized = initialize_edcs_maps(
                config_directory + "/edcs_3p1_4p3_feature_mapping.cfg",
                config_directory + "/edcs_3p1_4p3_attribute_mapping.cfg",
                config_directory +
                    "/edcs_3p1_4p3_attribute_enumerant_mapping.cfg");

            ASSERT(
                edcs_initialized,
                fatal,
                "Could not initialize the EDCS data.");

            file.open(
                (database_directory + "/otf/" + farm_file_label).c_str(),
                std::ios::in | std::ios::binary);

            if (not file.is_open())
            {
                failure_reason =
                    "Could not open the file '" + farm_file_label + "'.";

                LOG(fatal, failure_reason);
            }

            // Read and check the endianness for the FARM.
            //
            if (failure_reason == "")
            {
                file.read(
                    reinterpret_cast<char *>(&little_endian),
                    sizeof(little_endian));

                if (little_endian != CORE::little_endian())
                {
                    failure_reason =
                        "The endianness for the FARM is incorrect.";

                    LOG(fatal, failure_reason);
                }
            }

            // Read and check the version of the file.
            //
            if (failure_reason == "")
            {
                if (not file_vers.read(file))
                {
                    LOG(fatal,
                        "Could not read the version of the terrain database!");

                    failure_reason = "Could not read the version of the "
                        "terrain database!";
                }
                else
                {
                    if (file_vers != attr_vers)
                    {
                        std::cout << "FARM File Version:" << std::endl;
                        file_vers.display(std::cout);
                        std::cout << std::endl;

                        std::cout << "FARM Software Version:" << std::endl;
                        attr_vers.display(std::cout);
                        std::cout << std::endl;

                        std::string
                            file_msg =
                                CORE::to_string(file_vers.get_version()) + "." +
                                CORE::to_string(file_vers.get_format()) + "." +
                                CORE::to_string(file_vers.get_update());
                        std::string
                            sw_msg =
                                CORE::to_string(attr_vers.get_version()) + "." +
                                CORE::to_string(attr_vers.get_format()) + "." +
                                CORE::to_string(attr_vers.get_update());
                        std::string
                            out_msg = "The FARM file version: " + file_msg +
                                      " and the software version: " + sw_msg +
                                     " are not the same!";

                        LOG(high, out_msg);
                    }
                }
            }

            if (failure_reason == "")
            {
                // Read the 2-dimensional feature to attribute mapping array.
                //
                read_farm_table(file);

                // Read the data structures for the feature categories.
                //
                read_map<
                    FeatureLabelAndGeometry,
                    FeatureLabelAndGeometry,
                    FeatureCategory,
                    CORE::UInt16>(
                        file, feature_labels_and_geometries_to_categories);

                typedef std::map<FeatureCategory, Feature>
                        CtoFtype;
                CtoFtype CtoF;
                read_map<FeatureCategory, CORE::UInt16, Feature, Feature>(
                    file, CtoF);

                 // Put it into vector
                 //
                 feature_categories_to_features.resize(CtoF.size());
                 for (CtoFtype::const_iterator code_itr = CtoF.begin();
                      code_itr != CtoF.end();
                      ++code_itr)
                 {
                      feature_categories_to_features[code_itr->first]=
                            code_itr->second;

                 }
                // Read the data structures for the attribute categories.
                //
                typedef std::map<AttributeCode, Attribute>
                AtoAtype;
                AtoAtype AtoA;

                read_map<
                    AttributeCode,
                    CORE::UInt16,
                    Attribute,
                    Attribute>(file, AtoA);

                 // Put it into vector
                 //
                  int
                        code_max=0;

                  for (AtoAtype::const_iterator code_itr = AtoA.begin();
                      code_itr != AtoA.end();
                      ++code_itr)

                      if(code_itr->first > code_max)
                          code_max=code_itr->first;

                 attribute_codes_to_attributes.resize(code_max+1);
                 for (AtoAtype::const_iterator code_itr = AtoA.begin();
                      code_itr != AtoA.end();
                      ++code_itr)

                          attribute_codes_to_attributes[code_itr->first]=
                                code_itr->second;

            }

            farm_initialized = failure_reason == "";

            if (farm_initialized)
            {
                ASSERT(
                    FeatureCategories::initialize(),
                    fatal,
                    "Could not initialize the feature categories.");

                ASSERT(
                    AttributeCategories::initialize(),
                    fatal,
                    "Could not initialize the attribute categories.");

                // Initialize the enumerations that are commonly used.
                //
                ASSERT(
                    EnumValues::initialize(),
                    fatal,
                    "Could not initialize the commonly used enumeration "
                        "values.");

                ASSERT(
                    updated_usage_bitmask(
                        "vehObstacle",
                        feature_categories_to_features[
                            FeatureCategories::terrain_crater()]),
                    fatal,
                    "Could not add vehObstacle to terrain crater usage bitmask.");

                attribute_labels_to_attributes.clear();
            }
            else
            {
                ASSERT(farm_initialized,fatal,failure_reason);
            }
        }

        return initialized();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::write(
        const std::string &database_directory
    )
    {
        std::ofstream
            file(
                (database_directory + "/otf/" + farm_file_label).c_str(),
                std::ios::out | std::ios::binary);

        ASSERT(
            file.is_open(),
            fatal,
            "Could not open the file for the FARM!");

        verify_farm_initialization();

        // Write the endianness for the FARM.
        //
        CORE::UInt16
            little_endian = CORE::little_endian() ? 1 : 0;

        file.write(
            reinterpret_cast<char *>(&little_endian), sizeof(little_endian));

        // Write the version of the file.
        //
        CORE::Version
            version(CORE::Version::land_version);

        version.write(file);

        // Write the 2-dimensional feature to attribute mapping array.
        //
        write_farm_table(file);

        // Write the data structures for the feature categories.
        //
        write_map<
            FeatureLabelAndGeometry,
            FeatureLabelAndGeometry,
            FeatureCategory,
            CORE::UInt16>(file, feature_labels_and_geometries_to_categories);
        // Convert vector to map for writing to DB
        //
        typedef std::map<FeatureCategory, Feature>
            CtoFtype;
        CtoFtype CtoF;
         for (int i=0; i<feature_categories_to_features.size(); i++)
             if(feature_categories_to_features[i].valid())
                 CtoF.insert(CtoFtype::value_type(
                        i,
                        feature_categories_to_features[i]));
        write_map<FeatureCategory, CORE::UInt16, Feature, Feature>(file, CtoF);

        // Write the data structures for the attribute categories.
        //
        typedef std::map<AttributeCode, Attribute>
        AtoAtype;
        AtoAtype AtoA;
         for (int attr=0; attr < attribute_codes_to_attributes.size(); attr++)
             if(attribute_codes_to_attributes[attr].valid())
                 AtoA.insert(AtoAtype::value_type(
                        attr,
                        attribute_codes_to_attributes[attr]));
        write_map<
            AttributeCode,
            CORE::Int32,
            Attribute,
            Attribute>(file, AtoA);

        return file.good();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::dump(const std::string &output_dir)
    {
        std::cout << "Dumping the FARM." << std::endl;

        bool
            successful = initialized();

        ASSERT(
            successful, high, "The FARM was used before it was initialized.");

        if (successful)
        {
            // Create the output directory.
            //
            successful = CORE::DirectoryParser::verify_dir(output_dir);

            ASSERT(
                successful,
                high,
                "Could not create the directory '" + output_dir + "'.");

            if (successful)
            {
                // Create the file to write the FARM to.
                //
                std::string
                    file_name = output_dir + "/" + farm_binary_input_file;
                std::ofstream
                    file(file_name.c_str(), std::ios::out | std::ios::binary);

                // Verify that the file was created successfully.
                //
                successful = file.is_open();

                ASSERT(
                    successful,
                    high,
                    "Could not create the file '" + file_name + "'.");

                if (successful)
                {
                    file.precision(20);

                    // Write the tables and maps in the FARM to the file.
                    //
                    dump_feature_labels_geometries_to_categories(file);
                    dump_feature_categories_to_features(file);
                    dump_attribute_codes_to_attributes(file);
                    dump_attribute_codes_to_enums(file);
                    dump_farm_table(file);
                }
            }
        }

        return successful;
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::integrated_feature(
        const FeatureCategory &feature_category
    )
    {
        FeatureGeometry
            geometry;

        ASSERT(
            initialized(),
            fatal,
            "The FARM was used before it was initialized.");

        if (initialized())
        {
            ASSERT(
                get_feature_geometry(feature_category, geometry),
                fatal,
                "Could not get the geometry for a feature.");
        }

        // The feature needs to be integrated if it is a linear feature with a
        // trafficability code and a width or an areal feature with a
        // trafficability code.  Bridges, causeways, and overpasses can not be
        // integrated.  For now a check must be done specifically for railroads
        // and railroad sidetracks because they do not have a width in the EDM,
        // but they are integrated in the JRTC and NTC databases.  This needs
        // to be fixed.  Also vehicle lots are hardcoded because they do not
        // contain an STGJ.  This also needs to be fixed.
        //
        return
            initialized() and
            (contains_attribute(
                feature_category,
                AttributeCategories::trafficability_fine()) or
            feature_category == FeatureCategories::vehicle_lot()) and
            (geometry == areal or
            (geometry == linear and
            not is_bridge(feature_category) and
            (contains_attribute(
                feature_category, AttributeCategories::width()) or
            feature_category == FeatureCategories::railroad() or
            feature_category == FeatureCategories::railroad_sidetrack())));
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::integrated_linear(
        const FeatureCategory &feature_category
    )
    {
        ASSERT(
            initialized(),
            fatal,
            "The FARM was used before it was initialized.");

        FeatureGeometry
            geometry;

        ASSERT(
            get_feature_geometry(feature_category, geometry),
            fatal,
            "Could not get the geometry for a feature.");

        return geometry == linear and integrated_feature(feature_category);
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::is_bridge(
        const FeatureCategory &feature_category
    )
    {
        ASSERT(
            initialized(),
            fatal,
            "The FARM was used before it was initialized.");

        return
            feature_category == FeatureCategories::engineer_bridge()
            or
            feature_category == FeatureCategories::bridge()
            or
            feature_category == FeatureCategories::causeway()
            or
            feature_category == FeatureCategories::overpass();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::has_spans_or_piers(
        const FeatureCategory &feature_category
    )
    {
        ASSERT(
            initialized(),
            fatal,
            "The FARM was used before it was initialized.");

        // Note, an OVERPASS does not have a COMPLEX_COMPONENT_IDENTIFIER or 
        // BRIDGE_SPAN_COUNT attribute, but it may have piers.

        return
            feature_category == FeatureCategories::bridge()
            or
            feature_category == FeatureCategories::causeway()
            or
            feature_category == FeatureCategories::overpass();
    }

    // ------------------------------------------------------------------------
    bool FeatureAttributeMapping::is_tunnel(
        const FeatureCategory &feature_category
    )
    {
        ASSERT(
            initialized(),
            fatal,
            "The FARM was used before it was initialized.");

        return
            feature_category == FeatureCategories::tunnel() or
            feature_category == FeatureCategories::underground_railroad();
    }
}

